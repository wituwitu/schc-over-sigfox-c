#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "schc_session.h"

int session_construct(SCHCSession *s, Rule rule) {
    s->rule = rule;
    s->fragments = malloc(sizeof(Fragment) * rule.max_fragment_number + 1);
    s->ack.byte_size = 0;

    s->bitmaps = malloc(rule.window_size * rule.max_window_number);
    s->requested_fragments = malloc(rule.window_size * rule.max_window_number);

    generate_null_frg(&s->last_fragment);
    s->pending_ack.byte_size = 0;
    s->receiver_abort.byte_size = 0;
    s->aborted = 0;

    s->timestamp = -1;

    return 0;
}

void session_destroy(SCHCSession *s) {
    free(s->fragments);
    free(s->bitmaps);
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

