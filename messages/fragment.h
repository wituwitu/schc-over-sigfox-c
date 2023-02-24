//
// Created by witu on 2/20/23.
//

#include "rule.h"
#include "schc.h"
#ifndef SCHC_OVER_SIGFOX_C_FRAGMENT_H
#define SCHC_OVER_SIGFOX_C_FRAGMENT_H

#endif // SCHC_OVER_SIGFOX_C_FRAGMENT_H

typedef struct {
  char message[UPLINK_MTU_BYTES];
  int byte_size;
} Fragment;

void fragment_to_bin(Fragment *fragment, char dest[]);

void init_rule_from_fragment(Rule *dest, Fragment *fragment);

void get_fragment_rule_id(Rule *rule, Fragment *fragment, char dest[]);

void get_fragment_dtag(Rule *rule, Fragment *fragment, char dest[]);

void get_fragment_w(Rule *rule, Fragment *fragment, char dest[]);

void get_fragment_fcn(Rule *rule, Fragment *fragment, char dest[]);

int is_fragment_all_0(Rule *rule, Fragment *fragment);

int is_fragment_all_1(Rule *rule, Fragment *fragment);

void get_fragment_rcs(Rule *rule, Fragment *fragment, char dest[]);

int get_fragment_header_size(Rule *rule, Fragment *fragment);

int get_fragment_max_payload_size(Rule *rule, Fragment *fragment);

void get_fragment_header(Rule *rule, Fragment *fragment, char dest[]);

void get_fragment_payload(Rule *rule, Fragment *fragment, char dest[]);

int fragment_expects_ack(Rule *rule, Fragment *fragment);

int is_fragment_sender_abort(Rule *rule, Fragment *fragment);