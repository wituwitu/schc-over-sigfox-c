#include "fragment.h"
#include "casting.h"
#include "misc.h"
#include <assert.h>
#include <stdio.h>

int test_fragment_to_bin() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    char as_bin[UPLINK_MTU_BITS + 1];
    frg_to_bin(&fragment, as_bin);

    assert(strcmp(as_bin, "00010101100010001000100010001000") == 0);
    assert(strlen(as_bin) == strlen(fragment.message) * 8);
    return 0;
}

int test_init_rule_from_fragment() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    char as_bin[UPLINK_MTU_BITS + 1];
    frg_to_bin(&fragment, as_bin);
    init_rule_from_frg(&rule, &fragment);
    assert(rule.id == 0);

    return 0;
}

int test_get_fragment_rule_id() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);

    char rule_id[rule.rule_id_size + 1];
    get_frg_rule_id(&rule, &fragment, rule_id);
    assert(strcmp(rule_id, "000") == 0);

    return 0;
}

int test_get_fragment_dtag() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);

    char dtag[rule.t + 1];
    get_frg_dtag(&rule, &fragment, dtag);
    assert(strcmp(dtag, "") == 0);

    return 0;
}

int test_get_fragment_w() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);

    char w[rule.m + 1];
    get_frg_w(&rule, &fragment, w);
    assert(strcmp(w, "10") == 0);

    return 0;
}

int test_get_fragment_window() {
    Fragment frg1 = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &frg1);

    assert(get_frg_window(&rule, &frg1) == 2);

    return 0;
}

int test_get_fragment_fcn() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);

    char fcn[rule.n + 1];
    get_frg_fcn(&rule, &fragment, fcn);
    assert(strcmp(fcn, "101") == 0);

    return 0;
}

int test_is_fragment_all_0() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    assert(!is_frg_all_0(&rule, &fragment));

    char all_0_as_bytes[5];
    bin_to_bytes(all_0_as_bytes, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, all_0_as_bytes, 4);
    all_0.byte_size = 4;
    assert(is_frg_all_0(&rule, &all_0));

    char all_1_as_bytes[5];
    bin_to_bytes(all_1_as_bytes, "00010111100000000100010001000100", 32);
    Fragment all_1;
    strncpy(all_1.message, all_1_as_bytes, 4);
    all_1.byte_size = 4;
    assert(!is_frg_all_0(&rule, &all_1));

    return 0;
}

int test_is_fragment_all_1() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    assert(!is_frg_all_1(&rule, &fragment));

    char all_0_as_bytes[5];
    bin_to_bytes(all_0_as_bytes, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, all_0_as_bytes, 4);
    all_0.byte_size = 4;
    assert(!is_frg_all_1(&rule, &all_0));

    char all_1_as_bytes[5];
    bin_to_bytes(all_1_as_bytes, "00010111100000000100010001000100", 32);
    Fragment all_1;
    strncpy(all_1.message, all_1_as_bytes, 4);
    all_1.byte_size = 4;
    assert(is_frg_all_1(&rule, &all_1));

    char only_header_as_bytes[5];
    bin_to_bytes(only_header_as_bytes, "0001011110000000", 16);
    Fragment only_header;
    strncpy(only_header.message, only_header_as_bytes, 2);
    only_header.byte_size = 2;
    assert(is_frg_all_1(&rule, &only_header));

    char only_header_as_bytes_invalid[5];
    bin_to_bytes(only_header_as_bytes_invalid, "0001010110000000", 16);
    Fragment only_header_invalid;
    strncpy(only_header_invalid.message, only_header_as_bytes_invalid, 2);
    only_header_invalid.byte_size = 2;
    assert(!is_frg_all_1(&rule, &only_header_invalid));

    return 0;
}

int test_get_fragment_rcs() {
    char message[5];
    bin_to_bytes(message, "00010111100000000100010001000100", 32);
    Fragment fragment;
    strncpy(fragment.message, message, 4);
    fragment.byte_size = 4;
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    assert(is_frg_all_1(&rule, &fragment));
    char rcs[rule.u + 1];
    get_frg_rcs(&rule, &fragment, rcs);
    assert(strcmp(rcs, "100") == 0);

    char message_all0[5];
    bin_to_bytes(message_all0, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, message_all0, 4);
    all_0.byte_size = 4;
    char rcs_second[rule.u + 1];
    memset(rcs_second, '\0', rule.u + 1);
    get_frg_rcs(&rule, &all_0, rcs_second);
    assert(rcs_second[0] == '\0');
    assert(strcmp(rcs_second, "") == 0);

    return 0;
}

