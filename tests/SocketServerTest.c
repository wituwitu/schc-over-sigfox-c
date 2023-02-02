#include <stdio.h>
#include <string.h>
#include "sigfox_socket.h"

int main() {
    SigfoxServer server;

    sgfx_server_start(&server);

    int init = server.timeout == 60 && strlen(server.buffer) == 0;
    printf("%d\n", (int) strlen(server.buffer));

    printf("sock_fd: %d\n", server.sock_fd);
    printf("client_fd: %d\n", server.client_fd);
    printf("timeout: %d\n", server.timeout);
    printf("buffer: %s\n", server.buffer);

    char buf[12];

    printf("receiving\n");
    sgfx_server_recv(&server, buf);
    printf("received: %s\n", buf);

    char ack[] = "hellocl\n";

    printf("sending: %s\n", ack);
    sgfx_server_send(&server, ack);

    printf("sent\n");

    printf("closing\n");
    sgfx_server_close(&server);

}