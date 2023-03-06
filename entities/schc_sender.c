#include <stdlib.h>
#include <stdio.h>
#include "schc_sender.h"

void init_sender(SCHCSender *s) {
    s->attempts = 0;
    s->nb_fragments = 0;
    s->last_window = 0;
    sgfx_client_start(&s->socket);
    s->transmission_q = NULL;
    s->retransmission_q = NULL;
    s->rt = 0;
    s->ul_loss_rate = UPLINK_LOSS_RATE;
    s->dl_loss_rate = DOWNLINK_LOSS_RATE;
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
    s->rt = !is_frg_null(&s->retransmission_q[0]);
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