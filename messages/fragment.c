//
// Created by witu on 2/20/23.
//

#include "fragment.h"
#include "casting.h"
#include "misc.h"

void fragment_to_bin(Fragment *fragment, char dest[]) {
  memset(dest, '\0', UPLINK_MTU_BITS);
  char* message = fragment->message;
  bytes_to_bin(dest, message, strlen(message) * 8);
}

void init_rule_from_fragment(Rule *dest, Fragment *fragment) {
  char as_bin[UPLINK_MTU_BITS];
  fragment_to_bin(fragment, as_bin);
  init_rule(dest, as_bin);
}

void get_fragment_rule_id(Rule *rule, Fragment *fragment, char dest[]) {
  int rule_id_index = 0;
  int rule_id_size = rule->rule_id_size;
  char fragment_as_bin[UPLINK_MTU_BITS];
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + rule_id_index, rule_id_size);
  dest[rule_id_size] = '\0';
}

void get_fragment_dtag(Rule *rule, Fragment *fragment, char dest[]) {
  int dtag_index = rule->rule_id_size;
  int dtag_size = rule->t;
  char fragment_as_bin[UPLINK_MTU_BITS];
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + dtag_index, dtag_size);
  dest[dtag_size] = '\0';
}

void get_fragment_w(Rule *rule, Fragment *fragment, char dest[]) {
  int w_index = rule->rule_id_size + rule->t;
  int w_size = rule->m;
  char fragment_as_bin[UPLINK_MTU_BITS];
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + w_index, w_size);
  dest[w_size] = '\0';
}

void get_fragment_fcn(Rule *rule, Fragment *fragment, char dest[]) {
  int fcn_index = rule->rule_id_size + rule->t + rule->m;
  int fcn_size = rule->n;
  char fragment_as_bin[UPLINK_MTU_BITS];
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + fcn_index, fcn_size);
  dest[fcn_size] = '\0';
}

int is_fragment_all_0(Rule *rule, Fragment *fragment) {
  char fcn[rule->n];
  get_fragment_fcn(rule, fragment, fcn);
  return is_monochar(fcn, '0');
}

int is_fragment_all_1(Rule *rule, Fragment *fragment) {
  char fcn[rule->n];
  get_fragment_fcn(rule, fragment, fcn);

  if (!is_monochar(fcn, '1')) return 0;

  int all_1_header_length = rule->all1_header_length;
  int payload_size = UPLINK_MTU_BITS - all_1_header_length;
  char fragment_as_bin[UPLINK_MTU_BITS];
  char payload[payload_size];

  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(payload, fragment_as_bin + all_1_header_length, payload_size);

  if (payload[0] != '\0') return 1;

  if (strlen(fragment_as_bin) == all_1_header_length) return 1;

  return 0;
}

void get_fragment_rcs(Rule *rule, Fragment *fragment, char dest[]) {
  if (!is_fragment_all_1(rule, fragment)) return;

  int rcs_index = rule->rule_id_size + rule->t + rule->m + rule->n;
  int rcs_size = rule->u;
  char fragment_as_bin[UPLINK_MTU_BITS];
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + rcs_index, rcs_size);
  dest[rcs_size] = '\0';
}

int get_fragment_header_size(Rule *rule, Fragment *fragment) {
  return is_fragment_all_1(rule, fragment)
             ? rule->all1_header_length
             : rule->header_length;
}

void get_fragment_header(Rule *rule, Fragment *fragment, char dest[]) {
  int header_size = get_fragment_header_size(rule, fragment);
  char fragment_as_bin[UPLINK_MTU_BITS];
  fragment_to_bin(fragment, fragment_as_bin);
  memset(dest, '\0', header_size);
  strncpy(dest, fragment_as_bin, header_size);
  dest[header_size] = '\0';
}

void get_fragment_payload(Rule *rule, Fragment *fragment, char dest[]) {
  int header_size = get_fragment_header_size(rule, fragment);
  int payload_size = UPLINK_MTU_BITS - header_size;
  char fragment_as_bin[UPLINK_MTU_BITS];

  memset(dest, '\0', payload_size);
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + header_size, payload_size);
  dest[payload_size] = '\0';
}

int fragment_expects_ack(Rule *rule, Fragment *fragment) {
  return is_fragment_all_0(rule, fragment) || is_fragment_all_1(rule, fragment);
}

int is_fragment_sender_abort(Rule *rule, Fragment *fragment) {
  char w[rule->m], fcn[rule->n];
  get_fragment_w(rule, fragment, w);
  get_fragment_fcn(rule, fragment, fcn);

  if (!is_monochar(w, '1') || !is_monochar(fcn, '1')) return 0;

  char fragment_as_bin[UPLINK_MTU_BITS];
  fragment_to_bin(fragment, fragment_as_bin);

  return strlen(fragment_as_bin) < rule->all1_header_length;
}