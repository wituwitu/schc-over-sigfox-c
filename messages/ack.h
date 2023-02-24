//
// Created by witu on 2/21/23.
//

#include "rule.h"
#include "schc.h"
#ifndef SCHC_OVER_SIGFOX_C_ACK_H
#define SCHC_OVER_SIGFOX_C_ACK_H

#endif // SCHC_OVER_SIGFOX_C_ACK_H

typedef struct {
  char message[DOWNLINK_MTU_BYTES];
} CompoundACK;

void ack_to_bin(CompoundACK *ack, char dest[]);

void init_rule_from_ack(Rule *dest, CompoundACK *ack);

void get_ack_rule_id(Rule *rule, CompoundACK *ack, char dest[]);

void get_ack_dtag(Rule *rule, CompoundACK *ack, char dest[]);

void get_ack_w(Rule *rule, CompoundACK *ack, char dest[]);

void get_ack_c(Rule *rule, CompoundACK *ack, char dest[]);

void get_ack_bitmap(Rule *rule, CompoundACK *ack, char dest[]);

void get_ack_tuples(Rule *rule, CompoundACK *ack, char **windows, char **bitmaps);

int is_ack_compound(Rule *rule, CompoundACK *ack);

int is_ack_complete(Rule *rule, CompoundACK *ack);

int get_ack_header_size(Rule *rule, CompoundACK *ack);

int get_ack_payload_size(Rule *rule, CompoundACK *ack);

void get_ack_header(Rule *rule, CompoundACK *ack, char dest[]);

void get_ack_payload(Rule *rule, CompoundACK *ack, char dest[]);

int is_ack_receiver_abort(Rule *rule, CompoundACK *ack);