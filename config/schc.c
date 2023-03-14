//
// Created by witu on 1/30/23.
//

#include "schc.h"

char FR_MODE[] = "ACK ON ERROR";
int RETRANSMISSION_TIMEOUT = 100;
int SIGFOX_DL_TIMEOUT = 60; /* in seconds */
int INACTIVITY_TIMEOUT = 500;  /* in seconds */
int MAX_ACK_REQUESTS = 5;
const int L2_WORD_SIZE = 8;
int DELAY_BETWEEN_FRAGMENTS = 10; /* in seconds */

/* Sender config */
int DISABLE_MAX_ACK_REQUESTS = 0;
int UPLINK_LOSS_RATE = 0;
int DOWNLINK_LOSS_RATE = 0;

/* Receiver config */
int RESET_DATA_AFTER_REASSEMBLY = 0;
int CHECK_FOR_CALLBACK_RETRIES = 1;
int DISABLE_INACTIVITY_TIMEOUT = 0;

char RECEIVER_URL[] = "http://localhost:5000/receive";

