#include <assert.h>
#include <stdio.h>
#include "rule.h"

int test_init_rule() {
    Rule rule_single_byte;
    init_rule(&rule_single_byte, "010");
    assert(rule_single_byte.id == 2);
    assert(rule_single_byte.rule_id_size == 3);
    assert(rule_single_byte.t == 0);
    assert(rule_single_byte.m == 2);
    assert(rule_single_byte.n == 3);
    assert(rule_single_byte.window_size == 7);
    assert(rule_single_byte.u == 3);
    assert(rule_single_byte.header_len == 8);
    assert(rule_single_byte.all1_header_len == 16);
    assert(rule_single_byte.ack_header_len == 6);
    assert(rule_single_byte.max_window_nb == 4);
    assert(rule_single_byte.max_fragment_nb == 28);
    assert(rule_single_byte.regular_payload_len == 88);
    assert(rule_single_byte.max_all1_payload_len == 80);
    assert(rule_single_byte.max_schc_packet_byte_size == 300);
    assert(rule_single_byte.frg_indices.rule_id_idx == 0);
    assert(rule_single_byte.frg_indices.dtag_idx == 3);
    assert(rule_single_byte.frg_indices.w_idx == 3);
    assert(rule_single_byte.frg_indices.fcn_idx == 5);
    assert(rule_single_byte.frg_indices.rcs_idx == 8);
    assert(rule_single_byte.ack_indices.rule_id_idx == 0);
    assert(rule_single_byte.ack_indices.dtag_idx == 3);
    assert(rule_single_byte.ack_indices.w_idx == 3);
    assert(rule_single_byte.ack_indices.c_idx == 5);

    Rule rule_two_byte_op_1;
    init_rule(&rule_two_byte_op_1, "111010");
    assert(rule_two_byte_op_1.id == 8);
    assert(rule_two_byte_op_1.rule_id_size == 6);
    assert(rule_two_byte_op_1.t == 0);
    assert(rule_two_byte_op_1.m == 2);
    assert(rule_two_byte_op_1.n == 4);
    assert(rule_two_byte_op_1.window_size == 12);
    assert(rule_two_byte_op_1.u == 4);
    assert(rule_two_byte_op_1.header_len == 16);
    assert(rule_two_byte_op_1.all1_header_len == 16);
    assert(rule_two_byte_op_1.ack_header_len == 9);
    assert(rule_two_byte_op_1.max_window_nb == 4);
    assert(rule_two_byte_op_1.max_fragment_nb == 48);
    assert(rule_two_byte_op_1.regular_payload_len == 80);
    assert(rule_two_byte_op_1.max_all1_payload_len == 80);
    assert(rule_two_byte_op_1.max_schc_packet_byte_size == 480);
    assert(rule_two_byte_op_1.frg_indices.rule_id_idx == 0);
    assert(rule_two_byte_op_1.frg_indices.dtag_idx == 6);
    assert(rule_two_byte_op_1.frg_indices.w_idx == 6);
    assert(rule_two_byte_op_1.frg_indices.fcn_idx == 8);
    assert(rule_two_byte_op_1.frg_indices.rcs_idx == 12);
    assert(rule_two_byte_op_1.ack_indices.rule_id_idx == 0);
    assert(rule_two_byte_op_1.ack_indices.dtag_idx == 6);
    assert(rule_two_byte_op_1.ack_indices.w_idx == 6);
    assert(rule_two_byte_op_1.ack_indices.c_idx == 8);

    Rule rule_two_byte_op_2;
    init_rule(&rule_two_byte_op_2, "11111110");
    assert(rule_two_byte_op_2.id == 17);
    assert(rule_two_byte_op_2.rule_id_size == 8);
    assert(rule_two_byte_op_2.t == 0);
    assert(rule_two_byte_op_2.m == 3);
    assert(rule_two_byte_op_2.n == 5);
    assert(rule_two_byte_op_2.window_size == 31);
    assert(rule_two_byte_op_2.u == 5);
    assert(rule_two_byte_op_2.header_len == 16);
    assert(rule_two_byte_op_2.all1_header_len == 24);
    assert(rule_two_byte_op_2.ack_header_len == 12);
    assert(rule_two_byte_op_2.max_window_nb == 8);
    assert(rule_two_byte_op_2.max_fragment_nb == 248);
    assert(rule_two_byte_op_2.regular_payload_len == 80);
    assert(rule_two_byte_op_2.max_all1_payload_len == 72);
    assert(rule_two_byte_op_2.max_schc_packet_byte_size == 2400);
    assert(rule_two_byte_op_2.frg_indices.rule_id_idx == 0);
    assert(rule_two_byte_op_2.frg_indices.dtag_idx == 8);
    assert(rule_two_byte_op_2.frg_indices.w_idx == 8);
    assert(rule_two_byte_op_2.frg_indices.fcn_idx == 11);
    assert(rule_two_byte_op_2.frg_indices.rcs_idx == 16);
    assert(rule_two_byte_op_2.ack_indices.rule_id_idx == 0);
    assert(rule_two_byte_op_2.ack_indices.dtag_idx == 8);
    assert(rule_two_byte_op_2.ack_indices.w_idx == 8);
    assert(rule_two_byte_op_2.ack_indices.c_idx == 11);

    return 0;
}

int test_parse_rule_from_bytes() {
    char byt[] = "\x15\x88\x88\x88";
    Rule rule;
    parse_rule_from_bytes(&rule, byt);
    assert(rule.id == 0);

    return 0;
}

int test_get_rule_id_bin() {
    Rule rule_single_byte;
    init_rule(&rule_single_byte, "010");
    char rule_id_bin_1b[rule_single_byte.rule_id_size + 1];
    get_rule_id_bin(&rule_single_byte, rule_id_bin_1b);
    assert(rule_single_byte.id == 2);
    assert(strcmp(rule_id_bin_1b, "010") == 0);

    Rule rule_two_byte_op_1;
    init_rule(&rule_two_byte_op_1, "111010");
    char rule_id_bin_2bo1[rule_two_byte_op_1.rule_id_size + 1];
    get_rule_id_bin(&rule_two_byte_op_1, rule_id_bin_2bo1);
    assert(rule_two_byte_op_1.id == 8);
    assert(strcmp(rule_id_bin_2bo1, "111010") == 0);

    Rule rule_two_byte_op_2;
    init_rule(&rule_two_byte_op_2, "11111110");
    char rule_id_bin_2bo2[rule_two_byte_op_2.rule_id_size + 1];
    get_rule_id_bin(&rule_two_byte_op_2, rule_id_bin_2bo2);
    assert(rule_two_byte_op_2.id == 17);
    assert(strcmp(rule_id_bin_2bo2, "11111110") == 0);

    return 0;
}


int main() {
    printf("%d test_init_rule\n", test_init_rule());
    printf("%d test_parse_rule_from_bytes\n", test_parse_rule_from_bytes());
    printf("%d test_get_rule_id_bin \n", test_get_rule_id_bin());

    return 0;
}