#include <stdlib.h>
#include <stdio.h>
#include "schc_sender.h"

void init_sender(SCHCSender *s) {
    s->attempts = 0;
    s->nb_fragments = 0;
    s->last_window = 0;
    sgfx_client_start(&s->socket);
    s->transmission_q = NULL;
    s->retransmission_q = NULL;
    s->rt = 0;
    s->ul_loss_rate = UPLINK_LOSS_RATE;
    s->dl_loss_rate = DOWNLINK_LOSS_RATE;
}

ssize_t schc_send(SCHCSender *s, Rule *rule, Fragment *frg) {
    if (s->ul_loss_rate > 0 && !is_frg_sender_abort(rule, frg)) {
        if (random() % (101) < s->ul_loss_rate) {
            s->socket.seqnum += 1;
            printf("Fragment lost (rate)\n");
            return 0;
        }
    }
    return sgfx_client_send(&s->socket, frg->message, frg->byte_size);
}
