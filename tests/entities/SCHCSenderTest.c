#include "casting.h"
#include "schc_sender.h"
#include "misc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

int test_sender_construct() {
    // Generate a SCHC Packet
    int size = 300;
    char schc_packet[size];
    generate_packet(schc_packet, size);

    // Init rule
    Rule rule;
    init_rule(&rule, "000");

    SCHCSender s;
    sender_construct(&s, &rule, schc_packet, size);

    assert(s.socket.seqnum == 0);
    assert(s.nb_fragments == 28);
    assert(s.last_window == 3);
    assert(s.fragments != NULL);

    for (int i = 0; i < s.nb_fragments; i++) {
        if (is_frg_all_1(&rule, &s.fragments[i])) {
            assert(i == 27);
            assert(s.fragments[i].byte_size == 5);
        } else {
            assert(s.fragments[i].byte_size == 12);
        }
    }

    assert(fq_is_empty(&s.transmission_q));
    assert(fq_is_empty(&s.retransmission_q));
    assert(s.attempts == 0);
    assert(s.rt == 0);
    assert(s.ul_loss_rate == UPLINK_LOSS_RATE);
    assert(s.dl_loss_rate == DOWNLINK_LOSS_RATE);

    // Free memory
    fq_destroy(&s.transmission_q);
    fq_destroy(&s.retransmission_q);
    free(s.fragments);

    return 0;
}

int test_sender_destroy() {
    int size = 300;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    Rule rule;
    init_rule(&rule, "000");
    SCHCSender s;
    sender_construct(&s, &rule, schc_packet, size);

    sender_destroy(&s);
    assert(sgfx_client_send(&s.socket, "test", 4) == -1);
    assert(s.fragments == NULL);
    assert(s.transmission_q.objs == NULL);
    assert(s.retransmission_q.objs == NULL);

    return 0;
}

int test_update_rt() {
    int size = 11;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    Rule rule;
    init_rule(&rule, "000");
    SCHCSender s;
    sender_construct(&s, &rule, schc_packet, size);

    // Empty retransmission queue
    assert(s.rt == 0);
    update_rt(&s);
    assert(s.rt == 0);

    // Non empty retransmission queue
    fq_put(&s.retransmission_q, &s.fragments[0]);
    assert(s.rt == 0);
    update_rt(&s);
    assert(s.rt == 1);

    sender_destroy(&s);
    return 0;
}

int test_update_timeout() {
    Rule rule;
    init_rule(&rule, "000");
    SCHCSender s;
    sender_construct(&s, &rule, "SCHCPACKET", 10);

    // Normal fragment
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    update_timeout(&s, &rule, &fragment);
    assert(s.socket.timeout == 60);

    // All-0
    Fragment all_0 = {"\000DDD", 4};
    update_timeout(&s, &rule, &all_0);
    assert(s.socket.timeout == SIGFOX_DL_TIMEOUT);

    // All-1
    Fragment all_1 = {"\027\200DD", 4};
    update_timeout(&s, &rule, &all_1);
    assert(s.socket.timeout == RETRANSMISSION_TIMEOUT);

    sender_destroy(&s);
    return 0;
}

int test_get_bitmap_to_retransmit() {
    Rule rule;
    init_rule(&rule, "000");
    SCHCSender s;
    sender_construct(&s, &rule, "SCHCPACKET", 10);
    char bitmap_to_retransmit[rule.window_size + 1];

    // Bitmap with losses of non-final window
    memset(bitmap_to_retransmit, '\0', rule.window_size + 1);
    s.nb_fragments = 14;
    s.last_window = 1;
    int ack_window_1 = 0;
    char bitmap_1[] = "1001011";

    assert(get_bitmap_to_retransmit(
            &s, &rule, ack_window_1,
            bitmap_1, bitmap_to_retransmit
    ) == rule.window_size);
    assert(strcmp(bitmap_to_retransmit, "1001011") == 0);

    // Bitmap without losses of non-final window
    memset(bitmap_to_retransmit, '\0', rule.window_size + 1);
    int ack_window_2 = 0;
    char bitmap_2[] = "1111111";

    assert(get_bitmap_to_retransmit(
            &s, &rule, ack_window_2,
            bitmap_2, bitmap_to_retransmit
    ) == -1);

    // Bitmap with losses of final window, full size
    memset(bitmap_to_retransmit, '\0', rule.window_size + 1);
    int ack_window_3 = 1;
    char bitmap_3[] = "1011001";
    assert(get_bitmap_to_retransmit(
            &s, &rule, ack_window_3,
            bitmap_3, bitmap_to_retransmit
    ) == rule.window_size - 1);
    assert(strcmp(bitmap_to_retransmit, "101100") == 0);

    // Bitmap without losses of final window, full size
    memset(bitmap_to_retransmit, '\0', rule.window_size + 1);
    int ack_window_4 = 1;
    char bitmap_4[] = "1111111";
    assert(get_bitmap_to_retransmit(
            &s, &rule, ack_window_4,
            bitmap_4, bitmap_to_retransmit
    ) == -1);

    // Bitmap with losses of final window, smaller size
    memset(bitmap_to_retransmit, '\0', rule.window_size + 1);
    s.nb_fragments = 12;
    int ack_window_5 = 1;
    char bitmap_5[] = "1011001";
    assert(get_bitmap_to_retransmit(
            &s, &rule, ack_window_5,
            bitmap_5, bitmap_to_retransmit
    ) == 4);
    assert(strcmp(bitmap_to_retransmit, "1011") == 0);
    memset(bitmap_to_retransmit, '\0', rule.window_size + 1);

    // Bitmap without losses of final window, smaller size
    memset(bitmap_to_retransmit, '\0', rule.window_size + 1);
    int ack_window_6 = 1;
    char bitmap_6[] = "1111001";
    assert(get_bitmap_to_retransmit(
            &s, &rule, ack_window_6,
            bitmap_6, bitmap_to_retransmit
    ) == -1);

    // Bitmap without losses of final window, minimum size
    memset(bitmap_to_retransmit, '\0', rule.window_size + 1);
    s.nb_fragments = 8;
    int ack_window_7 = 1;
    char bitmap_7[] = "0000001";
    assert(get_bitmap_to_retransmit(
            &s, &rule, ack_window_7,
            bitmap_7, bitmap_to_retransmit
    ) == -1);

    sender_destroy(&s);
    return 0;
}

