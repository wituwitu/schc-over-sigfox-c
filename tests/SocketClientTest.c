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
    printf("Sending: %s\n", hello);
    assert(sgfx_client_send(&client, hello) == 12);
    printf("Sent\n");
    char buf[DOWNLINK_MTU];
    printf("receiving\n");
    printf("%ld\n", sgfx_client_recv(&client, buf));
    printf("received: %s\n", buf);

    sleep(2);

    sgfx_client_set_reception(&client, 0);

    char hello_small[] = "hello2";
    printf("Sending: %s\n", hello_small);
    sgfx_client_send(&client, hello_small);
    printf("Sent\n");

    sgfx_client_close(&client);

}