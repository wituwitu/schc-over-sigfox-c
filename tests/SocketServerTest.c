#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sigfox_socket.h"

int main() {
    SigfoxServer server;

    sgfx_server_start(&server);

    int init = server.timeout == 60;

    char buf[UPLINK_MTU];
    assert(sgfx_server_recv(&server, buf) >= 0);
    assert(strcmp(buf, "helloserver.") == 0);

    char ack[] = "hellocli";

    printf("sending: %s\n", ack);
    assert(sgfx_server_send(&server, ack) >= 0);

    printf("sent\n");
    printf("configuring timeout\n");

    sgfx_server_set_timeout(&server, 100);
    assert(server.timeout == 100);

    printf("%lo\n", sgfx_server_recv(&server, buf));
    printf("%lo\n", sgfx_server_recv(&server, buf));
    printf("%lo\n", sgfx_server_recv(&server, buf));

    assert(sgfx_server_recv(&server, buf) < 1);

    printf("closing\n");
    sgfx_server_close(&server);

    return 0;
}