#include "casting.h"
#include "schc_sender.h"
#include "misc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int init_sender_test() {
    SCHCSender s;
    init_sender(&s);
    assert(s.attempts == 0);
    assert(s.nb_fragments == 0);
    assert(s.last_window == 0);
    assert(s.socket.seqnum == 0);
    assert(s.rt == 0);
    assert(s.ul_loss_rate == UPLINK_LOSS_RATE);
    assert(s.dl_loss_rate == DOWNLINK_LOSS_RATE);

    return 0;
}

int schc_send_test() {
    SCHCSender s;
    init_sender(&s);
    Rule rule;
    init_rule(&rule, "000");

    // Send fragment without loss
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    assert(schc_send(&s, &rule, &fragment) == 4);
    assert(s.socket.seqnum == 1);

    // Send fragment with loss
    s.ul_loss_rate = 100;
    assert(schc_send(&s, &rule, &fragment) == 4);
    assert(s.socket.seqnum == 2);

    sleep(2);

    return 0;
}

int schc_recv_test() {
    SCHCSender s;
    init_sender(&s);
    Rule rule;
    init_rule(&rule, "000");

    // Send and recv fragment without loss
    Fragment all_1 = {"\027\200DD", 4};
    s.socket.expects_ack = 1;
    schc_send(&s, &rule, &all_1);
    CompoundACK ack_1;
    assert(schc_recv(&s, &rule, &ack_1) == DOWNLINK_MTU_BYTES);
    assert(memcmp(ack_1.message, "\x13\xc8\x00\x00\x00\x00\x00\x00",
                  DOWNLINK_MTU_BYTES) == 0);

    // Send and recv fragment with DL loss
    s.dl_loss_rate = 100;
    s.socket.timeout = 1;
    CompoundACK ack_2;
    schc_send(&s, &rule, &all_1);
    assert(schc_recv(&s, &rule, &ack_2) == -1);

    // Send and recv fragment without expecting it
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    s.dl_loss_rate = 0;
    s.socket.expects_ack = 0;
    CompoundACK ack_3;
    schc_send(&s, &rule, &fragment);
    assert(schc_recv(&s, &rule, &ack_3) == 0);

    return 0;
}

int update_rt_test() {
    SCHCSender s;
    init_sender(&s);
    Rule rule;
    init_rule(&rule, "000");

    int size = 11;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    int nb_frgs = get_nb_fragments(&rule, size);
    Fragment fragments[nb_frgs + 1];
    fragment(&rule, fragments, schc_packet, size);

    fq_construct(&s.retransmission_q, nb_frgs + 1);

    // Empty retransmission queue
    assert(s.rt == 0);
    update_rt(&s);
    assert(s.rt == 0);

    // Non empty retransmission queue
    fq_write(&s.retransmission_q, &fragments[0]);
    assert(s.rt == 0);
    update_rt(&s);
    assert(s.rt == 1);

    fq_destroy(&s.retransmission_q);

    return 0;
}

int update_timeout_test() {
    SCHCSender s;
    init_sender(&s);
    Rule rule;
    init_rule(&rule, "000");

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

    return 0;
}

int get_bitmap_to_retransmit_test() {
    SCHCSender s;
    init_sender(&s);
    Rule rule;
    init_rule(&rule, "000");
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

    return 0;
}

int update_rt_queue_test() {

    SCHCSender s;
    init_sender(&s);
    Rule rule;
    init_rule(&rule, "000");

    fq_construct(&s.transmission_q, rule.max_fragment_number + 1);
    fq_construct(&s.retransmission_q, rule.max_fragment_number + 1);

    // Fragment a SCHC Packet
    int size = 300;
    char schc_packet[size];
    generate_packet(schc_packet, size);
    int nb_frgs = get_nb_fragments(&rule, size);
    Fragment fragments[nb_frgs + 1];
    fragment(&rule, fragments, schc_packet, size);
    s.fragments = fragments;
    s.nb_fragments = nb_frgs;
    s.last_window = 3;

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
    assert(update_rt_queue(&s, &rule, &all0, &ack1) == -1);

    // Get ACK
    char ack2_bin[] =
            "0000001110001101110011000000000000000000000000000000000000000000";
    char ack2_bytes[8] = "";
    bin_to_bytes(ack2_bytes, ack2_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack2;
    memcpy(ack2.message, ack2_bytes, DOWNLINK_MTU_BYTES);

    // Update RT queue
    assert(update_rt_queue(&s, &rule, &all0, &ack2) == 0);

    // Check fragments in RT queue
    int i = 0;
    while (!fq_is_empty(&s.retransmission_q)) {
        void *rd = fq_read(&s.retransmission_q);
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
    assert(update_rt_queue(&s, &rule, &all1, &ack3) == 0);

    // Check fragments in RT queue
    int j = 0;
    while (!fq_is_empty(&s.retransmission_q)) {
        void *rd = fq_read(&s.retransmission_q);
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
    Fragment *get = (Fragment *) fq_read(&s.transmission_q);
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
    assert(update_rt_queue(&s, &rule, &all1, &ack4) == 0);

    // Check RT queue
    assert(fq_is_empty(&s.retransmission_q));

    // Check T queue
    assert(!fq_is_empty(&s.transmission_q));
    Fragment *sa = (Fragment *) fq_read(&s.transmission_q);
    assert(is_frg_sender_abort(&rule, sa));
    assert(fq_is_empty(&s.transmission_q));

    fq_destroy(&s.transmission_q);
    fq_destroy(&s.retransmission_q);

    return 0;
}

int main() {
    printf("%d init_sender_test\n", init_sender_test());
    //printf("%d schc_send_test\n", schc_send_test());
    //printf("%d schc_recv_test\n", schc_recv_test());
    printf("%d update_rt_test\n", update_rt_test());
    printf("%d update_timeout_test\n", update_timeout_test());
    printf("%d get_bitmap_to_retransmit_test\n",
           get_bitmap_to_retransmit_test());
    printf("%d update_rt_queue_test\n", update_rt_queue_test());

    return 0;
}