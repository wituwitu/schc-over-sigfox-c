#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "schc_session.h"
#include "misc.h"
#include "casting.h"

int state_construct(State *state, Rule rule) {

    Fragment last_fragment;
    CompoundACK last_ack;
    CompoundACK receiver_abort;

    generate_null_frg(&last_fragment);
    generate_null_ack(&last_ack);
    generate_null_ack(&receiver_abort);

    memcpy(&state->last_fragment, &last_fragment, sizeof(Fragment));
    memcpy(&state->last_ack, &last_ack, sizeof(CompoundACK));
    memcpy(&state->receiver_abort, &receiver_abort, sizeof(CompoundACK));

    state->aborted = 0;
    state->timestamp = -1;

    state->bitmap = calloc(rule.max_fragment_number + 1, sizeof(char));
    memset(state->bitmap, '0', rule.max_fragment_number);
    state->bitmap[rule.max_fragment_number] = '\0';

    state->requested_frg = calloc(rule.max_fragment_number + 1, sizeof(char));
    memset(state->requested_frg, '0', rule.max_fragment_number);
    state->requested_frg[rule.max_fragment_number] = '\0';

    return 0;
}

int state_destroy(State *state) {
    free(state->requested_frg);
    free(state->bitmap);
    state = NULL;

    return 0;
}

int session_construct(SCHCSession *s, Rule rule) {
    State state;
    state_construct(&state, rule);
    s->rule = rule;

    CompoundACK ack;
    generate_null_ack(&ack);
    memcpy(&s->ack, &ack, sizeof(CompoundACK));
    memcpy(&s->state, &state, sizeof(State));

    s->fragments = calloc(rule.max_fragment_number + 1, sizeof(Fragment));
    for (int i = 0; i <= rule.max_fragment_number; i++) {
        Fragment frg;
        generate_null_frg(&frg);
        memcpy(&s->fragments[i], &frg, sizeof(Fragment));
    }

    return 0;
}

int session_destroy(SCHCSession *s) {
    free(s->fragments);
    state_destroy(&s->state);
    s = NULL;

    return 0;
}

int session_sender_abort(SCHCSession *s) {
    s->state.timestamp = -1;
    s->state.aborted = 1;
    return 0;
}

int session_receiver_abort(SCHCSession *s) {
    s->state.timestamp = -1;
    s->state.aborted = 1;
    CompoundACK ra;
    generate_receiver_abort(&s->rule, &ra);
    memcpy(&s->ack, &ra, sizeof(CompoundACK));
    memcpy(&s->state.receiver_abort, &ra, sizeof(CompoundACK));
    return 0;
}

int session_was_aborted(SCHCSession *s) {
    return s->state.aborted == 1;
}

void session_update_timestamp(SCHCSession *s, time_t timestamp) {
    s->state.timestamp = (int) timestamp;
}

int session_expired_inactivity_timeout(SCHCSession *s, time_t timestamp) {
    if (DISABLE_INACTIVITY_TIMEOUT || s->state.timestamp < 0) return 0;

    return (float) abs((int) timestamp - s->state.timestamp) >
           INACTIVITY_TIMEOUT;
}

int session_check_requested_fragment(SCHCSession *s, Fragment *frg) {
    int idx = get_frg_nb(&s->rule, frg);
    return s->state.requested_frg[idx] == '1';
}

int session_already_received(SCHCSession *s, Fragment *frg) {
    int idx = get_frg_nb(&s->rule, frg);
    if (s->state.bitmap[idx] == '1') return 1;

    if (is_frg_all_1(&s->rule, frg)) {
        return s->state.last_ack.byte_size != 0
               && is_ack_complete(&s->rule, &s->state.last_ack)
               && frg_equal(&s->state.last_fragment, frg);
    }

    return 0;
}

int session_expects_fragment(SCHCSession *s, Fragment *frg) {
    if (!is_frg_all_1(&s->rule, frg) && session_already_received(s, frg))
        return 0;

    if (is_frg_null(&s->state.last_fragment)
        || is_frg_sender_abort(&s->rule, &s->state.last_fragment))
        return 1;

    if (is_frg_all_1(&s->rule, &s->state.last_fragment))
        return is_ack_complete(&s->rule, &s->state.last_ack);

    if (session_check_requested_fragment(s, frg)) return 1;

    int frg_window = get_frg_window(&s->rule, frg);
    int last_frg_window = get_frg_window(&s->rule, &s->state.last_fragment);

    if (frg_window > last_frg_window) return 1;
    if (frg_window == last_frg_window) {
        int frg_nb = get_frg_nb(&s->rule, frg);
        int last_frg_nb = get_frg_nb(&s->rule, &s->state.last_fragment);

        if (frg_nb > last_frg_nb) return 1;
        if (frg_nb == last_frg_nb) return is_frg_all_1(&s->rule, frg);
    }

    return 0;
}

int start_new_session(SCHCSession *s, int retain_last_data) {
    Fragment last_frg;
    CompoundACK last_ack;

    if (retain_last_data) {
        memcpy(&last_frg, &s->state.last_fragment, sizeof(Fragment));
        memcpy(&last_ack, &s->state.last_ack, sizeof(CompoundACK));
    }

    free(s->fragments);
    state_destroy(&s->state);
    session_construct(s, s->rule);

    if (retain_last_data) {
        memcpy(&s->state.last_fragment, &last_frg, sizeof(Fragment));
        memcpy(&s->state.last_ack, &last_ack, sizeof(CompoundACK));
    }

    return 0;
}

