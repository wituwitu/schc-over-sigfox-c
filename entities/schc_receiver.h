#include "sigfox_socket.h"
#include "schc_session.h"

#ifndef SCHC_OVER_SIGFOX_C_SCHC_RECEIVER_H
#define SCHC_OVER_SIGFOX_C_SCHC_RECEIVER_H

#endif //SCHC_OVER_SIGFOX_C_SCHC_RECEIVER_H

typedef struct {
    SigfoxServer socket;
    SCHCSession sessions[MAX_NB_RULE_IDS];
} SCHCReceiver;