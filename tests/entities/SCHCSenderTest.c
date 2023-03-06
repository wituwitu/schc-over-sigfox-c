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
    assert(s.transmission_q == NULL);
    assert(s.retransmission_q == NULL);
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
    generate_null_frg(&fragments[nb_frgs]);

    // Non-empty retransmission queue
    assert(s.rt == 0);
    s.retransmission_q = fragments;
    update_rt(&s);
    assert(s.rt == 1);

    // Empty retransmission queue
    for (int i = 0; i < nb_frgs; i++) {
        generate_null_frg(&s.retransmission_q[i]);
    }
    update_rt(&s);
    assert(s.rt == 0);

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

int main() {
    printf("%d init_sender_test\n", init_sender_test());
    //printf("%d schc_send_test\n", schc_send_test());
    //printf("%d schc_recv_test\n", schc_recv_test());
    printf("%d update_rt_test\n", update_rt_test());
    printf("%d update_timeout_test\n", update_timeout_test());
    return 0;
}