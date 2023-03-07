
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "sigfox_socket.h"
#include "ack.h"

int main() {
    SigfoxServer server;
    sgfx_server_start(&server);
    ssize_t recvlen;

    /* ------ schc_send_test ------ */
    printf("------ schc_send_test ------\n");

    // Receiving fragment without loss
    char buf[UPLINK_MTU_BYTES];
    recvlen = sgfx_server_recv(&server, buf);
    assert(recvlen == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x15\x88\x88\x88", recvlen) == 0);

    // Receiving fragment with loss
    sgfx_server_set_timeout(&server, 1);
    recvlen = sgfx_server_recv(&server, buf);
    printf("%ld\n", recvlen);
    assert(recvlen == -1);
    assert(strlen(buf) == 0);
    printf("Did not receive anything.\n");

    /* ------ schc_recv_test ------ */
    printf("------ schc_recv_test ------\n");

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

    return 0;
}