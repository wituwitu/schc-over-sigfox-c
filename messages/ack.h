#include "fragment.h"
#include "schc.h"
#ifndef SCHC_OVER_SIGFOX_C_ACK_H
#define SCHC_OVER_SIGFOX_C_ACK_H

#endif // SCHC_OVER_SIGFOX_C_ACK_H

typedef struct {
    char message[DOWNLINK_MTU_BYTES];
} CompoundACK;

/*
 * Function:  ack_to_bin
 * --------------------
 * Stores the binary representation of an ACK in a buffer.
 *
 *  ack: the CompoundACK to be processed.
 *  dest: buffer where to store the data.
 */
void ack_to_bin(CompoundACK *ack, char dest[DOWNLINK_MTU_BITS + 1]);

/*
 * Function:  init_rule_from_ack
 * --------------------
 * Initializes a Rule struct using information from an ACK.
 *
 *  dest: the Rule to be initialized.
 *  ack: the CompoundACK to be processed.
 */
void init_rule_from_ack(Rule *dest, CompoundACK *ack);

/*
 * Function:  get_ack_rule_id
 * --------------------
 * Obtains the binary representation of the Rule ID from an ACK.
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 *  dest: buffer to store the result.
 */
void get_ack_rule_id(Rule *rule, CompoundACK *ack,
                     char dest[rule->rule_id_size + 1]);

/*
 * Function:  get_ack_dtag
 * --------------------
 * Obtains the binary representation of the DTag from an ACK.
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 *  dest: buffer to store the result.
 */
void get_ack_dtag(Rule *rule, CompoundACK *ack, char dest[rule->t + 1]);

/*
 * Function:  get_ack_w
 * --------------------
 * Obtains the binary representation of the W field from an ACK.
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 *  dest: buffer to store the result.
 */
void get_ack_w(Rule *rule, CompoundACK *ack, char dest[rule->m + 1]);

/*
 * Function:  get_ack_c
 * --------------------
 * Obtains the binary representation of the C field from an ACK.
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 *  dest: buffer to store the result.
 */
void get_ack_c(Rule *rule, CompoundACK *ack, char dest[2]);

/*
 * Function:  get_ack_bitmap
 * --------------------
 * Obtains the first bitmap present in the ACK.
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 *  dest: buffer to store the result.
 */
void get_ack_bitmap(Rule *rule, CompoundACK *ack,
                    char dest[rule->window_size + 1]);

/*
 * Function:  get_ack_nb_tuples
 * --------------------
 * Obtains the number of (window, bitmap) tuples present in the ACK.
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 */
int get_ack_nb_tuples(Rule *rule, CompoundACK *ack);

/*
 * Function:  get_ack_tuples
 * --------------------
 * Obtains all of the (window, bitmap) tuples present in the ACK.
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 *  nb_tuples: number of tuples to be retrieved.
 *  windows: string array where to store window identifiers.
 *  bitmaps: string array where to store bitmaps.
 */
void get_ack_tuples(Rule *rule, CompoundACK *ack, int nb_tuples,
                    char windows[nb_tuples][rule->m + 1],
                    char bitmaps[nb_tuples][rule->window_size + 1]);

/*
 * Function:  is_ack_receiver_abort
 * --------------------
 * Check whether an ACK is a Receiver-Abort.
 * or not.
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 */
int is_ack_receiver_abort(Rule *rule, CompoundACK *ack);

/*
 * Function:  is_ack_compound
 * --------------------
 * Check whether an ACK is a Compound ACK.
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 */
int is_ack_compound(Rule *rule, CompoundACK *ack);

/*
 * Function:  is_ack_complete
 * --------------------
 * Check whether an ACK is complete (reports completion of the transmission).
 *
 *  rule: the Rule struct used to process the ACK.
 *  ack: the CompoundACK struct to be processed.
 */
int is_ack_complete(Rule *rule, CompoundACK *ack);

/*
 * Function:  generate_receiver_abort
 * --------------------
 * Generates a Receiver-Abort using the information of a Rule and a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  src: the Fragment to be used as a reference.
 *  dest: Fragment where to store the Sender-Abort information.
 */
void generate_receiver_abort(Rule *rule, Fragment *src, CompoundACK *dest);