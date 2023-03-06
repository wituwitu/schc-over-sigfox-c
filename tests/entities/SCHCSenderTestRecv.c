
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "sigfox_socket.h"

int main() {
    SigfoxServer server;

    // Start
    sgfx_server_start(&server);

    // Receiving fragment without loss
    char buf[UPLINK_MTU_BYTES];
    assert(sgfx_server_recv(&server, buf) == 4);
    printf("Received.\n");
    assert(memcmp(buf, "\x15\x88\x88\x88", 4) == 0);

    // Receiving fragment with loss
    sgfx_server_set_timeout(&server, 1);
    assert(sgfx_server_recv(&server, buf) == -1);
    assert(strlen(buf) == 0);
    printf("Did not receive anything.\n");

    return 0;
}