#include "fragment.h"
#include "casting.h"
#include "misc.h"
#include <assert.h>
#include <stdio.h>

int test_fragment_to_bin() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    char as_bin[UPLINK_MTU_BITS + 1];
    fragment_to_bin(&fragment, as_bin);

    assert(strcmp(as_bin, "00010101100010001000100010001000") == 0);
    assert(strlen(as_bin) == strlen(fragment.message) * 8);
    return 0;
}

int test_init_rule_from_fragment() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    char as_bin[UPLINK_MTU_BITS + 1];
    fragment_to_bin(&fragment, as_bin);
    init_rule_from_fragment(&rule, &fragment);
    assert(rule.id == 0);

    return 0;
}

int test_get_fragment_rule_id() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);

    char rule_id[rule.rule_id_size + 1];
    get_fragment_rule_id(&rule, &fragment, rule_id);
    assert(strcmp(rule_id, "000") == 0);

    return 0;
}

int test_get_fragment_dtag() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);

    char dtag[rule.t + 1];
    get_fragment_dtag(&rule, &fragment, dtag);
    assert(strcmp(dtag, "") == 0);

    return 0;
}

int test_get_fragment_w() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);

    char w[rule.m + 1];
    get_fragment_w(&rule, &fragment, w);
    assert(strcmp(w, "10") == 0);

    return 0;
}

int test_get_fragment_fcn() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);

    char fcn[rule.n + 1];
    get_fragment_fcn(&rule, &fragment, fcn);
    assert(strcmp(fcn, "101") == 0);

    return 0;
}

int test_is_fragment_all_0() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);
    assert(!is_fragment_all_0(&rule, &fragment));

    char all_0_as_bytes[5];
    bin_to_bytes(all_0_as_bytes, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, all_0_as_bytes, 4);
    all_0.byte_size = 4;
    assert(is_fragment_all_0(&rule, &all_0));

    char all_1_as_bytes[5];
    bin_to_bytes(all_1_as_bytes, "00010111100000000100010001000100", 32);
    Fragment all_1;
    strncpy(all_1.message, all_1_as_bytes, 4);
    all_1.byte_size = 4;
    assert(!is_fragment_all_0(&rule, &all_1));

    return 0;
}

int test_is_fragment_all_1() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);
    assert(!is_fragment_all_1(&rule, &fragment));

    char all_0_as_bytes[5];
    bin_to_bytes(all_0_as_bytes, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, all_0_as_bytes, 4);
    all_0.byte_size = 4;
    assert(!is_fragment_all_1(&rule, &all_0));

    char all_1_as_bytes[5];
    bin_to_bytes(all_1_as_bytes, "00010111100000000100010001000100", 32);
    Fragment all_1;
    strncpy(all_1.message, all_1_as_bytes, 4);
    all_1.byte_size = 4;
    assert(is_fragment_all_1(&rule, &all_1));

    char only_header_as_bytes[5];
    bin_to_bytes(only_header_as_bytes, "0001011110000000", 16);
    Fragment only_header;
    strncpy(only_header.message, only_header_as_bytes, 2);
    only_header.byte_size = 2;
    assert(is_fragment_all_1(&rule, &only_header));

    char only_header_as_bytes_invalid[5];
    bin_to_bytes(only_header_as_bytes_invalid, "0001010110000000", 16);
    Fragment only_header_invalid;
    strncpy(only_header_invalid.message, only_header_as_bytes_invalid, 2);
    only_header_invalid.byte_size = 2;
    assert(!is_fragment_all_1(&rule, &only_header_invalid));

    return 0;
}

int test_get_fragment_rcs() {
    char message[5];
    bin_to_bytes(message, "00010111100000000100010001000100", 32);
    Fragment fragment;
    strncpy(fragment.message, message, 4);
    fragment.byte_size = 4;
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);
    assert(is_fragment_all_1(&rule, &fragment));
    char rcs[rule.u + 1];
    get_fragment_rcs(&rule, &fragment, rcs);
    assert(strcmp(rcs, "100") == 0);

    char message_all0[5];
    bin_to_bytes(message_all0, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, message_all0, 4);
    all_0.byte_size = 4;
    char rcs_second[rule.u + 1];
    memset(rcs_second, '\0', rule.u + 1);
    get_fragment_rcs(&rule, &all_0, rcs_second);
    assert(rcs_second[0] == '\0');
    assert(strcmp(rcs_second, "") == 0);

    return 0;
}

