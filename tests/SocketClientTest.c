/* Build SocketServerTest.c and SocketClientTest.c
 * Then execute SocketServerTest and SocketClientTest
 * in separate terminals */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "sigfox_socket.h"

int main() {
    SigfoxClient client;

    // Start
    sgfx_client_start(&client);

    assert(client.sock_fd > 0);
    assert(client.server_fd == 0);
    assert(client.expects_ack == 0);
    assert(client.seqnum == 0);
    assert(client.timeout == 60);
    assert(strlen(client.buffer) == 0);

    // Send (and receive)
    sgfx_client_set_reception(&client, 1);
    assert(client.expects_ack == 1);
    char hello[] = "helloserver.";
    assert(sgfx_client_send(&client, hello) == 12);
    char buf[DOWNLINK_MTU];
    assert(sgfx_client_recv(&client, buf) == 8);
    assert(strcmp(buf, "hellocli") == 0);

    // Trigger timeout at receiver
    sleep(2);

    // Send smaller message without reception
    sgfx_client_set_reception(&client, 0);
    assert(client.expects_ack == 0);
    char hello_small[] = "hello2";
    assert(sgfx_client_send(&client, hello_small) == 6);

    // Timeout
    sgfx_client_set_reception(&client, 1);
    sgfx_client_set_timeout(&client, 1);
    assert(client.expects_ack == 1);
    char hello_third[] = "third";
    assert(sgfx_client_send(&client, hello_third) == -1);

    // Close
    sgfx_client_close(&client);
}