int test_get_fragment_header_size() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    assert(get_frg_header_byte_size(&rule, &fragment) == 1);

    char all_1_as_bytes[5];
    bin_to_bytes(all_1_as_bytes, "00010111100000000100010001000100", 32);
    Fragment all_1;
    strncpy(all_1.message, all_1_as_bytes, 4);
    all_1.byte_size = 4;
    assert(get_frg_header_byte_size(&rule, &all_1) == 2);
    return 0;
}

int test_get_fragment_max_payload_size() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    assert(get_frg_max_payload_byte_size(&rule, &fragment) == 11);

    char all_1_as_bytes[5];
    bin_to_bytes(all_1_as_bytes, "00010111100000000100010001000100", 32);
    Fragment all_1;
    strncpy(all_1.message, all_1_as_bytes, 4);
    all_1.byte_size = 4;
    assert(get_frg_max_payload_byte_size(&rule, &all_1) == 10);
    return 0;
}

int test_get_fragment_header() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    int header_size = get_frg_header_byte_size(&rule, &fragment);
    char header[header_size + 1];
    get_frg_header(&rule, &fragment, header);
    assert(memcmp(header, "\x15", header_size) == 0);

    Fragment all_1 = {"\027\200DD", 4};
    int all_1_header_size = get_frg_header_byte_size(&rule, &all_1);
    char all_1_header[all_1_header_size + 1];
    get_frg_header(&rule, &all_1, all_1_header);
    assert(memcmp(all_1_header, "\027\200", all_1_header_size) == 0);

    return 0;
}

int test_get_fragment_payload() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    int payload_size = get_frg_payload_byte_size(&rule, &fragment);
    char payload[payload_size];
    get_frg_payload(&rule, &fragment, payload);
    assert(memcmp(payload, "\x88\x88\x88", payload_size) == 0);

    Fragment all_1 = {"\027\200DD", 4};
    int all_1_payload_size = get_frg_payload_byte_size(&rule, &all_1);
    char all_1_payload[all_1_payload_size + 1];
    get_frg_payload(&rule, &all_1, all_1_payload);
    assert(memcmp(all_1_payload, "DD", all_1_payload_size) == 0);

    Fragment all_1_no_payload = {"\027\200", 2};
    char all_1_no_payload_payload[all_1_payload_size + 1];
    int all_1_no_payload_payload_size = get_frg_payload_byte_size(&rule,
                                                                  &all_1_no_payload);
    get_frg_payload(&rule, &all_1_no_payload, all_1_no_payload_payload);
    assert(all_1_no_payload_payload_size == 0);
    assert(memcmp(all_1_no_payload_payload, "",
                  all_1_no_payload_payload_size) == 0);

    return 0;
}

int test_fragment_expects_ack() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    assert(!frg_expects_ack(&rule, &fragment));

    char all_0_as_bytes[5];
    bin_to_bytes(all_0_as_bytes, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, all_0_as_bytes, 4);
    all_0.byte_size = 4;
    assert(frg_expects_ack(&rule, &all_0));

    Fragment all_1 = {"\027\200DD", 4};
    assert(frg_expects_ack(&rule, &all_1));

    Fragment all_1_no_payload = {"\027\200", 2};
    assert(frg_expects_ack(&rule, &all_1_no_payload));

    return 0;
}

int test_is_fragment_sender_abort() {
    char sender_abort_as_bytes[2];
    bin_to_bytes(sender_abort_as_bytes, "00011111", 8);
    Fragment sender_abort;
    strncpy(sender_abort.message, sender_abort_as_bytes, 1);
    sender_abort.byte_size = 1;
    Rule rule;
    init_rule_from_frg(&rule, &sender_abort);
    assert(is_frg_sender_abort(&rule, &sender_abort));

    Fragment fragment = {"\x15\x88\x88\x88"};
    assert(!is_frg_sender_abort(&rule, &fragment));

    char all_0_as_bytes[5];
    bin_to_bytes(all_0_as_bytes, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, all_0_as_bytes, 4);
    all_0.byte_size = 4;
    assert(!is_frg_sender_abort(&rule, &all_0));

    Fragment all_1 = {"\027\200DD", 4};
    assert(!is_frg_sender_abort(&rule, &all_1));

    Fragment all_1_no_payload = {"\027\200", 2};
    assert(!is_frg_sender_abort(&rule, &all_1_no_payload));

    return 0;
}

