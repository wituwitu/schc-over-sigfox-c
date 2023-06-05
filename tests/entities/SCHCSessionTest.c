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
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    time_t now = time(0);
    assert(!session_expired_inactivity_timeout(&s, now));

    session_update_timestamp(&s, now);
    time_t not_exp = now + 50;
    time_t exp = now + 501;
    assert(!session_expired_inactivity_timeout(&s, not_exp));
    assert(session_expired_inactivity_timeout(&s, exp));

    session_destroy(&s);
    return 0;
}

int test_session_check_requested_fragment() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    strncpy(
            s.state.requested_frg,
            "0000000000000001111100001000",
            rule.max_fragment_number
    );

    Fragment frg1 = {"\x15\x88\x88\x88", 4};
    assert(get_frg_nb(&rule, &frg1) == 15);
    assert(session_check_requested_fragment(&s, &frg1));


    Fragment frg2 = {"\x00\x00\x00\x00", 4};
    assert(get_frg_nb(&rule, &frg2) == 6);
    assert(!session_check_requested_fragment(&s, &frg2));

    session_destroy(&s);
    return 0;
}

int test_session_already_received() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    strncpy(
            s.state.bitmap,
            "1111111111111110000011110111",
            rule.max_fragment_number
    );

    Fragment frg1 = {"\x15\x88\x88\x88", 4};
    assert(get_frg_nb(&rule, &frg1) == 15);
    assert(!session_already_received(&s, &frg1));

    Fragment frg2 = {"\x00\x00\x00\x00", 4};
    assert(get_frg_nb(&rule, &frg2) == 6);
    assert(session_already_received(&s, &frg2));

    strncpy(
            s.state.bitmap,
            "0000000000000000000000000000",
            rule.max_fragment_number
    );

    // Previous session ended but ACK was lost
    // => new session started + repeated all-1 of previous session
    Fragment all_1 = {"\027\200DD", 4};
    assert(is_frg_all_1(&rule, &all_1));

    // last ack is empty
    assert(!session_already_received(&s, &all_1));

    // last ack is not complete
    CompoundACK incomplete_ack = {"\x15\x88\x88\x88\x88\x88\x88\x88",
                                  8};
    assert(!is_ack_complete(&rule, &incomplete_ack));
    memcpy(&s.state.last_ack, &incomplete_ack, sizeof(CompoundACK));
    assert(!session_already_received(&s, &all_1));

    // last fragment is not equal
    CompoundACK complete_ack = {"\x1C\x00\x00\x00\x00\x00\x00\x00",
                                8};
    assert(is_ack_complete(&rule, &complete_ack));
    memcpy(&s.state.last_ack, &complete_ack, sizeof(CompoundACK));

    Fragment another_all_1 = {"\027\200EE", 4};
    assert(is_frg_all_1(&rule, &another_all_1));
    memcpy(&s.state.last_fragment, &another_all_1, sizeof(Fragment));
    assert(!session_already_received(&s, &all_1));

    // last ack byte size != 0 && last ack is complete && last fragment is equal
    memcpy(&s.state.last_fragment, &all_1, sizeof(Fragment));
    assert(session_already_received(&s, &all_1));

    session_destroy(&s);
    return 0;
}

int test_session_expects_fragment() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    // not all1, already received -> 0
    strncpy(
            s.state.bitmap,
            "1111111111111110000011110111",
            rule.max_fragment_number
    );
    Fragment frg = {"\x00\x00\x00\x00\x00\x00\x00\x00", 8};
    assert(session_already_received(&s, &frg));
    assert(!session_expects_fragment(&s, &frg));

    // last frg is null -> 1
    strncpy(
            s.state.bitmap,
            "0000000000000000000000000000",
            rule.max_fragment_number
    );
    Fragment null;
    generate_null_frg(&null);
    memcpy(&s.state.last_fragment, &null, sizeof(Fragment));
    assert(session_expects_fragment(&s, &frg));

    // frg is sender-abort -> 1
    Fragment sa;
    generate_sender_abort(&rule, &frg, &sa);
    memcpy(&s.state.last_fragment, &sa, sizeof(Fragment));
    assert(session_expects_fragment(&s, &frg));

    // last frg is all 1, ack is not complete -> 0
    Fragment all_1 = {"\027\200DD", 4};
    memcpy(&s.state.last_fragment, &all_1, sizeof(Fragment));
    CompoundACK incomplete_ack = {"\x15\x88\x88\x88\x88\x88\x88\x88",
                                  8};
    memcpy(&s.state.last_ack, &incomplete_ack, sizeof(CompoundACK));
    assert(!session_expects_fragment(&s, &frg));

    // last frg is all 1, ack is complete -> 1
    CompoundACK complete_ack = {"\x1C\x00\x00\x00\x00\x00\x00\x00",
                                8};
    memcpy(&s.state.last_ack, &complete_ack, sizeof(CompoundACK));
    assert(session_expects_fragment(&s, &frg));

    // last frg not all-1, not already received, fragment is requested -> 1
    Fragment frg0 = {"\x01\x01\x88\x88\x88\x88\x88\x88", 8};
    memcpy(&s.state.last_fragment, &frg0, sizeof(Fragment));
    strncpy(
            s.state.bitmap,
            "1111000000000000000011110111",
            rule.max_fragment_number
    );
    strncpy(
            s.state.requested_frg,
            "0000111111111111111100001000",
            rule.max_fragment_number
    );
    assert(session_expects_fragment(&s, &frg));

    // (not requested) (not already received) smaller window -> 0
    strncpy(
            s.state.bitmap,
            "0000000000000000000000000000",
            rule.max_fragment_number
    );
    strncpy(
            s.state.requested_frg,
            "0000000000000000000000001000",
            rule.max_fragment_number
    );
    Fragment frg2 = {"\x15\x88\x88\x88\x88\x88\x88\x88", 8};
    memcpy(&s.state.last_fragment, &frg2, sizeof(Fragment));
    assert(!session_expects_fragment(&s, &frg));

    //  greater window -> 1
    Fragment frg3 = {"\x1D\x88\x88\x88\x88\x88\x88\x88", 8};
    assert(session_expects_fragment(&s, &frg3));

    // same window && smaller nb -> 0
    Fragment frg4 = {"\x16\x88\x88\x88\x88\x88\x88\x88", 8};
    assert(!session_expects_fragment(&s, &frg4));

    // same window && greater nb -> 1
    Fragment frg5 = {"\x11\x88\x88\x88\x88\x88\x88\x88", 8};
    assert(session_expects_fragment(&s, &frg5));

    // same window && same nb && is not all-1 -> 0
    Fragment frg6 = {"\x15\x88\x88\x88\x88\x88\x88\x88", 8};
    assert(!session_expects_fragment(&s, &frg6));

    // same window && same nb && is all-1 -> 1
    memcpy(&s.state.last_fragment, &all_1, sizeof(Fragment));
    Fragment frg7 = {"\027\200DD", 4};
    assert(session_expects_fragment(&s, &frg7));

    session_destroy(&s);
    return 0;
}

