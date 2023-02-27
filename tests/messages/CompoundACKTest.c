
#include "ack.h"
#include "casting.h"
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

int test_is_ack_receiver_abort() {
  char ra_bin[64] = "0001111111111111000000000000000000000000000000000000000000000000";
  char ra_bytes[8];
  bin_to_bytes(ra_bytes, ra_bin, 64);
  CompoundACK receiver_abort;
  strncpy(receiver_abort.message, ra_bytes, 8);
  Rule rule;
  init_rule_from_ack(&rule, &receiver_abort);
  assert(is_ack_receiver_abort(&rule, &receiver_abort));

  char ack_bin[64] = "0001110000000000000000000000000000000000000000000000000000000000";
  char ack_bytes[8];
  bin_to_bytes(ack_bytes, ack_bin, 64);
  CompoundACK ack;
  strncpy(ack.message, ack_bytes, 8);
  assert(!is_ack_receiver_abort(&rule, &ack));

  return 0;
}

int test_is_ack_compound() {
  char cack_bin[64] = "0000001110001101110011111111001000000000000000000000000000000000";
  char cack_bytes[8];
  bin_to_bytes(cack_bytes, cack_bin, 64);
  CompoundACK cack;
  strncpy(cack.message, cack_bytes, 8);
  Rule rule;
  init_rule_from_ack(&rule, &cack);
  assert(is_ack_compound(&rule, &cack));

  char ra_bin[64] = "0001111111111111000000000000000000000000000000000000000000000000";
  char ra_bytes[8];
  bin_to_bytes(ra_bytes, ra_bin, 64);
  CompoundACK receiver_abort;
  strncpy(receiver_abort.message, ra_bytes, 8);
  assert(!is_ack_compound(&rule, &receiver_abort));
  assert(is_ack_receiver_abort(&rule, &receiver_abort));

  return 0;
}

int test_is_ack_complete() {
  char ack_bin[64] = "0001110000000000000000000000000000000000000000000000000000000000";
  char ack_bytes[8] = "";
  bin_to_bytes(ack_bytes, ack_bin, 64);
  CompoundACK ack;
  strncpy(ack.message, ack_bytes, 8);
  Rule rule;
  init_rule_from_ack(&rule, &ack);
  assert(is_ack_complete(&rule, &ack));

  return 0;
}

int test_get_ack_nb_tuples() {
  char ack_bin[64] = "0000001110001101110011111111001000000000000000000000000000000000";
  char ack_bytes[8] = "";
  bin_to_bytes(ack_bytes, ack_bin, 64);
  CompoundACK ack;
  strncpy(ack.message, ack_bytes, 8);
  Rule rule;
  init_rule_from_ack(&rule, &ack);
  assert(get_ack_nb_tuples(&rule, &ack) == 3);

  char one_bin[64] = "0001110000000000000000000000000000000000000000000000000000000000";
  char one_bytes[8] = "";
  bin_to_bytes(one_bytes, one_bin, 64);
  CompoundACK one;
  strncpy(one.message, one_bytes, 8);
  assert(get_ack_nb_tuples(&rule, &one) == 1);

  return 0;
}


int test_get_ack_tuples() {
  char ack_bin[64] = "0000001110001101110011111111001000000000000000000000000000000000";
  char ack_bytes[8] = "";
  bin_to_bytes(ack_bytes, ack_bin, 64);
  CompoundACK ack;
  strncpy(ack.message, ack_bytes, 8);
  Rule rule;
  init_rule_from_ack(&rule, &ack);

  int nb_tuples = get_ack_nb_tuples(&rule, &ack);
  char windows[nb_tuples][rule.m];
  char bitmaps[nb_tuples][rule.window_size];

  //get_ack_tuples(&rule, &ack, windows, bitmaps);

  char windows_expected[3][3] = {"00", "10", "11"};
  char bitmaps_expected[3][8] = {"1110001", "1110011", "1111001"};

  //assert(windows == windows_expected);
  //assert(bitmaps == bitmaps_expected);

  return 0;
}


int main() {
    printf("%d test_ack_to_bin\n", test_ack_to_bin());
    printf("%d test_init_rule_from_ack\n", test_init_rule_from_ack());
    printf("%d test_get_ack_rule_id\n", test_get_ack_rule_id());
    printf("%d test_get_ack_dtag\n", test_get_ack_dtag());
    printf("%d test_get_ack_w\n", test_get_ack_w());
    printf("%d test_get_ack_c\n", test_get_ack_c());
    printf("%d test_is_ack_receiver_abort\n", test_is_ack_receiver_abort());
    printf("%d test_is_ack_compound\n", test_is_ack_compound());
    printf("%d test_is_ack_complete\n", test_is_ack_complete());
    printf("%d test_get_ack_nb_tuples\n", test_get_ack_nb_tuples());
    // printf("%d test_get_ack_tuples\n", test_get_ack_tuples());

    return 0;
}