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

/*
 * Function:  state_construct
 * --------------------
 * Initializes the parameters of the State structure.
 *
 *  state: Uninitialized State structure.
 *  rule: Rule parameters to initialize the State with.
 */
int state_construct(State *state, Rule rule);

/*
 * Function:  state_destroy
 * --------------------
 * Frees the memory allocated for the State structure.
 *
 *  state: State structure to be destroyed.
 */
int state_destroy(State *state);

/*
 * Function:  session_construct
 * --------------------
 * Initializes the parameters of the SCHCSession structure. It also initializes
 * and allocates memory for the internal State structure.
 *
 *  session: Uninitialized SCHCSession structure.
 *  rule: Rule parameters to initialize the SCHCSession with.
 */
int session_construct(SCHCSession *session, Rule rule);

/*
 * Function:  session_destroy
 * --------------------
 * Frees the memory allocated for the SCHCSession structure. It also frees the
 * memory allocated for the internal State structure.
 *
 *  session: SCHCSession structure to be destroyed.
 */
int session_destroy(SCHCSession *session);

/*
 * Function:  session_sender_abort
 * --------------------
 * Sets internal SCHCSession variables that signal a session aborted by the
 * sender.
 *
 *  session: SCHCSession structure that controls the communication.
 */
int session_sender_abort(SCHCSession *session);

/*
 * Function:  session_receiver_abort
 * --------------------
 * Sets internal SCHCSession variables that signal a session aborted by the
 * receiver, and generates a SCHC Receiver Abort message into the ack and
 * state.receiver_abort parameters.
 *
 *  session: SCHCSession structure that controls the communication.
 */
int session_receiver_abort(SCHCSession *session);

/*
 * Function:  session_was_aborted
 * --------------------
 * Checks if the SCHC transmission was aborted.
 *
 *  session: SCHCSession structure that controls the communication.
 */
int session_was_aborted(SCHCSession *session);

/*
 * Function:  session_update_timestamp
 * --------------------
 * Updates the timestamp of the transmission, which is generated after receiving
 * a SCHC Fragment.
 *
 *  session: SCHCSession structure that controls the communication.
 *  timestamp: The timestamp to configure into the session.
 */
void session_update_timestamp(SCHCSession *session, time_t timestamp);

/*
 * Function:  session_expired_inactivity_timeout
 * --------------------
 * Checks if the SCHC Inactivity Timer has expired by comparing the last
 * updated timestamp and a new one. Defaults to 0 if the Inactivity Timer check
 * is disabled or if no timestamp has been configured yet.
 *
 *  session: SCHCSession structure that controls the communication.
 *  timestamp: The new timestamp to compare with the previously configured.
 */
int session_expired_inactivity_timeout(SCHCSession *session, time_t timestamp);

/*
 * Function:  session_check_requested_fragment
 * --------------------
 * Checks if a SCHC Fragment has been requested by a downlink Compound ACK.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg: Fragment to be checked.
 */
int session_check_requested_fragment(SCHCSession *session, Fragment *frg);

/*
 * Function:  session_already_received
 * --------------------
 * Checks if a SCHC Fragment has already been processed by the SCHC Session.
 * This is performed by checking the bitmap of the transmission. However, it
 * may be the case that the fragment to be checked is the retransmission of an
 * All-1 whose corresponding complete Compound ACK was not received by the
 * sender. In this case, the last_ack parameter is checked and the fragment
 * is compared to the last_fragment parameter.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg: Fragment to be checked.
 */
int session_already_received(SCHCSession *session, Fragment *frg);

/*
 * Function:  session_expects_fragment
 * --------------------
 * Checks if a SCHC Fragment can be processed by the current SCHC session. This
 * is done by performing a series of checks regarding parameters of the session
 * and the fragment.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg: Fragment to be checked.
 */
int session_expects_fragment(SCHCSession *session, Fragment *frg);

/*
 * Function:  start_new_session
 * --------------------
 * Deletes the data of a SCHC session to process a new one.
 *
 *  session: SCHCSession structure that controls the communication.
 *  retain_last_data: Whether to keep the last received fragment and last ack
 *  sent. This allows the algorithm to detect All-1 retransmissions of the
 *  previous session.
 */
int start_new_session(SCHCSession *session, int retain_last_data);

/*
 * Function:  session_has_pending_ack
 * --------------------
 * Checks if the session has a complete ACK that was not correctly delivered
 * to the sender after receiving an All-1. This check is triggered by receiving
 * a fragment of a new SCHC session. If the new fragment is not an All-1, it
 * is assumed that the last ACK was received correctly, and both the
 * last_fragment and last_ack parameters are reset.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg: Fragment that triggered the check.
 */
int session_has_pending_ack(SCHCSession *session, Fragment *frg);

/*
 * Function:  session_store_frg
 * --------------------
 * Stores a SCHC Fragment in the internal fragment array. Returns 1 on success,
 * and 0 if the fragment was already received, in which case the fragment is
 * ignored.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg: Fragment to be stored.
 */
int session_store_frg(SCHCSession *session, Fragment *frg);

/*
 * Function:  session_get_bitmap
 * --------------------
 * Obtains the bitmap pertaining to a particular window.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg_window: Window number.
 *  dest: Where to store the bitmap.
 */
void session_get_bitmap(SCHCSession *session, int frg_window, char dest[]);

/*
 * Function:  session_update_bitmap
 * --------------------
 * Updates the internal bitmap of received SCHC Fragments.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg: SCHC Fragment to be registered in the bitmap.
 */
void session_update_bitmap(SCHCSession *session, Fragment *frg);

/*
 * Function:  session_update_requested
 * --------------------
 * Updates the internal bitmap of SCHC Fragments requested by a downlink ACK.
 * Returns -1 if the ACK is complete.
 *
 *  session: SCHCSession structure that controls the communication.
 *  ack: Loss-reporting CompoundACK.
 */
int session_update_requested(SCHCSession *session, CompoundACK *ack);

/*
 * Function:  session_check_bitmaps
 * --------------------
 * When receiving an All-0 or an All-1, checks if the internal bitmap of
 * received fragments signals lost fragments and obtains an array of all bitmaps
 * according to their window number.
 * Returns -1 if the fragment does not expect an ACK.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg: SCHC Fragment that triggered the check.
 *  bitmaps: Array of char[]s where to store all bitmaps.
 */
int session_check_bitmaps(
        SCHCSession *session,
        Fragment *frg,
        char bitmaps[session->rule.max_window_nb][session->rule.window_size + 1]
);

/*
 * Function:  session_check_bitmaps
 * --------------------
 * Generates a Compound ACK into the internal ack parameter by separating the
 * bitmaps of the session and checking for fragments reported lost.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg: SCHC Fragment that triggered the ACK generation.
 */
int session_generate_ack(SCHCSession *session, Fragment *frg);

/*
 * Function:  schc_recv
 * --------------------
 * Executes the SCHC Receiver algorithm to process an incoming SCHC Fragment.
 *
 *  session: SCHCSession structure that controls the communication.
 *  frg: SCHC Fragment to be processed by the algorithm.
 *  timestamp: Time at which the fragment was received.
 */
int schc_recv(SCHCSession *session, Fragment *frg, time_t timestamp);