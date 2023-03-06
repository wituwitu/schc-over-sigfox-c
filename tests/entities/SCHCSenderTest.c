#include "casting.h"
#include "schc_sender.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

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

    // Send fragment without loss
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule(&rule, "000");
    schc_send(&s, &rule, &fragment);
    assert(s.socket.seqnum == 1);

    // Send fragment with loss
    s.ul_loss_rate = 100;
    schc_send(&s, &rule, &fragment);
    assert(s.socket.seqnum == 2);

    return 0;
}

int main() {
    printf("%d init_sender_test\n", init_sender_test());
    printf("%d schc_send_test\n", schc_send_test());
    return 0;
}