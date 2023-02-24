
#include "ack.h"
#include <assert.h>
#include <stdio.h>
int test_ack_to_bin() {
  CompoundACK ack = {"\x13\xc8\x00\x00\x00\x00\x00\x00"};
  char as_bin[UPLINK_MTU_BITS];
  ack_to_bin(&ack, as_bin);

  assert(strcmp(as_bin, "0001001111001000000000000000000000000000000000000000000000000000") == 0);
  assert(strlen(as_bin) == sizeof(ack.message) * 8);
  return 0;
}

int test_init_rule_from_ack() {
  CompoundACK ack = {"\x13\xc8\x00\x00\x00\x00\x00\x00"};
  Rule rule;
  char as_bin[UPLINK_MTU_BITS];
  ack_to_bin(&ack, as_bin);
  init_rule_from_ack(&rule, &ack);
  assert(rule.id == 0);
}

int test_get_ack_rule_id() {
  CompoundACK ack = {"\x13\xc8\x00\x00\x00\x00\x00\x00"};
  Rule rule;
  init_rule_from_ack(&rule, &ack);
  char rule_id[rule.rule_id_size + 1];
  get_ack_rule_id(&rule, &ack, rule_id);
  assert(strcmp(rule_id, "000") == 0);

  return 0;
}

int test_get_ack_dtag() {
  CompoundACK ack = {"\x13\xc8\x00\x00\x00\x00\x00\x00"};
  Rule rule;
  init_rule_from_ack(&rule, &ack);
  char dtag[rule.t + 1];
  get_ack_dtag(&rule, &ack, dtag);
  assert(strcmp(dtag, "") == 0);

  return 0;
}

int test_get_ack_w(){
  CompoundACK ack = {"\x13\xc8\x00\x00\x00\x00\x00\x00"};
  Rule rule;
  init_rule_from_ack(&rule, &ack);

  char w[rule.m + 1];
  get_ack_w(&rule, &ack, w);
  assert(strcmp(w, "10") == 0);

  return 0;
}

int test_get_ack_c() {
  CompoundACK ack = {"\x13\xc8\x00\x00\x00\x00\x00\x00"};
  Rule rule;
  init_rule_from_ack(&rule, &ack);

  char c[2];
  get_ack_c(&rule, &ack, c);
  assert(strcmp(c, "0") == 0);

  return 0;
}

int main() {
    printf("%d test_ack_to_bin\n", test_ack_to_bin());
    printf("%d test_init_rule_from_ack\n", test_init_rule_from_ack());
    printf("%d test_get_ack_rule_id\n", test_get_ack_rule_id());
    printf("%d test_get_ack_dtag\n", test_get_ack_dtag());
    printf("%d test_get_ack_w\n", test_get_ack_w());
    printf("%d test_get_ack_c\n", test_get_ack_c());

    return 0;
}