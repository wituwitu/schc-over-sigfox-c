#include "ack.h"

#ifndef SCHC_OVER_SIGFOX_C_SCHC_SESSION_H
#define SCHC_OVER_SIGFOX_C_SCHC_SESSION_H

#endif //SCHC_OVER_SIGFOX_C_SCHC_SESSION_H

typedef struct {
    Rule rule;
    Fragment *fragments;
    CompoundACK ack;
    char *bitmap;
    char *requested_fragments;
    Fragment last_fragment;
    CompoundACK last_ack;
    CompoundACK receiver_abort;
    int aborted;
    int timestamp;
} SCHCSession;

int session_construct(SCHCSession *s, Rule rule);

void session_destroy(SCHCSession *s);

int session_was_aborted(SCHCSession *s);

void session_update_timestamp(SCHCSession *s, time_t timestamp);

int session_expired_inactivity_timeout(SCHCSession *s, time_t timestamp);

int session_requested_fragment(SCHCSession *s, Fragment *frg);

int session_already_received(SCHCSession *s, Fragment *frg);

int session_expects_fragment(SCHCSession *s, Fragment *frg);

int session_get_pending_ack(SCHCSession *s, Fragment *frg);

int session_update_bitmap(SCHCSession *s, Fragment *frg);

int session_update_requested(SCHCSession *s);

int session_generate_ack(SCHCSession *s);

int schc_recv(SCHCSession *s);