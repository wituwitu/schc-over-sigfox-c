#include "ack.h"

#ifndef SCHC_OVER_SIGFOX_C_SCHC_SESSION_H
#define SCHC_OVER_SIGFOX_C_SCHC_SESSION_H

#endif //SCHC_OVER_SIGFOX_C_SCHC_SESSION_H

typedef struct {
    Rule rule;
    Fragment *fragments;
    CompoundACK ack;
    struct {
        char *bitmap;
        char *requested_fragments;
        Fragment last_fragment;
        CompoundACK last_ack;
        CompoundACK receiver_abort;
        int aborted;
        int timestamp;
    } state;
} SCHCSession;

int session_construct(SCHCSession *s, Rule rule);

void session_destroy(SCHCSession *s);

int session_was_aborted(SCHCSession *s);

void session_update_timestamp(SCHCSession *s, time_t timestamp);

int session_expired_inactivity_timeout(SCHCSession *s, time_t timestamp);

int session_check_requested_fragment(SCHCSession *s, Fragment *frg);

int session_already_received(SCHCSession *s, Fragment *frg);

int session_expects_fragment(SCHCSession *s, Fragment *frg);

void start_new_session(SCHCSession *s, int retain_last_data);

int session_check_pending_ack(SCHCSession *s, Fragment *frg);

void session_store_frg(SCHCSession *s, Fragment *frg);

void session_get_bitmap(SCHCSession *s, Fragment *frg, char dest[]);

void session_update_bitmap(SCHCSession *s, Fragment *frg);

int session_update_requested(SCHCSession *s, CompoundACK *ack);

int session_get_tuples(
        SCHCSession *s,
        Fragment *frg,
        char windows[s->rule.max_window_number][s->rule.m + 1],
        char bitmaps[s->rule.max_window_number][s->rule.window_size + 1]
);

void session_generate_ack(SCHCSession *s, Fragment *frg);

int schc_recv(SCHCSession *s, Fragment *frg, time_t timestamp);