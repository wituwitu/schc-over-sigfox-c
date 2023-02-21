//
// Created by witu on 2/21/23.
//

#include "ack.h"
#include "casting.h"

void ack_to_bin(CompoundACK *ack, char dest[]) {
  memset(dest, '\0', DOWNLINK_MTU_BITS);
  char* message = ack->message;
  bytes_to_bin(dest, message, strlen(message) * 8);
}

void init_rule_from_ack(Rule *dest, CompoundACK *ack) {
  char as_bin[DOWNLINK_MTU_BITS];
  ack_to_bin(ack, as_bin);
  init_rule(dest, as_bin);
}

void get_ack_rule_id(Rule *rule, CompoundACK *ack, char dest[]) {
  int rule_id_index = 0;
  int rule_id_size = rule->rule_id_size;
  char ack_as_bin[DOWNLINK_MTU_BITS];
  ack_to_bin(ack, ack_as_bin);
  strncpy(dest, ack_as_bin + rule_id_index, rule_id_size);
  dest[rule_id_size] = '\0';
}

void get_ack_dtag(Rule *rule, CompoundACK *ack, char dest[]) {
  int dtag_index = rule->rule_id_size;
  int dtag_size = rule->t;
  char ack_as_bin[DOWNLINK_MTU_BITS];
  ack_to_bin(ack, ack_as_bin);
  strncpy(dest, ack_as_bin + dtag_index, dtag_size);
  dest[dtag_size] = '\0';
}

void get_ack_w(Rule *rule, CompoundACK *ack, char dest[]) {
  int w_index = rule->rule_id_size + rule->t;
  int w_size = rule->m;
  char ack_as_bin[UPLINK_MTU_BITS];
  ack_to_bin(ack, ack_as_bin);
  strncpy(dest, ack_as_bin + w_index, w_size);
  dest[w_size] = '\0';
}