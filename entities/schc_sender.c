#include <stdlib.h>
#include <stdio.h>
#include "schc_sender.h"
#include "casting.h"
#include "misc.h"

int
sender_construct(SCHCSender *s, Rule *rule, char schc_packet[], int byte_size) {
    sgfx_client_start(&s->socket);

    s->nb_fragments = get_nb_fragments(rule, byte_size);
    s->last_window = get_nb_windows(rule, byte_size);
    s->fragments = malloc(sizeof(Fragment) * s->nb_fragments);

    if (fragment(rule, s->fragments, schc_packet, byte_size) < 0) {
        printf("Fragment procedure failed.\n");
        return -1;
    }

    fq_construct(&s->transmission_q, s->nb_fragments + 1);
    fq_construct(&s->retransmission_q, s->nb_fragments + 1);

    s->attempts = 0;
    s->rt = 0;
    s->ul_loss_rate = UPLINK_LOSS_RATE;
    s->dl_loss_rate = DOWNLINK_LOSS_RATE;

    return 0;
}

void sender_destroy(SCHCSender *s) {
    sgfx_client_close(&s->socket);
    free(s->fragments);
    s->fragments = 0;
    fq_destroy(&s->transmission_q);
    fq_destroy(&s->retransmission_q);
}

ssize_t schc_send(SCHCSender *s, Rule *rule, Fragment *frg) {
    if (s->ul_loss_rate > 0
        && !is_frg_sender_abort(rule, frg)
        && random() % 101 < s->ul_loss_rate) {
        s->socket.seqnum += 1;
        printf("Fragment lost (rate)\n");
        return frg->byte_size;
    }
    return sgfx_client_send(&s->socket, frg->message, frg->byte_size);
}

ssize_t schc_recv(SCHCSender *s, Rule *rule, CompoundACK *dest) {
    if (!s->socket.expects_ack) return 0;

    char received[DOWNLINK_MTU_BYTES];
    ssize_t retval = sgfx_client_recv(&s->socket, received);
    memcpy(dest->message, received, DOWNLINK_MTU_BYTES);

    if (s->dl_loss_rate > 0 && random() % 101 < s->dl_loss_rate) {
        printf("ACK lost (rate)\n");
        return -1;
    }
    return retval;
}

void update_rt(SCHCSender *s) {
    s->rt = !fq_is_empty(&s->retransmission_q);
}

void update_timeout(SCHCSender *s, Rule *rule, Fragment *frg) {
    if (is_frg_all_0(rule, frg) && !s->rt) {
        sgfx_client_set_timeout(&s->socket, SIGFOX_DL_TIMEOUT);
    } else if (is_frg_all_1(rule, frg)) {
        sgfx_client_set_timeout(&s->socket, RETRANSMISSION_TIMEOUT);
    } else {
        sgfx_client_set_timeout(&s->socket, 60);
    }
}

int get_bitmap_to_retransmit(SCHCSender *s, Rule *rule, int ack_window,
                             char bitmap[], char dest[]) {
    int frgs_arent_missing;
    int bitmap_sz;

    if (ack_window != s->last_window) {
        bitmap_sz = rule->window_size;
        frgs_arent_missing = is_monochar(bitmap, '1');
        strncpy(dest, bitmap, rule->window_size);
    } else {
        bitmap_sz = (s->nb_fragments - 1) % rule->window_size;
        char bitmap_last[bitmap_sz + 1];
        strncpy(bitmap_last, bitmap, bitmap_sz);
        bitmap_last[bitmap_sz] = '\0';

        int only_has_all1 = strcmp(bitmap_last, "") == 0 &&
                            bitmap[rule->window_size - 1] == '1';
        int is_ones = is_monochar(bitmap_last, '1');
        frgs_arent_missing = only_has_all1 || is_ones;
        strncpy(dest, bitmap_last, bitmap_sz);
    }

    return frgs_arent_missing ? -1 : bitmap_sz;
}

int
update_rt_queue(SCHCSender *s, Rule *rule, Fragment *frg, CompoundACK *ack) {
    int nb_tuples = get_ack_nb_tuples(rule, ack);
    char windows[nb_tuples][rule->m + 1];
    char bitmaps[nb_tuples][rule->window_size + 1];
    get_ack_tuples(rule, ack, nb_tuples, windows, bitmaps);

    char frg_w[rule->m + 1];
    get_frg_w(rule, frg, frg_w);
    int frg_wdw = bin_to_int(frg_w);
    int greatest_ack_wdw = bin_to_int(windows[nb_tuples - 1]);
    if (greatest_ack_wdw > frg_wdw) {
        printf("Compound ACK reported losses of a window greater than "
               "that of the ACK-REQ (%d > %d)\n", greatest_ack_wdw, frg_wdw);
        return -1;
    }

    for (int i = 0; i < nb_tuples; i++) {
        int nb_ack_window = bin_to_int(windows[i]);
        char bitmap[rule->window_size + 1];
        strncpy(bitmap, bitmaps[i], rule->window_size);
        bitmap[rule->window_size] = '\0';

        char bitmap_to_retransmit[rule->window_size + 1];
        memset(bitmap_to_retransmit, '\0', rule->window_size + 1);

        int bitmap_sz = get_bitmap_to_retransmit(s, rule, nb_ack_window, bitmap,
                                                 bitmap_to_retransmit);
        if (bitmap_sz < 0) {
            printf("Compound ACK reported no missing Fragment.\n");
            Fragment sender_abort;
            generate_sender_abort(rule, frg, &sender_abort);
            fq_put(&s->transmission_q, &sender_abort);
            return 0;
        }

        for (int j = 0; j < bitmap_sz; j++) {
            if (bitmap_to_retransmit[j] == '0') {
                int idx = rule->window_size * nb_ack_window + j;
                fq_put(&s->retransmission_q, &s->fragments[idx]);
            }
        }
    }

    if (is_frg_all_1(rule, frg)) {
        fq_put(&s->transmission_q, frg);
    }

    return 0;
}