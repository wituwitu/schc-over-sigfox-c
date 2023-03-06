#include "sigfox_socket.h"
#include "fr_procedure.h"

#ifndef SCHC_OVER_SIGFOX_C_SCHC_SENDER_H
#define SCHC_OVER_SIGFOX_C_SCHC_SENDER_H

#endif //SCHC_OVER_SIGFOX_C_SCHC_SENDER_H

typedef struct {
    int attempts;
    int nb_fragments;
    int last_window;
    SigfoxClient socket;
    Fragment *transmission_q;
    Fragment *retransmission_q;
    int rt;
    int ul_loss_rate;
    int dl_loss_rate;
} SCHCSender;

/*
 * Function:  init_sender
 * --------------------
 * Initializes the parameters of the SCHCSender structure.
 *
 *  s: SCHCSender structure to be initialized.
 */
void init_sender(SCHCSender *s);

/*
 * Function:  schc_send
 * --------------------
 * Sends a SCHC Fragment towards the receiver end. Returns -1 on errors.
 *
 *  s: SCHCSender structure that controls the communication.
 *  rule: Rule used to process the communication.
 *  frg: Fragment to be sent.
 */
ssize_t schc_send(SCHCSender *s, Rule *rule, Fragment *frg);

/*
 * Function:  schc_recv
 * --------------------
 * Waits for a response from the SCHC Receiver. If a response is obtained,
 * it is parsed as a CompoundACK. Returns the number of bytes received.
 * If no response is obtained, returns -1.
 *
 *  s: SCHCSender structure that controls the communication.
 *  rule: Rule used to process the communication.
 */
ssize_t schc_recv(SCHCSender *s, Rule *rule, CompoundACK *dest);

/*
 * Function:  update_rt
 * --------------------
 * Updates the retransmission flag of the SCHCSender. This function should be
 * called before and after processing an ACK.
 *
 *  s: SCHCSender structure that controls the communication.
 */
void update_rt(SCHCSender *s);

/*
 * Function:  update_timeout
 * --------------------
 * Configures the Sigfox socket timeout according to the SCHC Fragment to be
 * sent.
 *
 *  s: SCHCSender structure that controls the communication.
 *  frg: Fragment to be sent.
 */
void update_timeout(SCHCSender *s, Rule *rule, Fragment *frg);

/*
 * Function:  update_queues
 * --------------------
 * Updates the transmission and retransmission queues with information of
 * the current state of the SCHC transmission, provided by the SCHC Fragment
 * just sent and the ACK received.
 *
 *  s: SCHCSender structure that controls the communication.
 *  frg: Fragment just sent.
 *  ack: CompoundACK just received.
 */
int update_queues(SCHCSender *s, Fragment *frg, CompoundACK *ack);

/*
 * Function:  schc
 * --------------------
 * Performs the SCHC Sender behavior to send a single SCHC Fragment and update
 * internal variables.
 * Returns -1 on errors, such as sending a Sender-Abort, receiving a
 * Receiver-Abort or a network timeout.
 *
 *  s: SCHCSender structure that controls the communication.
 *  rule: Rule used to process the communication.
 *  frg: Fragment to be sent.
 */
int schc(SCHCSender *s, Rule *rule, Fragment *frg);

/*
 * Function:  start
 * --------------------
 * Performs the full SCHC Sender procedure (fragmentation and delivery)
 * for a SCHC Packet.
 *
 *  s: SCHCSender structure that controls the communication.
 *  rule: Rule to be used to process the communication.
 *  schc_packet: SCHC Packet to be fragmented and sent.
 */
int start(SCHCSender *s, Rule *rule, char schc_packet[]);