#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "schc_session.h"

int session_construct(SCHCSession *s, Rule rule) {
    s->rule = rule;
    s->fragments = malloc(sizeof(Fragment) * rule.max_fragment_number + 1);
    s->ack.byte_size = 0;

    s->bitmap = malloc(rule.window_size * rule.max_window_number);
    memset(s->bitmap, 0, rule.max_fragment_number);
    s->requested_fragments = malloc(rule.window_size * rule.max_window_number);
    memset(s->bitmap, 0, rule.max_fragment_number);

    generate_null_frg(&s->last_fragment);
    s->last_ack.byte_size = 0;
    s->receiver_abort.byte_size = 0;
    s->aborted = 0;

    s->timestamp = -1;

    return 0;
}

void session_destroy(SCHCSession *s) {
    free(s->fragments);
    free(s->bitmap);
    free(s->requested_fragments);
}

int session_was_aborted(SCHCSession *s) {
    return s->aborted == 1 || s->receiver_abort.byte_size > 0;
}

void session_update_timestamp(SCHCSession *s, time_t timestamp) {
    s->timestamp = (int) timestamp;
}

int session_expired_inactivity_timeout(SCHCSession *s, time_t timestamp) {
    if (DISABLE_INACTIVITY_TIMEOUT || s->timestamp == -1) return -1;

    return abs((int) timestamp - s->timestamp) > INACTIVITY_TIMEOUT;
}

int session_requested_fragment(SCHCSession *s, Fragment *frg) {
    int idx = get_frg_idx(&s->rule, frg);

    return s->requested_fragments[idx] == 1;
}

int session_already_received(SCHCSession *s, Fragment *frg) {
    int idx = get_frg_idx(&s->rule, frg);
    if (s->bitmap[idx] == 1) return 1;

    if (is_frg_all_1(&s->rule, frg)) {
        return s->last_ack.byte_size != 0
               && is_ack_complete(&s->rule, &s->last_ack)
               && frg_equal(&s->last_fragment, frg);
    }

    return 0;
}