int test_generate_sender_abort() {

    // Normal fragment
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    assert(!is_frg_sender_abort(&rule, &fragment));

    // Sender abort
    Fragment sender_abort;
    generate_sender_abort(&rule, &fragment, &sender_abort);
    char w[rule.m + 1];
    char fcn[rule.n + 1];
    char rcs[rule.u + 1];
    get_frg_w(&rule, &sender_abort, w);
    get_frg_fcn(&rule, &sender_abort, fcn);
    get_frg_rcs(&rule, &sender_abort, rcs);

    assert(is_frg_sender_abort(&rule, &sender_abort));
    assert(is_monochar(w, '1') && strlen(w) == rule.m);
    assert(is_monochar(fcn, '1') && strlen(fcn) == rule.n);
    assert(strcmp(rcs, "") == 0);
    assert(!frg_expects_ack(&rule, &sender_abort));

    return 0;
}

int test_generate_fragment() {

    // Regular fragment
    int nb_frag = 0;
    Fragment frg;
    Rule rule;
    init_rule(&rule, "000");
    char payload[] = "\xde\xad\xca\xfe";
    assert(generate_frg(&rule, &frg, payload, 4, nb_frag, 0) == 0);

    char frg_rule_id[rule.rule_id_size + 1];
    char frg_dtag[rule.t + 1];
    char frg_w[rule.m + 1];
    char frg_fcn[rule.n + 1];
    char frg_rcs[rule.u + 1];

    get_frg_rule_id(&rule, &frg, frg_rule_id);
    get_frg_dtag(&rule, &frg, frg_dtag);
    get_frg_w(&rule, &frg, frg_w);
    get_frg_fcn(&rule, &frg, frg_fcn);
    get_frg_rcs(&rule, &frg, frg_rcs);

    assert(strcmp(frg_rule_id, "000") == 0);
    assert(strcmp(frg_dtag, "") == 0);
    assert(strcmp(frg_w, "00") == 0);
    assert(strcmp(frg_fcn, "110") == 0);
    assert(strcmp(frg_rcs, "") == 0);

    // All-0
    int nb_frag_all0 = 6;
    Fragment all0;
    assert(generate_frg(&rule, &all0, payload, 4, nb_frag_all0, 0) == 0);

    char all0_rule_id[rule.rule_id_size + 1];
    char all0_dtag[rule.t + 1];
    char all0_w[rule.m + 1];
    char all0_fcn[rule.n + 1];
    char all0_rcs[rule.u + 1];

    get_frg_rule_id(&rule, &all0, all0_rule_id);
    get_frg_dtag(&rule, &all0, all0_dtag);
    get_frg_w(&rule, &all0, all0_w);
    get_frg_fcn(&rule, &all0, all0_fcn);
    get_frg_rcs(&rule, &all0, all0_rcs);

    assert(strcmp(all0_rule_id, "000") == 0);
    assert(strcmp(all0_dtag, "") == 0);
    assert(strcmp(all0_w, "00") == 0);
    assert(strcmp(all0_fcn, "000") == 0);
    assert(strcmp(all0_rcs, "") == 0);
    assert(is_frg_all_0(&rule, &all0));

    // All-1
    int nb_frag_all1 = 8;
    Fragment all1;
    assert(generate_frg(&rule, &all1, payload, 4, nb_frag_all1, 1) == 0);

    char all1_rule_id[rule.rule_id_size + 1];
    char all1_dtag[rule.t + 1];
    char all1_w[rule.m + 1];
    char all1_fcn[rule.n + 1];
    char all1_rcs[rule.u + 1];

    get_frg_rule_id(&rule, &all1, all1_rule_id);
    get_frg_dtag(&rule, &all1, all1_dtag);
    get_frg_w(&rule, &all1, all1_w);
    get_frg_fcn(&rule, &all1, all1_fcn);
    get_frg_rcs(&rule, &all1, all1_rcs);

    assert(strcmp(all1_rule_id, "000") == 0);
    assert(strcmp(all1_dtag, "") == 0);
    assert(strcmp(all1_w, "01") == 0);
    assert(strcmp(all1_fcn, "111") == 0);
    assert(strcmp(all1_rcs, "010") == 0);
    assert(is_frg_all_1(&rule, &all1));

    // All-1 with payload of length multiple of eleven
    char eleven[12];
    memset(eleven, '\x11', 11);
    eleven[11] = '\0';

    int nb_frag_invalid = 8;
    Fragment invalid;
    assert(generate_frg(&rule, &invalid, eleven, 11, nb_frag_invalid, 1) ==
           -1);

    return 0;
}

int test_generate_null_fragment() {
    Fragment frg;
    Rule rule;
    init_rule(&rule, "000");
    generate_null_frg(&frg);

    char empty[UPLINK_MTU_BYTES];
    memset(empty, '\0', UPLINK_MTU_BYTES);
    assert(memcmp(frg.message, empty, UPLINK_MTU_BYTES) == 0);
    assert(frg.byte_size == -1);

    return 0;
}