int test_get_fragment_header_size() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);
    assert(get_fragment_header_size(&rule, &fragment) == 8);

    char all_1_as_bytes[5];
    bin_to_bytes(all_1_as_bytes, "00010111100000000100010001000100", 32);
    Fragment all_1;
    strncpy(all_1.message, all_1_as_bytes, 4);
    all_1.byte_size = 4;
    assert(get_fragment_header_size(&rule, &all_1) == 16);
    return 0;
}

int test_get_fragment_max_payload_size() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);
    assert(get_fragment_max_payload_size(&rule, &fragment) == 88);

    char all_1_as_bytes[5];
    bin_to_bytes(all_1_as_bytes, "00010111100000000100010001000100", 32);
    Fragment all_1;
    strncpy(all_1.message, all_1_as_bytes, 4);
    all_1.byte_size = 4;
    assert(get_fragment_max_payload_size(&rule, &all_1) == 80);
    return 0;
}

int test_get_fragment_header() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);
    int header_size = get_fragment_header_size(&rule, &fragment);
    char header[header_size + 1];
    get_fragment_header(&rule, &fragment, header);
    assert(strcmp(header, "00010101") == 0);

    Fragment all_1 = {"\027\200DD", 4};
    int all_1_header_size = get_fragment_header_size(&rule, &all_1);
    char all_1_header[all_1_header_size + 1];
    get_fragment_header(&rule, &all_1, all_1_header);
    assert(strcmp(all_1_header, "0001011110000000") == 0);

    return 0;
}

int test_get_fragment_payload() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);
    int payload_size = get_fragment_max_payload_size(&rule, &fragment);
    char payload[payload_size + 1];
    get_fragment_payload(&rule, &fragment, payload);
    assert(strcmp(payload, "100010001000100010001000") == 0);

    Fragment all_1 = {"\027\200DD", 4};
    int all_1_payload_size = get_fragment_max_payload_size(&rule, &all_1);
    char all_1_payload[all_1_payload_size + 1];
    get_fragment_payload(&rule, &all_1, all_1_payload);
    assert(strcmp(all_1_payload, "0100010001000100") == 0);

    Fragment all_1_no_payload = {"\027\200", 2};
    char all_1_no_payload_payload[all_1_payload_size + 1];
    get_fragment_payload(&rule, &all_1_no_payload, all_1_no_payload_payload);
    assert(strcmp(all_1_no_payload_payload, "") == 0);

    return 0;
}

int test_fragment_expects_ack() {
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);
    assert(!fragment_expects_ack(&rule, &fragment));

    char all_0_as_bytes[5];
    bin_to_bytes(all_0_as_bytes, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, all_0_as_bytes, 4);
    all_0.byte_size = 4;
    assert(fragment_expects_ack(&rule, &all_0));

    Fragment all_1 = {"\027\200DD", 4};
    assert(fragment_expects_ack(&rule, &all_1));

    Fragment all_1_no_payload = {"\027\200", 2};
    assert(fragment_expects_ack(&rule, &all_1_no_payload));

    return 0;
}

int test_is_fragment_sender_abort() {
    char sender_abort_as_bytes[2];
    bin_to_bytes(sender_abort_as_bytes, "00011111", 8);
    Fragment sender_abort;
    strncpy(sender_abort.message, sender_abort_as_bytes, 1);
    sender_abort.byte_size = 1;
    Rule rule;
    init_rule_from_fragment(&rule, &sender_abort);
    assert(is_fragment_sender_abort(&rule, &sender_abort));

    Fragment fragment = {"\x15\x88\x88\x88"};
    assert(!is_fragment_sender_abort(&rule, &fragment));

    char all_0_as_bytes[5];
    bin_to_bytes(all_0_as_bytes, "00010000100000000100010001000100", 32);
    Fragment all_0;
    strncpy(all_0.message, all_0_as_bytes, 4);
    all_0.byte_size = 4;
    assert(!is_fragment_sender_abort(&rule, &all_0));

    Fragment all_1 = {"\027\200DD", 4};
    assert(!is_fragment_sender_abort(&rule, &all_1));

    Fragment all_1_no_payload = {"\027\200", 2};
    assert(!is_fragment_sender_abort(&rule, &all_1_no_payload));

    return 0;
}

int test_generate_sender_abort() {

    // Normal fragment
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_fragment(&rule, &fragment);
    assert(!is_fragment_sender_abort(&rule, &fragment));

    // Sender abort
    Fragment sender_abort;
    generate_sender_abort(&rule, &fragment, &sender_abort);
    char w[rule.m + 1];
    char fcn[rule.n + 1];
    char rcs[rule.u + 1];
    get_fragment_w(&rule, &sender_abort, w);
    get_fragment_fcn(&rule, &sender_abort, fcn);
    get_fragment_rcs(&rule, &sender_abort, rcs);

    assert(is_fragment_sender_abort(&rule, &sender_abort));
    assert(is_monochar(w, '1') && strlen(w) == rule.m);
    assert(is_monochar(fcn, '1') && strlen(fcn) == rule.n);
    assert(strcmp(rcs, "") == 0);
    assert(!fragment_expects_ack(&rule, &sender_abort));

    return 0;
}