int test_update_queues() {
    Rule rule;
    init_rule(&rule, "000");

    // Fragment a SCHC Packet
    int size = 300;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    int nb_frgs = get_nb_fragments(&rule, size);
    Fragment fragments[nb_frgs + 1];
    fragment(&rule, fragments, schc_packet, size);

    SCHCSender s;
    sender_construct(&s, &rule, schc_packet, size);

    // --------- All-0 fragment (window "10") ---------
    char all0_bin[] = "0001000000000000000000000000000000000000";
    Fragment all0;
    all0.byte_size = bin_to_bytes(all0.message, all0_bin, 40);

    // Get ACK
    char ack1_bin[] =
            "0000001110001101110011111111001000000000000000000000000000000000";
    char ack1_bytes[8] = "";
    bin_to_bytes(ack1_bytes, ack1_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack1;
    memcpy(ack1.message, ack1_bytes, DOWNLINK_MTU_BYTES);

    // Trigger error (ACK reported greater window)
    assert(update_queues(&s, &rule, &all0, &ack1) == -1);

    // Get ACK
    char ack2_bin[] =
            "0000001110001101110011000000000000000000000000000000000000000000";
    char ack2_bytes[8] = "";
    bin_to_bytes(ack2_bytes, ack2_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack2;
    memcpy(ack2.message, ack2_bytes, DOWNLINK_MTU_BYTES);

    // Update RT queue
    assert(update_queues(&s, &rule, &all0, &ack2) == 0);

    // Check fragments in RT queue
    int i = 0;
    while (!fq_is_empty(&s.retransmission_q)) {
        void *rd = fq_get(&s.retransmission_q);
        if (rd == NULL) {
            break;
        }

        Fragment *ptr = (Fragment *) rd;
        int idx = get_frg_idx(&rule, ptr);

        Fragment lost = fragments[idx];

        assert(memcmp(ptr->message, lost.message, ptr->byte_size) == 0);
        assert(ptr->byte_size == lost.byte_size);
        i++;
    }
    assert(i == 5);
    assert(fq_is_empty(&s.retransmission_q));

    // --------- All-1 fragment ---------
    Fragment all1 = {"\x1F\200DD", 4};
    assert(is_frg_all_1(&rule, &all1));

    // Get ACK
    char ack3_bin[] =
            "0000001110001101110011111111001000000000000000000000000000000000";
    char ack3_bytes[8] = "";
    bin_to_bytes(ack3_bytes, ack3_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack3;
    memcpy(ack3.message, ack3_bytes, DOWNLINK_MTU_BYTES);

    // Update RT queue
    assert(update_queues(&s, &rule, &all1, &ack3) == 0);

    // Check fragments in RT queue
    int j = 0;
    while (!fq_is_empty(&s.retransmission_q)) {
        void *rd = fq_get(&s.retransmission_q);
        if (rd == NULL) {
            break;
        }

        Fragment *ptr = (Fragment *) rd;
        int idx = get_frg_idx(&rule, ptr);

        Fragment lost = fragments[idx];

        assert(memcmp(ptr->message, lost.message, ptr->byte_size) == 0);
        assert(ptr->byte_size == lost.byte_size);
        j++;
    }
    assert(j == 7);
    assert(fq_is_empty(&s.retransmission_q));

    // Check if All-1 was queued
    assert(!fq_is_empty(&s.transmission_q));
    Fragment *get = (Fragment *) fq_get(&s.transmission_q);
    assert(is_frg_all_1(&rule, get));
    assert(fq_is_empty(&s.transmission_q));

    // --------- No missing tile at receiver ---------

    // Get ACK
    char ack4_bin[] =
            "0000001111111000000000000000000000000000000000000000000000000000";
    char ack4_bytes[8] = "";
    bin_to_bytes(ack4_bytes, ack4_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack4;
    memcpy(ack4.message, ack4_bytes, DOWNLINK_MTU_BYTES);

    // Update RT queue
    assert(update_queues(&s, &rule, &all1, &ack4) == 0);

    // Check RT queue
    assert(fq_is_empty(&s.retransmission_q));

    // Check T queue
    assert(!fq_is_empty(&s.transmission_q));
    Fragment *sa = (Fragment *) fq_get(&s.transmission_q);
    assert(is_frg_sender_abort(&rule, sa));
    assert(fq_is_empty(&s.transmission_q));

    sender_destroy(&s);
    return 0;
}

int test_timeout_procedure() {
    Rule rule;
    init_rule(&rule, "000");
    int size = 300;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    SCHCSender s;
    sender_construct(&s, &rule, schc_packet, size);

    // Normal fragment timeout
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    assert(timeout_procedure(&s, &rule, &fragment) == -1);

    // All-0 timeout
    Fragment all0 = {"\000DDD", 4};
    assert(timeout_procedure(&s, &rule, &all0) == 0);

    // All-1 timeout not having reached MAX_ACK_REQUESTS
    Fragment all1 = {"\027\200DD", 4};
    assert(timeout_procedure(&s, &rule, &all1) == 0);
    assert(!fq_is_empty(&s.transmission_q));
    Fragment *just_put = fq_get(&s.transmission_q);
    assert(memcmp(just_put->message, all1.message, all1.byte_size) == 0);
    assert(just_put->byte_size == all1.byte_size);

    // All-1 timeout having reached MAX_ACK_REQUESTS
    assert(fq_is_empty(&s.transmission_q));
    s.attempts = MAX_ACK_REQUESTS;
    assert(timeout_procedure(&s, &rule, &all1) == 0);
    assert(!fq_is_empty(&s.transmission_q));
    Fragment *sa = fq_get(&s.transmission_q);
    assert(is_frg_sender_abort(&rule, sa));

    sender_destroy(&s);
    return 0;
}

int test_schc_send() {
    Rule rule;
    init_rule(&rule, "000");
    int size = 300;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    SCHCSender s;
    sender_construct(&s, &rule, schc_packet, size);

    // Send fragment without loss
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    assert(schc_send(&s, &rule, &fragment) == 4);
    assert(s.socket.seqnum == 1);

    // Send fragment with loss
    s.ul_loss_rate = 100;
    assert(schc_send(&s, &rule, &fragment) == 4);
    assert(s.socket.seqnum == 2);

    sleep(2);
    sender_destroy(&s);
    return 0;
}

int test_schc_recv() {
    Rule rule;
    init_rule(&rule, "000");
    int size = 300;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    SCHCSender s;
    sender_construct(&s, &rule, schc_packet, size);

    // Send and recv fragment without loss
    Fragment all_1 = {"\027\200DD", 4};
    s.socket.expects_ack = 1;
    schc_send(&s, &rule, &all_1);
    CompoundACK ack_1;
    assert(schc_recv(&s, &ack_1) == DOWNLINK_MTU_BYTES);
    assert(memcmp(ack_1.message, "\x13\xc8\x00\x00\x00\x00\x00\x00",
                  DOWNLINK_MTU_BYTES) == 0);

    // Send and recv fragment with DL loss
    s.dl_loss_rate = 100;
    s.socket.timeout = 1;
    CompoundACK ack_2;
    schc_send(&s, &rule, &all_1);
    assert(schc_recv(&s, &ack_2) == -1);

    // Send and recv fragment without expecting it
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    s.dl_loss_rate = 0;
    s.socket.expects_ack = 0;
    CompoundACK ack_3;
    schc_send(&s, &rule, &fragment);
    assert(schc_recv(&s, &ack_3) == 0);

    sender_destroy(&s);
    return 0;
}

int test_schc() {
    Rule rule;
    init_rule(&rule, "000");
    int size = 300;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    SCHCSender s;
    sender_construct(&s, &rule, schc_packet, size);

    // Sending regular with timeout
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    s.ul_loss_rate = 100;
    assert(schc(&s, &rule, &fragment) == 0);
    s.ul_loss_rate = 0;

    // Sending regular without timeout
    assert(schc(&s, &rule, &fragment) == 0);
    assert(s.rt == 0);

    // Sending all-0 with timeout (Fragment lost)
    Fragment all0 = {"\x10\x88\x88\x88", 4};
    s.ul_loss_rate = 100;
    assert(schc(&s, &rule, &all0) == 0);
    assert(s.rt == 0);
    s.ul_loss_rate = 0;

    // Sending all-0 with timeout (ACK lost)
    s.dl_loss_rate = 100;
    assert(schc(&s, &rule, &all0) == 0);
    s.dl_loss_rate = 0;

    // Sending all-0 with timeout (no response)
    assert(schc(&s, &rule, &all0) == 0);

    // Sending all-0 without timeout (ACK received)
    assert(schc(&s, &rule, &all0) == 0);
    assert(s.rt == 1);
    assert(s.attempts == 0);
    assert(!fq_is_empty(&s.retransmission_q));

    // Send from retransmission queue
    while (!fq_is_empty(&s.retransmission_q)) {
        Fragment *to_send = fq_get(&s.retransmission_q);
        assert(s.rt == 1);
        assert(schc(&s, &rule, to_send) == 0);
    }
    assert(s.rt == 0);

    // Sending all-0 without timeout (receiver-abort received)
    assert(schc(&s, &rule, &all0) == SCHC_RECEIVER_ABORTED);

    // Sending all-1 with timeout
    Fragment all1 = {"\x1F\xE0\x88\x88", 4};
    s.ul_loss_rate = 100;
    assert(schc(&s, &rule, &all1) == 0);
    assert(s.attempts == 1);
    assert(!fq_is_empty(&s.transmission_q));
    Fragment *tr1 = fq_get(&s.transmission_q);
    assert(is_frg_all_1(&rule, tr1));
    s.ul_loss_rate = 0;

    // Sending all-1 without timeout (ACK lost)
    s.dl_loss_rate = 100;
    assert(schc(&s, &rule, &all1) == 0);
    assert(s.attempts == 2);
    assert(!fq_is_empty(&s.transmission_q));
    Fragment *tr2 = fq_get(&s.transmission_q);
    assert(is_frg_all_1(&rule, tr2));
    s.dl_loss_rate = 0;

    // Sending all-1 without timeout (incomplete ACK received)
    assert(schc(&s, &rule, &all1) == 0);
    assert(s.rt == 1);
    assert(s.attempts == 0);
    assert(!fq_is_empty(&s.retransmission_q));

    // Send from retransmission queue after All-1
    while (!fq_is_empty(&s.retransmission_q)) {
        Fragment *to_send = fq_get(&s.retransmission_q);
        assert(s.rt == 1);
        assert(schc(&s, &rule, to_send) == 0);
    }
    assert(s.rt == 0);
    Fragment *enqueued = fq_get(&s.transmission_q);
    assert(is_frg_all_1(&rule, enqueued));
    assert(memcmp(enqueued->message, all1.message, enqueued->byte_size) == 0);
    assert(enqueued->byte_size == all1.byte_size);

    // Sending all-1 without timeout (complete ack received)
    assert(schc(&s, &rule, enqueued) == SCHC_COMPLETED);

    // Sending all-1 without timeout (receiver-abort received)
    assert(schc(&s, &rule, &all1) == SCHC_RECEIVER_ABORTED);

    // Sending sender-abort
    Fragment sa;
    generate_sender_abort(&rule, &all1, &sa);
    assert(schc(&s, &rule, &sa) == SCHC_SENDER_ABORTED);

    sender_destroy(&s);
    return 0;
}

int test_sender_start() {
    Rule rule;
    init_rule(&rule, "000");
    int size = 300;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    SCHCSender s;
    sender_construct(&s, &rule, schc_packet, size);

    sender_start(&s, &rule);

    assert(fq_is_empty(&s.transmission_q));
    assert(fq_is_empty(&s.retransmission_q));

    sender_destroy(&s);

    return 0;
}

int main() {
    printf("%d test_sender_construct\n", test_sender_construct());
    printf("%d test_sender_destroy\n", test_sender_destroy());
    printf("%d test_update_rt\n", test_update_rt());
    printf("%d test_update_timeout\n", test_update_timeout());
    printf("%d test_get_bitmap_to_retransmit\n",
           test_get_bitmap_to_retransmit());
    printf("%d test_update_queues\n", test_update_queues());
    printf("%d test_timeout_procedure\n", test_timeout_procedure());

    // The below tests require SCHCSenderTestRecv.c to be run
    // in a separate terminal.

    printf("%d test_schc_send\n", test_schc_send());
    printf("%d test_schc_recv\n", test_schc_recv());
    printf("%d test_schc\n", test_schc());
    printf("%d test_sender_start\n", test_sender_start());

    return 0;
}