/*
int session_check_pending_ack(SCHCSession *s, Fragment *frg) {
    if (is_ack_null(&s->state.last_ack)) return 0;

    if (!is_ack_complete(&s->rule, &s->state.last_ack)) {
        printf("Last ACK was not complete");
        return 0;
    }

    if (!is_frg_all_1(&s->rule, frg)) {
        generate_null_frg(&s->state.last_fragment);
        generate_null_ack(&s->state.last_ack);
        return 0;
    }

    return is_frg_all_1(&s->rule, &s->state.last_fragment);
}

void session_store_frg(SCHCSession *s, Fragment *frg) {
    if (session_already_received(s, frg)) return;

    int frg_nb = get_frg_nb(&s->rule, frg);
    memcpy(&s->fragments[frg_nb], frg, sizeof(Fragment));
}

void session_get_bitmap(SCHCSession *s, Fragment *frg, char dest[]) {
    int frg_window = get_frg_window(&s->rule, frg);
    int bm_start_idx = frg_window * s->rule.window_size;
    memset(dest, '\0', s->rule.window_size + 1);
    memcpy(dest, s->state.bitmap + bm_start_idx, s->rule.window_size);
}

void session_update_bitmap(SCHCSession *s, Fragment *frg) {
    int frg_nb = get_frg_nb(&s->rule, frg);
    replace_char(s->state.bitmap, frg_nb, '1');
}

int session_update_requested(SCHCSession *s, CompoundACK *ack) {
    if (is_ack_complete(&s->rule, ack)) return -1;

    int nb_tuples = get_ack_nb_tuples(&s->rule, ack);
    char windows[nb_tuples][s->rule.m + 1];
    char bitmaps[nb_tuples][s->rule.window_size + 1];
    get_ack_tuples(&s->rule, ack, nb_tuples, windows, bitmaps);

    for (int i = 0; i < nb_tuples; i++) {
        int window_nb = bin_to_int(windows[i]);
        int bm_idx = window_nb * s->rule.window_size;

        for (int j = 0; j < s->rule.window_size; j++) {
            if (bitmaps[i][j] == '1') {
                replace_char(s->state.requested_frg,
                             bm_idx + j,
                             '1');
            }
        }
    }

    return 0;
}

int session_check_bitmaps(
        SCHCSession *s,
        Fragment *frg,
        char bitmaps[s->rule.max_window_number][s->rule.window_size + 1]
) {
    int curr_window = get_frg_window(&s->rule, frg);
    int nb_tuples = 0;

    for (int i = 0; i < s->rule.max_window_number; i++) {
        memset(bitmaps[i], '\0', s->rule.window_size + 1);
    }

    for (int i = 0; i < curr_window + 1; i++) {
        char bitmap[s->rule.window_size + 1];
        session_get_bitmap(s, frg, bitmap);
        int lost = 0;

        if (is_frg_all_1(&s->rule, frg) && i == curr_window) {
            char rcs[s->rule.u + 1];
            get_frg_rcs(&s->rule, frg, rcs);
            int frgs_in_last_window = bin_to_int(rcs);
            char expected_bitmap[s->rule.window_size + 1];

            memset(expected_bitmap, '\0', s->rule.window_size + 1);
            memset(expected_bitmap, '1', frgs_in_last_window - 1);
            memset(expected_bitmap + frgs_in_last_window - 1, '0',
                   s->rule.window_size - frgs_in_last_window);
            expected_bitmap[s->rule.window_size] = '1';

            if (strcmp(bitmap, expected_bitmap) != 0) lost = 1;
        } else if (!is_monochar(bitmap, '1')) lost = 1;

        if (lost) {
            nb_tuples++;
            memcpy(bitmaps[i], bitmap, s->rule.window_size + 1);
        }
    }

    return nb_tuples > 0;
}

void session_generate_ack(SCHCSession *s, Fragment *frg) {
    char bitmaps[s->rule.max_window_number][s->rule.window_size + 1];

    int lost = session_check_bitmaps(s, frg, bitmaps);
    int wdw = get_frg_window(&s->rule, frg);

    char c = lost ? '0' : '1';

    generate_ack(&s->rule, &s->ack, wdw, c, bitmaps);
}

int session_schc_recv(SCHCSession *s, Fragment *frg, time_t timestamp) {

    if (session_was_aborted(s)) return SCHC_RECEIVER_ABORTED;

    if (session_expired_inactivity_timeout(s, timestamp)) {
        session_receiver_abort(&s);
        return SCHC_RECEIVER_ABORTED;
    }

    session_update_timestamp(s, timestamp);

    if (is_frg_sender_abort(&s->rule, frg)) {
        session_sender_abort(&s);
        return SCHC_SENDER_ABORTED;
    }

    if (!session_expects_fragment(s, frg))
        start_new_session(s, 0);

    session_store_frg(s, frg);
    session_update_bitmap(s, frg);

    if (session_check_pending_ack(s, frg)) {
        memcpy(&s->ack, &s->state.last_ack, sizeof(CompoundACK));
        return 1;
    }

    if (!frg_expects_ack(&s->rule, frg)
        || session_check_requested_fragment(s, frg))
        return 0;

    session_generate_ack(s, frg);
    return 1;
}
*/