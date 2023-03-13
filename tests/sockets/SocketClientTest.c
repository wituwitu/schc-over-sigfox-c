/* Build SocketServerTest.c and SocketClientTest.c
 * Then execute SocketServerTest and SocketClientTest
 * in separate terminals */

#include "casting.h"
#include "sigfox_socket.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    SigfoxClient client;
    char hello[] = "helloserver.";
    char buf[DOWNLINK_MTU_BYTES];

    // Start
    sgfx_client_start(&client);

    assert(client.sock_fd > 0);
    assert(client.expects_ack == 0);
    assert(client.seqnum == 0);
    assert(client.timeout == 60);
    assert(strlen(client.buffer) == 0);

    // Send (and receive)
    sgfx_client_set_reception(&client, 1);
    assert(client.expects_ack == 1);
    assert(sgfx_client_send(&client, hello, 12) == 12);
    assert(sgfx_client_recv(&client, buf) == 8);
    assert(memcmp(buf, "hellocli", DOWNLINK_MTU_BYTES) == 0);

    // Trigger timeout at receiver
    sleep(2);

    // Send smaller message without reception
    sgfx_client_set_reception(&client, 0);
    assert(client.expects_ack == 0);
    char hello_small[] = "hello2";
    assert(sgfx_client_send(&client, hello_small, 6) == 6);

    // Timeout
    sgfx_client_set_reception(&client, 1);
    sgfx_client_set_timeout(&client, 1);
    assert(client.expects_ack == 1);
    char hello_third[] = "third";
    assert(sgfx_client_send(&client, hello_third, 5) == -1);

    // Sending and receiving messages with null bytes
    sgfx_client_set_reception(&client, 1);
    sgfx_client_set_timeout(&client, 60);
    char w_null_bytes[] = "\x15\x88\x00\x88\x88";
    assert(sgfx_client_send(&client, w_null_bytes, 5) == 5);
    assert(sgfx_client_recv(&client, buf) == 8);
    char as_bin[64];
    bytes_to_bin(as_bin, buf, 8);
    printf("Received (bin): %s\n", as_bin);
    assert(memcmp(buf, "\xFF\x00\xFF\x00\x00\x00\x00\x00",
                  DOWNLINK_MTU_BYTES) == 0);
    assert(strcmp(as_bin, "11111111000000001111111100000000"
                          "00000000000000000000000000000000") == 0);

    // Close
    sgfx_client_close(&client);

    return 0;
}