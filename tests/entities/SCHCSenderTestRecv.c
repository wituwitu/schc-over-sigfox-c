#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "sigfox_socket.h"
#include "ack.h"
#include "casting.h"

int main() {
    SigfoxServer server;
    sgfx_server_start(&server);
    ssize_t recvlen;

    /* ------ test_schc_send ------ */
    printf("------ test_schc_send ------\n");

    // Receiving fragment without loss
    char buf[UPLINK_MTU_BYTES];
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x15\x88\x88\x88", recvlen) == 0);

    // Receiving fragment with loss
    sgfx_server_set_timeout(&server, 1);
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == -1);
    assert(strlen(buf) == 0);
    printf("Did not receive anything.\n");

    /* ------ test_schc_recv ------ */
    printf("------ test_schc_recv ------\n");

    // Respond without loss
    memset(buf, '\0', UPLINK_MTU_BYTES);
    recvlen = sgfx_server_recv(&server, buf);
    assert(memcmp(buf, "\027\200DD", recvlen) == 0);
    Fragment all1;
    memcpy(all1.message, buf, recvlen);
    all1.byte_size = (int) recvlen;
    assert(memcmp(buf, all1.message, recvlen) == 0);
    assert(all1.byte_size == recvlen);
    printf("Received All-1 (1)\n");
    CompoundACK ack;
    memcpy(ack.message, "\x13\xc8\x00\x00\x00\x00\x00\x00", DOWNLINK_MTU_BYTES);
    assert(sgfx_server_send(&server, ack.message) == 8);
    printf("Sent ACK (1)\n");

    // Respond with loss
    recvlen = sgfx_server_recv(&server, buf);
    assert(memcmp(buf, "\027\200DD", recvlen) == 0);
    printf("Received All-1 (1)\n");
    assert(sgfx_server_send(&server, ack.message) == 8);
    printf("Sent ACK (2)\n");

    // Don't respond
    recvlen = sgfx_server_recv(&server, buf);
    assert(memcmp(buf, "\x15\x88\x88\x88", recvlen) == 0);
    printf("Received Fragment\n");

    /* ------ test_schc ------ */
    printf("------ test_schc ------\n");

    // Sending regular with timeout (do nothing)

    // Sending regular without timeout
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x15\x88\x88\x88", recvlen) == 0);

    // Sending all-0 with timeout (do nothing)

    // Sending all-0 with timeout (ACK lost)
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x10\x88\x88\x88", recvlen) == 0);
    char response_all0_bin[] =
            "0000001110001101110011000000000000000000000000000000000000000000";
    char response_all0_bytes[8] = "";
    bin_to_bytes(response_all0_bytes, response_all0_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack2;
    memcpy(ack2.message, response_all0_bin, DOWNLINK_MTU_BYTES);
    ack2.byte_size = DOWNLINK_MTU_BYTES;
    assert(sgfx_server_send(&server, ack2.message) == 8);
    printf("Sent ACK (response to All-0)\n");

    // Sending all-0 with timeout (ack received)
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x10\x88\x88\x88", recvlen) == 0);
    assert(sgfx_server_send(&server, ack2.message) == 8);
    printf("Sent ACK (again) (response to All-0)\n");

    // Send from retransmission queue
    for (int i = 0; i < 5; i++) {
        recvlen = sgfx_server_recv(&server, buf);
        assert(recvlen == 12);
    }

    // Sending all-0 without timeout (receiver-abort received)
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x10\x88\x88\x88", recvlen) == 0);
    Fragment recv;
    memcpy(recv.message, buf, recvlen);
    recv.byte_size = (int) recvlen;
    Rule rule;
    init_rule_from_frg(&rule, &recv);
    CompoundACK ra;
    generate_receiver_abort(&rule, &recv, &ra);
    assert(sgfx_server_send(&server, ra.message) == 8);
    printf("Sent Receiver-Abort (response to All-0)\n");

    // Sending all-1 with timeout (do nothing)

    // Sending all-1 without timeout (ACK lost)
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x1F\xE0\x88\x88", recvlen) == 0);
    char response_all1_bin[] =
            "0000001110001101110011111111001000000000000000000000000000000000";
    char response_all1_bytes[8];
    bin_to_bytes(response_all1_bytes, response_all1_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack3;
    memcpy(ack3.message, response_all1_bytes, DOWNLINK_MTU_BYTES);
    ack3.byte_size = DOWNLINK_MTU_BYTES;
    assert(sgfx_server_send(&server, ack3.message) == 8);
    printf("Sent ACK (response to All-1)\n");

    // Sending all-1 without timeout (incomplete ACK received)
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x1F\xE0\x88\x88", recvlen) == 0);
    assert(sgfx_server_send(&server, ack3.message) == 8);
    printf("Sent ACK (again) (response to All-1)\n");

    // Send from retransmission queue after All-1
    for (int i = 0; i < 7; i++) {
        recvlen = sgfx_server_recv(&server, buf);
        printf("%d\n", (int) recvlen);
        assert(recvlen == 12);
    }

    // Sending all-1 without timeout (complete ACK received)
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x1F\xE0\x88\x88", recvlen) == 0);
    char complete_ack_bin[] =
            "0001110000000000000000000000000000000000000000000000000000000000";
    char complete_ack_bytes[9] = "";
    bin_to_bytes(complete_ack_bytes, complete_ack_bin, DOWNLINK_MTU_BITS);
    CompoundACK complete_ack;
    memcpy(complete_ack.message, complete_ack_bytes, DOWNLINK_MTU_BYTES);
    complete_ack.byte_size = DOWNLINK_MTU_BYTES;
    assert(sgfx_server_send(&server, complete_ack.message) == 8);
    printf("Sent Complete ACK\n");

    // Sending all-1 without timeout (receiver-abort received)
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x1F\xE0\x88\x88", recvlen) == 0);
    assert(sgfx_server_send(&server, ra.message) == 8);
    printf("Sent Receiver-Abort (response to All-1)\n");

    // Sending sender-abort
    recvlen = sgfx_server_recv(&server, buf);
    printf("Received.\n");
    Fragment sa;
    memcpy(sa.message, buf, recvlen);
    sa.byte_size = (int) recvlen;
    assert(is_frg_sender_abort(&rule, &sa));

    return 0;
}