int test_generate_fragment() {

    // Regular fragment
    int nb_frag = 0;
    Fragment frg;
    Rule rule;
    init_rule(&rule, "000");
    char payload[] = "\xde\xad\xca\xfe";
    assert(generate_fragment(&rule, &frg, payload, 4, nb_frag, 0) == 0);

    char frg_rule_id[rule.rule_id_size + 1];
    char frg_dtag[rule.t + 1];
    char frg_w[rule.m + 1];
    char frg_fcn[rule.n + 1];
    char frg_rcs[rule.u + 1];

    get_fragment_rule_id(&rule, &frg, frg_rule_id);
    get_fragment_dtag(&rule, &frg, frg_dtag);
    get_fragment_w(&rule, &frg, frg_w);
    get_fragment_fcn(&rule, &frg, frg_fcn);
    get_fragment_rcs(&rule, &frg, frg_rcs);

    assert(strcmp(frg_rule_id, "000") == 0);
    assert(strcmp(frg_dtag, "") == 0);
    assert(strcmp(frg_w, "00") == 0);
    assert(strcmp(frg_fcn, "110") == 0);
    assert(strcmp(frg_rcs, "") == 0);

    // All-0
    int nb_frag_all0 = 6;
    Fragment all0;
    assert(generate_fragment(&rule, &all0, payload, 4, nb_frag_all0, 0) == 0);

    char all0_rule_id[rule.rule_id_size + 1];
    char all0_dtag[rule.t + 1];
    char all0_w[rule.m + 1];
    char all0_fcn[rule.n + 1];
    char all0_rcs[rule.u + 1];

    get_fragment_rule_id(&rule, &all0, all0_rule_id);
    get_fragment_dtag(&rule, &all0, all0_dtag);
    get_fragment_w(&rule, &all0, all0_w);
    get_fragment_fcn(&rule, &all0, all0_fcn);
    get_fragment_rcs(&rule, &all0, all0_rcs);

    assert(strcmp(all0_rule_id, "000") == 0);
    assert(strcmp(all0_dtag, "") == 0);
    assert(strcmp(all0_w, "00") == 0);
    assert(strcmp(all0_fcn, "000") == 0);
    assert(strcmp(all0_rcs, "") == 0);
    assert(is_fragment_all_0(&rule, &all0));

    // All-1
    int nb_frag_all1 = 8;
    Fragment all1;
    assert(generate_fragment(&rule, &all1, payload, 4, nb_frag_all1, 1) == 0);

    char all1_rule_id[rule.rule_id_size + 1];
    char all1_dtag[rule.t + 1];
    char all1_w[rule.m + 1];
    char all1_fcn[rule.n + 1];
    char all1_rcs[rule.u + 1];

    get_fragment_rule_id(&rule, &all1, all1_rule_id);
    get_fragment_dtag(&rule, &all1, all1_dtag);
    get_fragment_w(&rule, &all1, all1_w);
    get_fragment_fcn(&rule, &all1, all1_fcn);
    get_fragment_rcs(&rule, &all1, all1_rcs);

    assert(strcmp(all1_rule_id, "000") == 0);
    assert(strcmp(all1_dtag, "") == 0);
    assert(strcmp(all1_w, "01") == 0);
    assert(strcmp(all1_fcn, "111") == 0);
    assert(strcmp(all1_rcs, "010") == 0);
    assert(is_fragment_all_1(&rule, &all1));

    // All-1 with payload of length multiple of eleven
    char eleven[12];
    memset(eleven, '\x11', 11);
    eleven[11] = '\0';

    int nb_frag_invalid = 8;
    Fragment invalid;
    assert(generate_fragment(&rule, &invalid, eleven, 11, nb_frag_invalid, 1) ==
           -1);

    return 0;
}

int main() {
    printf("%d test_fragment_to_bin\n", test_fragment_to_bin());
    printf("%d test_init_rule_from_fragment\n", test_init_rule_from_fragment());
    printf("%d test_get_fragment_rule_id\n", test_get_fragment_rule_id());
    printf("%d test_get_fragment_dtag\n", test_get_fragment_dtag());
    printf("%d test_get_fragment_w\n", test_get_fragment_w());
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

    return 0;
}