int test_start_new_session() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    // fill session
    CompoundACK ack = {"\x15\x88\x88\x88\x88\x88\x88\x88", 8};
    memcpy(&s.ack, &ack, sizeof(CompoundACK));
    Fragment frg = {"\x00\x00\x00\x00\x00\x00\x00\x00", 8};
    memcpy(&s.fragments[0], &frg, sizeof(Fragment));
    // fill session->state
    memcpy(&s.state.last_fragment, &frg, sizeof(Fragment));
    memcpy(&s.state.last_ack, &ack, sizeof(Fragment));
    session_update_timestamp(&s, time(0));
    strncpy(
            s.state.bitmap,
            "1111000000000000000011110111",
            rule.max_fragment_number
    );
    strncpy(
            s.state.requested_frg,
            "1000111111111111111100001000",
            rule.max_fragment_number
    );

    // check that session has data
    assert(!is_ack_null(&s.ack));
    assert(!is_frg_null(&s.fragments[0]));
    assert(!is_frg_null(&s.state.last_fragment));
    assert(!is_ack_null(&s.state.last_ack));
    assert(is_ack_null(&s.state.receiver_abort));
    assert(&s.state.timestamp > 0);
    assert(strncmp(&s.state.bitmap[0], "1", 1) == 0);
    assert(strncmp(&s.state.requested_frg[0], "1", 1) == 0);

    // start new, retaining
    assert(start_new_session(&s, 1) == 0);
    assert(is_ack_null(&s.ack));
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

    assert(frg_equal(&s.state.last_fragment, &frg));
    assert(ack_equal(&s.state.last_ack, &ack));

    // start new, not retaining
    assert(start_new_session(&s, 0) == 0);
    assert(is_frg_null(&s.state.last_fragment));
    assert(is_ack_null(&s.state.last_ack));

    session_destroy(&s);
    return 0;
}

int test_session_check_pending_ack() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    Fragment frg = {"\x15\x88\x88\x88\x88\x88\x88\x88", 8};
    Fragment all_1 = {"\027\200DD", 4};
    CompoundACK incomplete_ack = {"\x15\x88\x88\x88\x88\x88\x88\x88", 8};
    CompoundACK complete_ack = {"\x1C\x00\x00\x00\x00\x00\x00\x00", 8};
    // ack is null
    assert(!session_has_pending_ack(&s, &frg));

    // ack is not complete
    memcpy(&s.state.last_ack, &incomplete_ack, sizeof(CompoundACK));
    assert(!session_has_pending_ack(&s, &frg));

    // ack is complete, frg is not all1
    memcpy(&s.state.last_fragment, &frg, sizeof(Fragment));
    memcpy(&s.state.last_ack, &complete_ack, sizeof(CompoundACK));
    assert(!session_has_pending_ack(&s, &frg));
    assert(is_frg_null(&s.state.last_fragment));
    assert(is_ack_null(&s.state.last_ack));

    // ack is complete, frg is all1
    memcpy(&s.state.last_ack, &complete_ack, sizeof(CompoundACK));
    assert(!session_has_pending_ack(&s, &all_1));

    session_destroy(&s);
    return 0;
}

int test_session_store_frg() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    strncpy(
            s.state.bitmap,
            "1111111111111110000011110111",
            rule.max_fragment_number
    );

    Fragment frg_ar = {"\x00\x00\x00\x00", 4};
    assert(session_store_frg(&s, &frg_ar) == 0);

    Fragment frg = {"\x15\x88\x88\x88", 4};
    assert(session_store_frg(&s, &frg) == 1);
    int frg_nb = get_frg_nb(&rule, &frg);
    assert(frg_equal(&frg, &s.fragments[frg_nb]));

    session_destroy(&s);
    return 0;
}

int test_session_get_bitmap() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);


    session_destroy(&s);
    return -1;
}

int test_session_update_bitmap() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);


    session_destroy(&s);
    return -1;
}

int test_session_update_requested() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);


    session_destroy(&s);
    return -1;
}

int test_session_check_bitmaps() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);


    session_destroy(&s);
    return -1;
}

int test_session_generate_ack() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);


    session_destroy(&s);
    return -1;
}

int test_session_schc_recv() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);


    session_destroy(&s);
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