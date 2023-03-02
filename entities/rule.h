#include <string.h>

#ifndef SCHC_OVER_SIGFOX_C_RULE_H
#define SCHC_OVER_SIGFOX_C_RULE_H

#endif //SCHC_OVER_SIGFOX_C_RULE_H

typedef struct {
    int id;
    int rule_id_size;
    int t;
    int m;
    int n;
    int u;
    int window_size;
    int header_length;
    int all1_header_length;
    int ack_header_length;
    int max_window_number;
    int max_fragment_number;
    struct {
      int rule_id_idx;
      int dtag_idx;
      int w_idx;
      int fcn_idx;
      int rcs_idx;
      int payload_idx;
    } frg_indices;
    struct {
      int rule_id_idx;
      int dtag_idx;
      int w_idx;
      int c_idx;
      int bitmap_idx;
      int tuple_idx;
    } ack_indices;
} Rule;

/*
 * Function:  init_rule
 * --------------------
 * Configures a Rule structure accorting to the SCHC/Sigfox rule identification
 * algorithm.
 *
 *  rule: pointer to the Rule struct.
 *  rule_id_binary: binary string of the Rule ID.
 */
void init_rule(Rule *rule, const char rule_id_binary[]);

/*
 * Function:  parse_rule_from_bytes
 * --------------------
 * Configures a Rule structure using a byte string (a char array) as input.
 *
 *  rule: pointer to the Rule struct.
 *  byte: char array to use in Rule identification.
 */
void parse_rule_from_bytes(Rule *rule, const char *byt);