int test_is_fragment_null() {
    // Null fragment
    Fragment frg;
    Rule rule;
    init_rule(&rule, "000");
    generate_null_frg(&frg);
    assert(is_frg_null(&frg));
    assert(!is_frg_all_0(&rule, &frg));
    assert(!is_frg_all_1(&rule, &frg));
    assert(!is_frg_sender_abort(&rule, &frg));

    // Normal fragment
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    assert(!is_frg_null(&fragment));

    // All-0 fragment of window 0 with 0-payload
    // (Header 000-00-000)
    // (Payload \x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00)
    Fragment all0 = {
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
            12
    };
    char rule_id[rule.rule_id_size + 1];
    char dtag[rule.t + 1];
    char w[rule.m + 1];
    char fcn[rule.n + 1];
    get_frg_rule_id(&rule, &all0, rule_id);
    get_frg_dtag(&rule, &all0, dtag);
    get_frg_w(&rule, &all0, w);
    get_frg_fcn(&rule, &all0, fcn);

    char empty[UPLINK_MTU_BYTES];
    memset(empty, '\0', UPLINK_MTU_BYTES);

    assert(strcmp(rule_id, "000") == 0);
    assert(strcmp(dtag, "") == 0);
    assert(strcmp(w, "00") == 0);
    assert(strcmp(fcn, "000") == 0);
    assert(memcmp(all0.message, empty, UPLINK_MTU_BYTES) == 0);
    assert(!is_frg_null(&all0));

    return 0;
}

int test_get_frg_idx() {

    Rule rule;
    init_rule(&rule, "000");

    // Normal fragment
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    assert(get_frg_nb(&rule, &fragment) == 15);

    // All-0 fragment
    Fragment all0 = {"\x00\x00\x00\x00", 4};
    assert(get_frg_nb(&rule, &all0) == 6);

    // All-1 fragment
    Fragment all1 = {"\027\200DD", 4};
    assert(get_frg_nb(&rule, &all1) == 17);

    return 0;
}

int test_frg_equal() {

    Fragment frg1 = {"\x15\x88\x88\x88", 4};
    Fragment frg2 = {"\x15\x88\x88\x88", 4};
    Fragment frg3 = {"\x32\x88\x88\x88", 4};
    Fragment frg4;
    generate_null_frg(&frg4);

    assert(frg_equal(&frg1, &frg1) == 1);
    assert(frg_equal(&frg2, &frg2) == 1);
    assert(frg_equal(&frg3, &frg3) == 1);
    assert(frg_equal(&frg4, &frg4) == 1);
    assert(frg_equal(&frg1, &frg2) == 1);
    assert(frg_equal(&frg1, &frg3) == 0);
    assert(frg_equal(&frg1, &frg4) == 0);

    return 0;
}

int main() {
    printf("%d test_fragment_to_bin\n", test_fragment_to_bin());
    printf("%d test_init_rule_from_fragment\n", test_init_rule_from_fragment());
    printf("%d test_get_fragment_rule_id\n", test_get_fragment_rule_id());
    printf("%d test_get_fragment_dtag\n", test_get_fragment_dtag());
    printf("%d test_get_fragment_w\n", test_get_fragment_w());
    printf("%d test_get_fragment_window\n", test_get_fragment_window());
    printf("%d test_get_fragment_fcn\n", test_get_fragment_fcn());
    printf("%d test_is_fragment_all_0\n", test_is_fragment_all_0());
    printf("%d test_is_fragment_all_1\n", test_is_fragment_all_1());
    printf("%d test_get_fragment_rcs\n", test_get_fragment_rcs());
    printf("%d test_get_fragment_header_size\n",
           test_get_fragment_header_size());
    printf("%d test_get_fragment_max_payload_size\n",
           test_get_fragment_max_payload_size());
    printf("%d test_get_fragment_header\n", test_get_fragment_header());
    printf("%d test_get_fragment_payload\n", test_get_fragment_payload());
    printf("%d test_fragment_expects_ack\n", test_fragment_expects_ack());
    printf("%d test_is_fragment_sender_abort\n",
           test_is_fragment_sender_abort());
    printf("%d test_generate_sender_abort\n", test_generate_sender_abort());
    printf("%d test_generate_fragment\n", test_generate_fragment());
    printf("%d test_generate_null_fragment\n", test_generate_null_fragment());
    printf("%d test_is_fragment_null\n", test_is_fragment_null());
    printf("%d test_get_frg_idx\n", test_get_frg_idx());
    printf("%d test_frg_equal\n", test_frg_equal());

    return 0;
}