#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include "schc_session.h"

int test_state_construct() {
    Rule rule;
    init_rule(&rule, "000");
    State state;

    assert(state_construct(&state, rule) == 0);
    assert(is_frg_null(&state.last_fragment));
    assert(is_ack_null(&state.last_ack));
    assert(is_ack_null(&state.receiver_abort));
    assert(state.aborted == 0);
    assert(state.timestamp == -1);
    assert(strncmp(
            state.bitmap,
            "0000000000000000000000000000",
            rule.max_fragment_number
    ) == 0);
    assert(strncmp(
            state.requested_frg,
            "0000000000000000000000000000",
            rule.max_fragment_number
    ) == 0);

    free(state.bitmap);
    free(state.requested_frg);
    return 0;
}

int test_state_destroy() {
    Rule rule;
    init_rule(&rule, "000");
    State state;
    state_construct(&state, rule);

    assert(state_destroy(&state) == 0);

    return 0;
}

int test_session_construct() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");

    assert(session_construct(&s, rule) == 0);
    assert(s.rule.id == rule.id);
    assert(is_ack_null(&s.ack));
    assert(is_frg_null(&s.state.last_fragment));
    assert(is_ack_null(&s.state.last_ack));
    assert(is_ack_null(&s.state.receiver_abort));
    assert(s.state.aborted == 0);
    assert(s.state.timestamp == -1);
    assert(strncmp(
            s.state.bitmap,
            "0000000000000000000000000000",
            rule.max_fragment_number
    ) == 0);
    assert(strncmp(
            s.state.requested_frg,
            "0000000000000000000000000000",
            rule.max_fragment_number
    ) == 0);
    for (int i = 0; i <= rule.max_fragment_number; i++) {
        assert(is_frg_null(&s.fragments[i]));
    }

    state_destroy(&s.state);
    free(s.fragments);
    return 0;
}

int test_session_destroy() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    assert(session_destroy(&s) == 0);

    return 0;
}

int test_session_sender_abort() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);
    assert(session_sender_abort(&s) == 0);
    assert(s.state.timestamp == -1);
    assert(s.state.aborted == 1);
    session_destroy(&s);
    return 0;
}

int test_session_receiver_abort() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);
    assert(session_receiver_abort(&s) == 0);
    assert(is_ack_receiver_abort(&rule, &s.ack));
    assert(is_ack_receiver_abort(&rule, &s.state.receiver_abort));
    assert(s.state.timestamp == -1);
    assert(s.state.aborted == 1);
    session_destroy(&s);
    return 0;
}

int test_session_was_aborted() {
    Rule rule;
    init_rule(&rule, "000");

    SCHCSession s1;
    session_construct(&s1, rule);
    session_sender_abort(&s1);
    assert(session_was_aborted(&s1));

    SCHCSession s2;
    session_construct(&s2, rule);
    session_receiver_abort(&s2);
    assert(session_was_aborted(&s2));

    session_destroy(&s1);
    session_destroy(&s2);
    return 0;
}

int test_session_update_timestamp() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    time_t now = time(0);
    session_update_timestamp(&s, now);
    assert(s.state.timestamp > 0);

    session_destroy(&s);
    return 0;
}

int test_session_expired_inactivity_timeout() {
    return -1;
}

int test_session_check_requested_fragment() {
    return -1;
}

int test_session_already_received() {
    return -1;
}

int test_session_expects_fragment() {
    return -1;
}

int test_start_new_session() {
    return -1;
}

int test_session_check_pending_ack() {
    return -1;
}

int test_session_store_frg() {
    return -1;
}

int test_session_get_bitmap() {
    return -1;
}

int test_session_update_bitmap() {
    return -1;
}

int test_session_update_requested() {
    return -1;
}

int test_session_check_bitmaps() {
    return -1;
}

int test_session_generate_ack() {
    return -1;
}

int test_session_schc_recv() {
    return -1;
}


int main() {
    printf("%d test_state_construct\n", test_state_construct());
    printf("%d test_state_destroy\n", test_state_destroy());
    printf("%d test_session_construct\n", test_session_construct());
    printf("%d test_session_destroy\n", test_session_destroy());
    printf("%d test_session_sender_abort\n", test_session_sender_abort());
    printf("%d test_session_receiver_abort\n", test_session_receiver_abort());

    printf("%d test_session_was_aborted\n", test_session_was_aborted());
    printf("%d test_session_update_timestamp\n",
           test_session_update_timestamp());
    printf("%d test_session_expired_inactivity_timeout\n",
           test_session_expired_inactivity_timeout());
    printf("%d test_session_check_requested_fragment\n",
           test_session_check_requested_fragment());
    printf("%d test_session_already_received\n",
           test_session_already_received());
    printf("%d test_session_expects_fragment\n",
           test_session_expects_fragment());
    printf("%d test_start_new_session\n", test_start_new_session());
    printf("%d test_session_store_frg\n", test_session_store_frg());
    printf("%d test_session_get_bitmap\n", test_session_get_bitmap());
    printf("%d test_session_update_bitmap\n", test_session_update_bitmap());
    printf("%d test_session_update_requested\n",
           test_session_update_requested());
    printf("%d test_session_check_bitmaps\n", test_session_check_bitmaps());
    printf("%d test_session_generate_ack\n", test_session_generate_ack());
    printf("%d test_session_schc_recv\n", test_session_schc_recv());

    return 0;
}