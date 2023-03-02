#include "fragment.h"
#include "casting.h"
#include "misc.h"

void fragment_to_bin(Fragment *fragment,
                     char dest[fragment->byte_size * 8 + 1]) {
  memset(dest, '\0', UPLINK_MTU_BITS + 1);
  bytes_to_bin(dest, fragment->message, fragment->byte_size);
}

void init_rule_from_fragment(Rule *dest, Fragment *fragment) {
  char as_bin[UPLINK_MTU_BITS + 1];
  fragment_to_bin(fragment, as_bin);
  init_rule(dest, as_bin);
}

void get_fragment_rule_id(Rule *rule, Fragment *fragment,
                          char dest[rule->rule_id_size + 1]) {
  int rule_id_index = 0;
  int rule_id_size = rule->rule_id_size;
  char fragment_as_bin[UPLINK_MTU_BITS + 1];
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + rule_id_index, rule_id_size);
  dest[rule_id_size] = '\0';
}

void get_fragment_dtag(Rule *rule, Fragment *fragment, char dest[rule->t + 1]) {
  int dtag_index = rule->rule_id_size;
  int dtag_size = rule->t;
  char fragment_as_bin[UPLINK_MTU_BITS + 1];
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + dtag_index, dtag_size);
  dest[dtag_size] = '\0';
}

void get_fragment_w(Rule *rule, Fragment *fragment, char dest[rule->m + 1]) {
  int w_index = rule->rule_id_size + rule->t;
  int w_size = rule->m;
  char fragment_as_bin[UPLINK_MTU_BITS + 1];
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + w_index, w_size);
  dest[w_size] = '\0';
}

void get_fragment_fcn(Rule *rule, Fragment *fragment, char dest[rule->n]) {
  int fcn_index = rule->rule_id_size + rule->t + rule->m;
  int fcn_size = rule->n;
  char fragment_as_bin[UPLINK_MTU_BITS + 1];
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
  char fragment_as_bin[UPLINK_MTU_BITS + 1];
  char payload[payload_size];

  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(payload, fragment_as_bin + all_1_header_length, payload_size);

  if (payload[0] != '\0')
    return 1;

  if (strlen(fragment_as_bin) == all_1_header_length)
    return 1;

  return 0;
}

int fragment_expects_ack(Rule *rule, Fragment *fragment) {
  return is_fragment_all_0(rule, fragment) || is_fragment_all_1(rule, fragment);
}

int is_fragment_sender_abort(Rule *rule, Fragment *fragment) {
  char w[rule->m], fcn[rule->n];
  get_fragment_w(rule, fragment, w);
  get_fragment_fcn(rule, fragment, fcn);

  if (!is_monochar(w, '1') || !is_monochar(fcn, '1'))
    return 0;

  char fragment_as_bin[UPLINK_MTU_BITS + 1];
  fragment_to_bin(fragment, fragment_as_bin);

  return strlen(fragment_as_bin) < rule->all1_header_length;
}

void get_fragment_rcs(Rule *rule, Fragment *fragment, char dest[rule->u + 1]) {
  if (!is_fragment_all_1(rule, fragment)) {
    memset(dest, '\0', rule->u + 1);
    return;
  }

  int rcs_index = rule->rule_id_size + rule->t + rule->m + rule->n;
  int rcs_size = rule->u;
  char fragment_as_bin[UPLINK_MTU_BITS + 1];
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + rcs_index, rcs_size);
  dest[rcs_size] = '\0';
}

int get_fragment_header_size(Rule *rule, Fragment *fragment) {
  return is_fragment_all_1(rule, fragment)
             ? rule->all1_header_length
             : rule->header_length;
}

int get_fragment_max_payload_size(Rule *rule, Fragment *fragment) {
  return UPLINK_MTU_BITS - get_fragment_header_size(rule, fragment);
}

void get_fragment_header(Rule *rule, Fragment *fragment, char dest[]) {
  int header_size = get_fragment_header_size(rule, fragment);
  char fragment_as_bin[UPLINK_MTU_BITS + 1];
  fragment_to_bin(fragment, fragment_as_bin);
  memset(dest, '\0', header_size);
  strncpy(dest, fragment_as_bin, header_size);
  dest[header_size] = '\0';
}

void get_fragment_payload(Rule *rule, Fragment *fragment, char dest[]) {
  int header_size = get_fragment_header_size(rule, fragment);
  int payload_size = UPLINK_MTU_BITS - header_size;
  char fragment_as_bin[UPLINK_MTU_BITS + 1];

  memset(dest, '\0', payload_size);
  fragment_to_bin(fragment, fragment_as_bin);
  strncpy(dest, fragment_as_bin + header_size, payload_size);
  dest[payload_size] = '\0';
}

void generate_sender_abort(Rule *rule, Fragment *src, Fragment *dest) {
  char rule_id[rule->rule_id_size + 1];
  char dtag[rule->t + 1];
  char w[rule->m + 1];
  char fcn[rule->n + 1];

  get_fragment_rule_id(rule, src, rule_id);
  get_fragment_dtag(rule, src, dtag);
  memset(w, '1', rule->m);
  memset(fcn, '1', rule->n);

  int dtag_idx = rule->rule_id_size;
  int w_idx = dtag_idx + rule->t;
  int fcn_idx = w_idx + rule->m;
  int sa_size = round_to_next_multiple(fcn_idx + rule->n, L2_WORD_SIZE);
  int byte_size = sa_size / 8;

  char sa_bin[sa_size + 1];
  memset(sa_bin, '0', sa_size);
  sa_bin[sa_size] = '\0';

  strncpy(sa_bin, rule_id, rule->rule_id_size);
  strncpy(sa_bin + dtag_idx, dtag, rule->t);
  strncpy(sa_bin + w_idx, w, rule->m);
  strncpy(sa_bin + fcn_idx, fcn, rule->n);

  memset(dest, '\0', sizeof(Fragment));
  memset(dest->message, '0', byte_size);
  bin_to_bytes(dest->message, sa_bin, sa_size);
  dest->byte_size = byte_size;
}