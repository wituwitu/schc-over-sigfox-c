#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include "schc_session.h"

int test_session_construct() {
    SCHCSession s;
    Rule rule;
    init_rule(&rule, "000");
    session_construct(&s, rule);

    assert(s.rule.id == rule.id);
    assert(is_ack_null(&s.ack));
    assert(is_frg_null(&s.state->last_fragment));
    assert(is_ack_null(&s.state->last_ack));
    assert(is_ack_null(&s.state->receiver_abort));
    assert(s.state->aborted == 0);
    assert(s.state->timestamp == -1);
    assert(strcmp(s.state->bitmap, "0000000000000000000000000000") == 0);
    assert(strcmp(s.state->requested_frg, "0000000000000000000000000000") == 0);

    for (int i = 0; i <= rule.max_fragment_number; i++) {
        assert(is_frg_null(&s.fragments[i]));
    }

    free(s.state->bitmap);
    free(s.state->requested_frg);
    free(s.fragments);

    return 0;
}

int test_session_destroy() {
    return -1;
}

int test_session_was_aborted() {
    return -1;
}

int test_session_update_timestamp() {
    return -1;
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
    printf("%d test_session_construct\n", test_session_construct());
    printf("%d test_session_destroy\n", test_session_destroy());
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