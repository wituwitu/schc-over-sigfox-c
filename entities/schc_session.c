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

    state->bitmap = calloc(rule.max_fragment_nb + 1, sizeof(char));
    memset(state->bitmap, '0', rule.max_fragment_nb);
    state->bitmap[rule.max_fragment_nb] = '\0';

    state->requested_frg = calloc(rule.max_fragment_nb + 1, sizeof(char));
    memset(state->requested_frg, '0', rule.max_fragment_nb);
    state->requested_frg[rule.max_fragment_nb] = '\0';

    return 0;
}

int state_destroy(State *state) {
    free(state->requested_frg);
    free(state->bitmap);
    state = NULL;

    return 0;
}

int session_construct(SCHCSession *session, Rule rule) {
    State state;
    state_construct(&state, rule);
    session->rule = rule;

    CompoundACK ack;
    generate_null_ack(&ack);
    memcpy(&session->ack, &ack, sizeof(CompoundACK));
    memcpy(&session->state, &state, sizeof(State));

    session->fragments = calloc(rule.max_fragment_nb + 1, sizeof(Fragment));
    for (int i = 0; i <= rule.max_fragment_nb; i++) {
        Fragment frg;
        generate_null_frg(&frg);
        memcpy(&session->fragments[i], &frg, sizeof(Fragment));
    }

    return 0;
}

int session_destroy(SCHCSession *session) {
    free(session->fragments);
    state_destroy(&session->state);
    session = NULL;

    return 0;
}

int session_sender_abort(SCHCSession *session) {
    session->state.timestamp = -1;
    session->state.aborted = 1;
    return 0;
}

int session_receiver_abort(SCHCSession *session) {
    session->state.timestamp = -1;
    session->state.aborted = 1;
    CompoundACK ra;
    generate_receiver_abort(&session->rule, &ra);
    memcpy(&session->ack, &ra, sizeof(CompoundACK));
    memcpy(&session->state.receiver_abort, &ra, sizeof(CompoundACK));
    return 0;
}

int session_was_aborted(SCHCSession *session) {
    return session->state.aborted == 1;
}

void session_update_timestamp(SCHCSession *session, time_t timestamp) {
    session->state.timestamp = (int) timestamp;
}

int session_expired_inactivity_timeout(SCHCSession *session, time_t timestamp) {
    if (DISABLE_INACTIVITY_TIMEOUT || session->state.timestamp < 0) return 0;

    return (float) abs((int) timestamp - session->state.timestamp) >
           INACTIVITY_TIMEOUT;
}

int session_check_requested_fragment(SCHCSession *session, Fragment *frg) {
    int idx = get_frg_nb(&session->rule, frg);
    return session->state.requested_frg[idx] == '1';
}

int session_already_received(SCHCSession *session, Fragment *frg) {
    int idx = get_frg_nb(&session->rule, frg);
    if (session->state.bitmap[idx] == '1') return 1;

    if (is_frg_all_1(&session->rule, frg)) {
        return session->state.last_ack.byte_size != 0
               && is_ack_complete(&session->rule, &session->state.last_ack)
               && frg_equal(&session->state.last_fragment, frg);
    }

    return 0;
}

int session_expects_fragment(SCHCSession *session, Fragment *frg) {
    if (!is_frg_all_1(&session->rule, frg) &&
        session_already_received(session, frg))
        return 0;

    if (is_frg_null(&session->state.last_fragment)
        || is_frg_sender_abort(&session->rule, &session->state.last_fragment))
        return 1;

    if (is_frg_all_1(&session->rule, &session->state.last_fragment))
        return is_ack_complete(&session->rule, &session->state.last_ack);

    if (session_check_requested_fragment(session, frg)) return 1;

    int frg_window = get_frg_window(&session->rule, frg);
    int last_frg_window = get_frg_window(&session->rule,
                                         &session->state.last_fragment);

    if (frg_window > last_frg_window) return 1;
    if (frg_window == last_frg_window) {
        int frg_nb = get_frg_nb(&session->rule, frg);
        int last_frg_nb = get_frg_nb(&session->rule,
                                     &session->state.last_fragment);

        if (frg_nb > last_frg_nb) return 1;
        if (frg_nb == last_frg_nb) return is_frg_all_1(&session->rule, frg);
    }

    return 0;
}

int start_new_session(SCHCSession *session, int retain_last_data) {
    Fragment last_frg;
    CompoundACK last_ack;

    if (retain_last_data) {
        memcpy(&last_frg, &session->state.last_fragment, sizeof(Fragment));
        memcpy(&last_ack, &session->state.last_ack, sizeof(CompoundACK));
    }

    free(session->fragments);
    state_destroy(&session->state);
    session_construct(session, session->rule);

    if (retain_last_data) {
        memcpy(&session->state.last_fragment, &last_frg, sizeof(Fragment));
        memcpy(&session->state.last_ack, &last_ack, sizeof(CompoundACK));
    }

    return 0;
}

