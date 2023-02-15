//
// Created by witu on 1/30/23.
//

#ifndef SCHC_OVER_SIGFOX_C_SCHC_H
#define SCHC_OVER_SIGFOX_C_SCHC_H

#endif //SCHC_OVER_SIGFOX_C_SCHC_H

extern char FR_MODE[];
extern int RETRANSMISSION_TIMEOUT;
extern int SIGFOX_DL_TIMEOUT;
extern int INACTIVITY_TIMEOUT;
extern int MAX_ACK_REQUESTS;
extern const int L2_WORD_SIZE;
extern int DELAY_BETWEEN_FRAGMENTS;

// Sender config
extern int ENABLE_MAX_ACK_REQUESTS;

// Receiver config
extern int RESET_DATA_AFTER_REASSEMBLY;
extern int CHECK_FOR_CALLBACK_RETRIES;
extern int DISABLE_INACTIVITY_TIMEOUT;
extern char RECEIVER_URL[];