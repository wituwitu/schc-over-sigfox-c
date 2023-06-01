#include "fragment.h"
#include "schc.h"
#ifndef SCHC_OVER_SIGFOX_C_ACK_H
#define SCHC_OVER_SIGFOX_C_ACK_H

#endif // SCHC_OVER_SIGFOX_C_ACK_H

typedef struct {
    char message[DOWNLINK_MTU_BYTES];
    int byte_size;
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
 *  bitmap: string array where to store bitmap.
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
 * Function:  generate_ack
 * --------------------
 * Generates a Complete ACK using information from the SCHC state.
 * Returns -1 on failures.
 *
 *  rule: Rule used to process and create the ACK.
 *  dest: Where to store the resulting Compound ACK information.
 *  wdw: Window number of the fragment that requested the ACK.
 *  c: Whether the ACK is complete or not.
 *  bitmaps: Array of all bitmaps of the SCHC session.
 */
int generate_ack(Rule *rule, CompoundACK *dest,
                 int wdw, char c,
                 char bitmaps[rule->max_window_number][rule->window_size + 1]);

/*
 * Function:  generate_receiver_abort
 * --------------------
 * Generates a Receiver-Abort using the information of a Rule.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  dest: Compound ACK where to store the Receiver-Abort information.
 */
void generate_receiver_abort(Rule *rule, CompoundACK *dest);

/*
 * Function:  generate_null_ack
 * --------------------
 * Generates a null SCHC Compound ACK, used to decide whether a CompoundACK
 * object has valid data or not.
 *
 *  dest: CompoundACK structure where to store the null ACK.
 */
void generate_null_ack(CompoundACK *dest);

/*
 * Function:  is_ack_null
 * --------------------
 * Checks whether an ACK is null.
 *
 *  ack: CompoundACK structure to check.
 */
int is_ack_null(CompoundACK *ack);

/*
 * Function:  ack_equal
 * --------------------
 * Checks whether the message and the byte size of two ACKs are identical.
 *
 *  ack1: first ACK.
 *  ack2: second ACK.
 */
int ack_equal(CompoundACK *ack1, CompoundACK *ack2);