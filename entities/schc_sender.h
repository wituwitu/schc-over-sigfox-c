#include "sigfox_socket.h"
#include "fr_procedure.h"
#include "fifo_queue.h"

#ifndef SCHC_OVER_SIGFOX_C_SCHC_SENDER_H
#define SCHC_OVER_SIGFOX_C_SCHC_SENDER_H

#endif //SCHC_OVER_SIGFOX_C_SCHC_SENDER_H

typedef struct {
    SigfoxClient socket;
    Fragment *fragments;
    int nb_fragments;
    int last_window;
    FIFOQueue transmission_q;
    FIFOQueue retransmission_q;
    int attempts;
    int rt;
    int ul_loss_rate;
    int dl_loss_rate;
    Fragment sender_abort;
} SCHCSender;

/*
 * Function:  sender_construct
 * --------------------
 * Initializes the parameters of the SCHCSender structure.
 * Returns -1 on errors.
 *
 *  s: SCHCSender structure to be initialized.
 */
int
sender_construct(SCHCSender *s, Rule *rule, char schc_packet[], int byte_size);

/*
 * Function:  sender_destroy
 * --------------------
 * Frees the memory allocated for the SCHCSender structure.
 *
 *  s: SCHCSender structure to be destroyed.
 */
void sender_destroy(SCHCSender *s);

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
 * Function:  get_bitmap_to_retransmit
 * --------------------
 * Obtains the bitmap used to identify lost fragments.
 *
 * Returns the size of said bitmap, or -1 if no fragment is reported missing
 * in the bitmap.
 *
 *  s: SCHCSender structure that controls the communication.
 *  rule: Rule used to process the communication.
 *  ack_window: Number of the window the bitmap corresponds to.
 *  bitmap: Bitmap being processed.
 *  dest: Where to store the resulting bitmap.
 */
int get_bitmap_to_retransmit(SCHCSender *s, Rule *rule, int ack_window,
                             char bitmap[], char dest[]);

/*
 * Function:  update_queues
 * --------------------
 * Updates the SCHCSender queues using information of the current state of
 * the SCHC transmission, provided by the SCHC Fragment just sent and the
 * ACK received.
 * Returns -1 on errors.
 *
 *  s: SCHCSender structure that controls the communication.
 *  frg: Fragment just sent.
 *  ack: CompoundACK just received.
 */
int
update_queues(SCHCSender *s, Rule *rule, Fragment *frg, CompoundACK *ack);

/*
 * Function:  timeout_procedure
 * --------------------
 * Updates internal variables of SCHCSender assuming that a schc_send() or
 * schc_recv() timeout occurred.
 * Returns -1 on network errors.
 *
 *  s: SCHCSender structure that controls the communication.
 *  rule: Rule used to process the communication.
 *  frg: Fragment just sent.
 */
int timeout_procedure(SCHCSender *s, Rule *rule, Fragment *frg);

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
 *  dest: CompoundACK structure where to store the informacion.
 */
ssize_t schc_recv(SCHCSender *s, CompoundACK *dest);

/*
 * Function:  schc
 * --------------------
 * Performs the SCHC Sender behavior to send a single SCHC Fragment and update
 * internal variables.
 * Returns -1 on network errors.
 * Returns SCHC_SENDER_ABORTED on sending a Sender-Abort.
 * Returns SCHC_RECEIVER_ABORTED on receiving a Receiver-Abort.
 * Returns SCHC_COMPLETED on receving a complete ACK.
 *
 *  s: SCHCSender structure that controls the communication.
 *  rule: Rule used to process the communication.
 *  frg: Fragment to be sent.
 */
int schc(SCHCSender *s, Rule *rule, Fragment *frg);

/*
 * Function:  sender_start
 * --------------------
 * Performs the full SCHC Sender procedure (fragmentation and delivery)
 * for a SCHC Packet.
 *
 *  s: SCHCSender structure that controls the communication.
 *  rule: Rule to be used to process the communication.
 *  schc_packet: SCHC Packet to be fragmented and sent.
 */
ssize_t sender_start(SCHCSender *s, Rule *rule);