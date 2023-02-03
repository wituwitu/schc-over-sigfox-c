#include <stdio.h>
#include "sigfox_socket.h"

int main() {
    SigfoxClient client;

    sgfx_client_start(&client);

    printf("sock_fd: %d\n", client.sock_fd);
    printf("server_fd: %d\n", client.server_fd);
    printf("expects_ack: %d\n", client.expects_ack);
    printf("seqnum: %d\n", client.seqnum);
    printf("timeout: %f\n", client.timeout);
    printf("buffer: %s\n", client.buffer);

    sgfx_client_set_reception(&client, 1);

    char hello[] = "helloserver.";
    printf("Sending: %s\n", hello);
    sgfx_client_send(&client, hello);
    printf("Sent\n");

    char buf[DOWNLINK_MTU];

    printf("receiving\n");
    sgfx_client_recv(&client, buf);
    printf("received: %s\n", buf);

    sgfx_client_close(&client);

}