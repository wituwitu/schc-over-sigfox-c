/* Build SocketServerTest.c and SocketClientTest.c
 * Then execute SocketServerTest and SocketClientTest
 * in separate terminals */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "sigfox_socket.h"

int main() {
    SigfoxServer server;

    // Start
    sgfx_server_start(&server);
    assert(server.sock_fd > 0);
    assert(server.timeout == 60);

    // Receiving a message
    char buf[UPLINK_MTU];
    assert(sgfx_server_recv(&server, buf) == 12);
    printf("Received: %s\n", buf);
    assert(strcmp(buf, "helloserver.") == 0);

    // Sending a reply
    char ack[] = "hellocli";
    assert(sgfx_server_send(&server, ack) >= 0);

    // Configuring timeout
    sgfx_server_set_timeout(&server, 1);
    assert(server.timeout == 1);

    // Read timeout
    assert(sgfx_server_recv(&server, buf) == -1);
    assert(strlen(buf) == 0);

    // Reading after timeout without answering
    assert(sgfx_server_recv(&server, buf) == 6);
    printf("Received: %s\n", buf);
    assert(strlen(buf) == 6);
    assert(strcmp(buf, "hello2") == 0);

    // Triggering timeout at sender
    assert(sgfx_server_recv(&server, buf) == 5);
    printf("Received: %s\n", buf);
    assert(strlen(buf) == 5);
    assert(strcmp(buf, "third") == 0);
    sleep(2);

    // Close
    sgfx_server_close(&server);

    return 0;
}