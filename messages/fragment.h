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
void frg_to_bin(Fragment *fragment,
                char dest[fragment->byte_size * 8 + 1]);

/*
 * Function:  init_rule_from_frg
 * --------------------
 * Initializes a Rule struct using information from a Fragment.
 *
 *  dest: the Rule to be initialized.
 *  fragment: the Fragment to be processed.
 */
void init_rule_from_frg(Rule *dest, Fragment *fragment);

/*
 * Function:  get_frg_rule_id
 * --------------------
 * Obtains the binary representation of the Rule ID from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_frg_rule_id(Rule *rule, Fragment *fragment,
                     char dest[rule->rule_id_size + 1]);

/*
 * Function:  get_frg_dtag
 * --------------------
 * Obtains the binary representation of the DTag from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_frg_dtag(Rule *rule, Fragment *fragment, char dest[rule->t + 1]);

/*
 * Function:  get_frg_w
 * --------------------
 * Obtains the binary representation of the W field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_frg_w(Rule *rule, Fragment *fragment, char dest[rule->m + 1]);

/*
 * Function:  get_frg_fcn
 * --------------------
 * Obtains the binary representation of the FCN field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_frg_fcn(Rule *rule, Fragment *fragment, char dest[rule->n + 1]);

/*
 * Function:  is_frg_all_0
 * --------------------
 * Check whether the FCN field of a Fragment is composed only of '0's.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int is_frg_all_0(Rule *rule, Fragment *fragment);

/*
 * Function:  is_frg_all_1
 * --------------------
 * Check whether the FCN field of a Fragment is composed only of '1's, while
 * still being a valid Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int is_frg_all_1(Rule *rule, Fragment *fragment);

/*
 * Function:  frg_expects_ack
 * --------------------
 * Checks whether the Fragment expects an ACK from the receiver.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int frg_expects_ack(Rule *rule, Fragment *fragment);

/*
 * Function:  get_frg_rcs
 * --------------------
 * Obtains the binary representation of the RCS field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_frg_rcs(Rule *rule, Fragment *fragment, char dest[rule->u + 1]);

/*
 * Function:  get_frg_header_byte_size
 * --------------------
 * Obtains the header size of a Fragment in bytes, depending on whether the
 * fragment is an All-1.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int get_frg_header_byte_size(Rule *rule, Fragment *fragment);

/*
 * Function:  get_frg_max_payload_byte_size
 * --------------------
 * Obtains the maximum payload size of a Fragment in bytes, depending on
 * whether the fragment is an All-1.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int get_frg_max_payload_byte_size(Rule *rule, Fragment *fragment);

/*
 * Function:  get_frg_payload_byte_size
 * --------------------
 * Obtains the payload size of a Fragment in bytes.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int get_frg_payload_byte_size(Rule *rule, Fragment *fragment);

/*
 * Function:  get_frg_header
 * --------------------
 * Obtains the bytes representation of the header field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_frg_header(Rule *rule, Fragment *fragment, char dest[]);

/*
 * Function:  get_frg_payload
 * --------------------
 * Obtains the bytes representation of the payload field from a Fragment.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 *  dest: buffer to store the result.
 */
void get_frg_payload(Rule *rule, Fragment *fragment, char dest[]);

/*
 * Function:  is_frg_sender_abort
 * --------------------
 * Checks whether the Fragment is a Sender-Abort.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  fragment: the Fragment to be processed.
 */
int is_frg_sender_abort(Rule *rule, Fragment *fragment);

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

/*
 * Function:  generate_frg
 * --------------------
 * Generates a SCHC Fragment and stores it in a Fragment struct. Returns -1 on
 * errors.
 *
 *  rule: the Rule struct used to create the Fragment.
 *  dest: struct where to store the Fragment.
 *  payload: byte array that contains the fragment payload.
 *  payload_byte_length: the length in bytes of the payload.
 *  nb_frag: the global index of the Fragment.
 *  all_1: whether the Fragment to be generated is an All-1.
 */
int generate_frg(Rule *rule, Fragment *dest, const char payload[],
                 int payload_byte_length, int nb_frag, int all_1);

/*
 * Function:  generate_null_frg
 * --------------------
 * Generates a null SCHC Fragment, used to identify the end of a SCHC Fragment
 * array at the receiver end.
 *
 *  rule: the Rule struct used to create the Fragment.
 *  dest: Fragment structure where to store the null fragment.
 */
void generate_null_frg(Fragment *dest);

/*
 * Function:  is_frg_null
 * --------------------
 * Checks whether a SCHC Fragment is a null Fragment.
 *
 *  rule: the Rule struct used to create the Fragment.
 *  frg: the Fragment to check.
 */
int is_frg_null(Fragment *frg);

/*
 * Function:  get_frg_idx
 * --------------------
 * Obtains the index of a fragment in an array of Fragments ordered by
 * window and FCN, usually the result of the fragmentation process.
 *
 *  rule: the Rule struct used to process the Fragment.
 *  frg: the Fragment to check.
 */
int get_frg_idx(Rule *rule, Fragment *frg);

/*
 * Function:  frg_equal
 * --------------------
 * Checks whether the message and the byte size of two fragments are identical.
 *
 *  frg1: first fragment.
 *  frg2: second fragment.
 */
int frg_equal(Fragment *frg1, Fragment *frg2);