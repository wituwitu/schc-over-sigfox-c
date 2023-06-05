#include "ack.h"
#include <time.h>

#ifndef SCHC_OVER_SIGFOX_C_SCHC_SESSION_H
#define SCHC_OVER_SIGFOX_C_SCHC_SESSION_H

#endif //SCHC_OVER_SIGFOX_C_SCHC_SESSION_H

typedef struct {
    Fragment last_fragment;
    CompoundACK last_ack;
    CompoundACK receiver_abort;
    int aborted;
    int timestamp;
    char *bitmap;
    char *requested_frg;
} State;

typedef struct {
    Rule rule;
    CompoundACK ack;
    Fragment *fragments;
    State state;
} SCHCSession;

int state_construct(State *state, Rule rule);

int state_destroy(State *state);

int session_construct(SCHCSession *s, Rule rule);

int session_destroy(SCHCSession *s);

int session_sender_abort(SCHCSession *s);

int session_receiver_abort(SCHCSession *s);

int session_was_aborted(SCHCSession *s);

void session_update_timestamp(SCHCSession *s, time_t timestamp);

int session_expired_inactivity_timeout(SCHCSession *s, time_t timestamp);

int session_check_requested_fragment(SCHCSession *s, Fragment *frg);

int session_already_received(SCHCSession *s, Fragment *frg);

int session_expects_fragment(SCHCSession *s, Fragment *frg);

int start_new_session(SCHCSession *s, int retain_last_data);

int session_has_pending_ack(SCHCSession *s, Fragment *frg);

int session_store_frg(SCHCSession *s, Fragment *frg);

void session_get_bitmap(SCHCSession *s, int frg_window, char dest[]);

void session_update_bitmap(SCHCSession *s, Fragment *frg);

int session_update_requested(SCHCSession *s, CompoundACK *ack);

int session_check_bitmaps(
        SCHCSession *s,
        Fragment *frg,
        char bitmaps[s->rule.max_window_nb][s->rule.window_size + 1]
);

int session_generate_ack(SCHCSession *s, Fragment *frg);

int schc_recv(SCHCSession *s, Fragment *frg, time_t timestamp);