int session_has_pending_ack(SCHCSession *session, Fragment *frg) {
    if (is_ack_null(&session->state.last_ack)) return 0;

    if (!is_ack_complete(&session->rule, &session->state.last_ack)) {
        printf("Last ACK was not complete");
        return 0;
    }

    if (!is_frg_all_1(&session->rule, frg)) {
        generate_null_frg(&session->state.last_fragment);
        generate_null_ack(&session->state.last_ack);
        return 0;
    }

    return is_frg_all_1(&session->rule, &session->state.last_fragment);
}

int session_store_frg(SCHCSession *session, Fragment *frg) {
    if (session_already_received(session, frg)) return 0;

    int frg_nb = get_frg_nb(&session->rule, frg);
    memcpy(&session->fragments[frg_nb], frg, sizeof(Fragment));
    return 1;
}

void session_get_bitmap(SCHCSession *session, int frg_window, char dest[]) {
    int bm_start_idx = frg_window * session->rule.window_size;
    memcpy(dest, session->state.bitmap + bm_start_idx,
           session->rule.window_size);
    dest[session->rule.window_size] = '\0';
}

void session_update_bitmap(SCHCSession *session, Fragment *frg) {
    int frg_nb = get_frg_nb(&session->rule, frg);
    replace_char(session->state.bitmap, frg_nb, '1');
}

int session_update_requested(SCHCSession *session, CompoundACK *ack) {
    if (is_ack_complete(&session->rule, ack)) return -1;

    int nb_tuples = get_ack_nb_tuples(&session->rule, ack);
    char windows[nb_tuples][session->rule.m + 1];
    char bitmaps[nb_tuples][session->rule.window_size + 1];
    get_ack_tuples(&session->rule, ack, nb_tuples, windows, bitmaps);

    for (int i = 0; i < nb_tuples; i++) {
        int window_nb = bin_to_int(windows[i]);
        int bm_idx = window_nb * session->rule.window_size;

        for (int j = 0; j < session->rule.window_size; j++) {
            if (bitmaps[i][j] == '0') {
                replace_char(session->state.requested_frg,
                             bm_idx + j,
                             '1');
            }
        }
    }

    return 0;
}

int session_check_bitmaps(
        SCHCSession *session,
        Fragment *frg,
        char bitmaps[session->rule.max_window_nb][session->rule.window_size + 1]
) {
    if (!frg_expects_ack(&session->rule, frg)) return -1;

    int curr_window = get_frg_window(&session->rule, frg);
    int nb_tuples = 0;

    for (int i = 0; i < session->rule.max_window_nb; i++) {
        memset(bitmaps[i], '\0', session->rule.window_size + 1);
    }

    for (int i = 0; i < curr_window + 1; i++) {
        char bitmap[session->rule.window_size + 1];
        session_get_bitmap(session, i, bitmap);
        int lost = 0;

        if (is_frg_all_1(&session->rule, frg) && i == curr_window) {
            char rcs[session->rule.u + 1];
            get_frg_rcs(&session->rule, frg, rcs);
            int frgs_in_last_window = bin_to_int(rcs);
            char expected_bitmap[session->rule.window_size + 1];

            memset(expected_bitmap, '\0', session->rule.window_size + 1);
            memset(expected_bitmap, '1', frgs_in_last_window - 1);
            memset(expected_bitmap + frgs_in_last_window - 1, '0',
                   session->rule.window_size - frgs_in_last_window + 1);
            expected_bitmap[session->rule.window_size - 1] = '1';

            if (strcmp(bitmap, expected_bitmap) != 0) lost = 1;
        } else if (!is_monochar(bitmap, '1')) lost = 1;

        if (lost) {
            nb_tuples++;
            memcpy(bitmaps[i], bitmap, session->rule.window_size + 1);
        }
    }

    return nb_tuples > 0;
}

int session_generate_ack(SCHCSession *session, Fragment *frg) {
    char bitmaps[session->rule.max_window_nb][session->rule.window_size + 1];

    int lost = session_check_bitmaps(session, frg, bitmaps);
    int wdw = get_frg_window(&session->rule, frg);
    char c = lost ? '0' : '1';

    return generate_ack(&session->rule, &session->ack, wdw, c, bitmaps);
}

/*
int session_schc_recv(SCHCSession *session, Fragment *frg, time_t timestamp) {

    if (session_was_aborted(session)) return SCHC_RECEIVER_ABORTED;

    if (session_expired_inactivity_timeout(session, timestamp)) {
        session_receiver_abort(session);
        return SCHC_RECEIVER_ABORTED;
    }

    session_update_timestamp(session, timestamp);

    if (is_frg_sender_abort(&session->rule, frg)) {
        session_sender_abort(session);
        return SCHC_SENDER_ABORTED;
    }

    if (!session_expects_fragment(session, frg))
        start_new_session(session, 0);

    session_store_frg(session, frg);
    session_update_bitmap(session, frg);

    if (session_has_pending_ack(session, frg)) {
        memcpy(&session->ack, &session->state.last_ack, sizeof(CompoundACK));
        return 1;
    }

    if (!frg_expects_ack(&session->rule, frg)
        || session_check_requested_fragment(session, frg))
        return 0;

    session_generate_ack(session, frg);
    return 1;
}
*/