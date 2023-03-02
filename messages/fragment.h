#include "rule.h"
#include "schc.h"
#ifndef SCHC_OVER_SIGFOX_C_FRAGMENT_H
#define SCHC_OVER_SIGFOX_C_FRAGMENT_H

#endif // SCHC_OVER_SIGFOX_C_FRAGMENT_H

typedef struct {
  char message[UPLINK_MTU_BYTES];
  int byte_size;
} Fragment;

/*
 * Function:  ack_to_bin
 * --------------------
 * Stores the binary representation of a Fragment in a buffer.
 *
 *  fragment: the Fragment to be processed.
 *  dest: buffer where to store the data.
 */
void fragment_to_bin(Fragment *fragment,
                     char dest[fragment->byte_size * 8 + 1]);

/*
 * Function:  init_rule_from_fragment
 * --------------------
 * Initializes a Rule struct using information from a Fragment.
 *
 *  dest: the Rule to be initialized.
 *  fragment: the Fragment to be processed.
 */
void init_rule_from_fragment(Rule *dest, Fragment *fragment);

/*
 * Function:  get_fragment_rule_id
 * --------------------
 * Obtains the binary representation of the Rule ID from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_fragment_rule_id(Rule *rule, Fragment *fragment,
                          char dest[rule->rule_id_size + 1]);

/*
 * Function:  get_fragment_dtag
 * --------------------
 * Obtains the binary representation of the DTag from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_fragment_dtag(Rule *rule, Fragment *fragment, char dest[rule->t + 1]);

/*
 * Function:  get_fragment_w
 * --------------------
 * Obtains the binary representation of the W field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_fragment_w(Rule *rule, Fragment *fragment, char dest[rule->m + 1]);

/*
 * Function:  get_fragment_fcn
 * --------------------
 * Obtains the binary representation of the FCN field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_fragment_fcn(Rule *rule, Fragment *fragment, char dest[rule->n + 1]);

/*
 * Function:  is_fragment_all_0
 * --------------------
 * Check whether the FCN field of a Fragment is composed only of '0's.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int is_fragment_all_0(Rule *rule, Fragment *fragment);

/*
 * Function:  is_fragment_all_1
 * --------------------
 * Check whether the FCN field of a Fragment is composed only of '1's, while
 * still being a valid Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int is_fragment_all_1(Rule *rule, Fragment *fragment);

/*
 * Function:  fragment_expects_ack
 * --------------------
 * Checks whether the Fragment expects an ACK from the receiver.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int fragment_expects_ack(Rule *rule, Fragment *fragment);

/*
 * Function:  get_fragment_rcs
 * --------------------
 * Obtains the binary representation of the RCS field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_fragment_rcs(Rule *rule, Fragment *fragment, char dest[rule->u + 1]);

/*
 * Function:  get_fragment_header_size
 * --------------------
 * Obtains the header size of a Fragment, depending on whether the fragment is
 * an All-1.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int get_fragment_header_size(Rule *rule, Fragment *fragment);

/*
 * Function:  get_fragment_max_payload_size
 * --------------------
 * Obtains the maximum payload size of a Fragment, depending on whether the
 * fragment is an All-1.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int get_fragment_max_payload_size(Rule *rule, Fragment *fragment);

/*
 * Function:  get_fragment_rcs
 * --------------------
 * Obtains the binary representation of the header field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_fragment_header(Rule *rule, Fragment *fragment, char dest[]);

/*
 * Function:  get_fragment_payload
 * --------------------
 * Obtains the binary representation of the payload field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_fragment_payload(Rule *rule, Fragment *fragment, char dest[]);

/*
 * Function:  is_fragment_sender_abort
 * --------------------
 * Checks whether the Fragment is a Sender-Abort.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int is_fragment_sender_abort(Rule *rule, Fragment *fragment);

/*
 * Function:  generate_sender_abort
 * --------------------
 * Generates a Sender-Abort using the information of a Rule and a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  src: the Fragment to be used as a reference.
 *  dest: Fragment where to store the Sender-Abort information.
 */
void generate_sender_abort(Rule *rule, Fragment *src, Fragment *dest);