#include "ack.h"
#include "casting.h"
#include "misc.h"
#include <assert.h>
#include <stdio.h>

void test_routine_ack(Rule rule,
                      CompoundACK ack,
                      char expected_rule_id[],
                      char expected_dtag[],
                      char expected_w[],
                      char expected_c[],
                      char expected_bitmap[],
                      int expected_nb_tuples,
                      char expected_windows[expected_nb_tuples][rule.m + 1],
                      char expected_bitmaps[expected_nb_tuples][
                      rule.window_size + 1],
                      int expected_compound,
                      int expected_complete
) {

    char ack_rule_id[rule.rule_id_size + 1];
    char ack_dtag[rule.t + 1];
    char ack_w[rule.m + 1];
    char ack_c[2];
    char ack_bitmap[rule.window_size + 1];

    get_ack_rule_id(&rule, &ack, ack_rule_id);
    get_ack_dtag(&rule, &ack, ack_dtag);
    get_ack_w(&rule, &ack, ack_w);
    get_ack_c(&rule, &ack, ack_c);
    get_ack_bitmap(&rule, &ack, ack_bitmap);

    assert(strcmp(ack_rule_id, expected_rule_id) == 0);
    assert(strcmp(ack_dtag, expected_dtag) == 0);
    assert(strcmp(ack_w, expected_w) == 0);
    assert(strcmp(ack_c, expected_c) == 0);
    assert(strcmp(ack_bitmap, expected_bitmap) == 0);

    int nb_tuples = get_ack_nb_tuples(&rule, &ack);
    assert(nb_tuples == expected_nb_tuples);

    char tp_windows[nb_tuples][rule.m + 1];
    char tp_bitmaps[nb_tuples][rule.window_size + 1];
    get_ack_tuples(&rule, &ack, nb_tuples, tp_windows, tp_bitmaps);

    for (int i = 0; i < nb_tuples; i++) {
        assert(strcmp(tp_windows[i], expected_windows[i]) == 0);
        assert(strcmp(tp_bitmaps[i], expected_bitmaps[i]) == 0);
    }

    assert(!is_ack_receiver_abort(&rule, &ack));
    assert(is_ack_compound(&rule, &ack) == expected_compound);
    assert(is_ack_complete(&rule, &ack) == expected_complete);
}

int test_ack_to_bin() {
    CompoundACK ack = {"\x13\xc8\x00\x00\x00\x00\x00\x00"};
    char as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(&ack, as_bin);

    assert(strcmp(as_bin,
                  "0001001111001000000000000000000000000000000000000000000000000000") ==
           0);
    assert(strlen(as_bin) == DOWNLINK_MTU_BYTES * 8);
    return 0;
}

int test_init_rule_from_ack() {
    CompoundACK ack = {"\x13\xc8\x00\x00\x00\x00\x00\x00"};
    Rule rule;
    char as_bin[UPLINK_MTU_BITS + 1];
    ack_to_bin(&ack, as_bin);
    init_rule_from_ack(&rule, &ack);
    assert(rule.id == 0);

    return 0;
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

int test_get_ack_w() {
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
    char ra_bin[] =
            "0001111111111111000000000000000000000000000000000000000000000000";
    char ra_bytes[8];
    bin_to_bytes(ra_bytes, ra_bin, DOWNLINK_MTU_BITS);
    CompoundACK receiver_abort;
    memcpy(receiver_abort.message, ra_bytes, DOWNLINK_MTU_BYTES);
    Rule rule;
    init_rule_from_ack(&rule, &receiver_abort);
    assert(is_ack_receiver_abort(&rule, &receiver_abort));

    char ack_bin[] =
            "0001110000000000000000000000000000000000000000000000000000000000";
    char ack_bytes[8];
    bin_to_bytes(ack_bytes, ack_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack;
    memcpy(ack.message, ack_bytes, DOWNLINK_MTU_BYTES);
    assert(!is_ack_receiver_abort(&rule, &ack));

    return 0;
}

int test_is_ack_compound() {
    char cack_bin[] =
            "0000001110001101110011111111001000000000000000000000000000000000";
    char cack_bytes[8];
    bin_to_bytes(cack_bytes, cack_bin, DOWNLINK_MTU_BITS);
    CompoundACK cack;
    memcpy(cack.message, cack_bytes, DOWNLINK_MTU_BYTES);
    Rule rule;
    init_rule_from_ack(&rule, &cack);
    assert(is_ack_compound(&rule, &cack));

    char ra_bin[] =
            "0001111111111111000000000000000000000000000000000000000000000000";
    char ra_bytes[8];
    bin_to_bytes(ra_bytes, ra_bin, DOWNLINK_MTU_BITS);
    CompoundACK receiver_abort;
    memcpy(receiver_abort.message, ra_bytes, DOWNLINK_MTU_BYTES);
    assert(!is_ack_compound(&rule, &receiver_abort));
    assert(is_ack_receiver_abort(&rule, &receiver_abort));

    return 0;
}

int test_is_ack_complete() {
    char ack_bin[] =
            "0001110000000000000000000000000000000000000000000000000000000000";
    char ack_bytes[9] = "";
    bin_to_bytes(ack_bytes, ack_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack;
    memcpy(ack.message, ack_bytes, DOWNLINK_MTU_BYTES);
    Rule rule;
    init_rule_from_ack(&rule, &ack);
    assert(is_ack_complete(&rule, &ack));


    CompoundACK complete_ack = {"\x1C\x00\x00\x00\x00\x00\x00\x00",
                                8};
    assert(is_ack_complete(&rule, &complete_ack));

    CompoundACK incomplete_ack = {"\x15\x88\x88\x88\x88\x88\x88\x88",
                                  4};
    assert(!is_ack_complete(&rule, &incomplete_ack));

    return 0;
}

int test_get_ack_nb_tuples() {
    char ack_bin[] =
            "0000001110001101110011111111001000000000000000000000000000000000";
    char ack_bytes[8] = "";
    bin_to_bytes(ack_bytes, ack_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack;
    memcpy(ack.message, ack_bytes, 8);
    Rule rule;
    init_rule_from_ack(&rule, &ack);
    assert(get_ack_nb_tuples(&rule, &ack) == 3);

    char one_bin[] =
            "0001110000000000000000000000000000000000000000000000000000000000";
    char one_bytes[8] = "";
    bin_to_bytes(one_bytes, one_bin, DOWNLINK_MTU_BITS);
    CompoundACK one;
    memcpy(one.message, one_bytes, 8);
    assert(get_ack_nb_tuples(&rule, &one) == 1);

    char two_bin[] =
            "0000001110001101110011000000000000000000000000000000000000000000";
    char two_bytes[8] = "";
    bin_to_bytes(two_bytes, two_bin, DOWNLINK_MTU_BITS);
    CompoundACK two;
    memcpy(two.message, two_bytes, DOWNLINK_MTU_BYTES);
    assert(get_ack_nb_tuples(&rule, &two) == 2);

    return 0;
}

int test_get_ack_tuples() {

    // Tuples of compound ACK
    char ack_bin[] =
            "0000001110001101110011111111001000000000000000000000000000000000";
    char ack_bytes[8] = "";
    bin_to_bytes(ack_bytes, ack_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack;
    memcpy(ack.message, ack_bytes, DOWNLINK_MTU_BYTES);
    Rule rule;
    init_rule_from_ack(&rule, &ack);

    int nb_tuples = get_ack_nb_tuples(&rule, &ack);
    char windows[nb_tuples][rule.m + 1];
    char bitmaps[nb_tuples][rule.window_size + 1];

    get_ack_tuples(&rule, &ack, nb_tuples, windows, bitmaps);

    char windows_expected[3][3] = {"00", "10", "11"};
    char bitmaps_expected[3][8] = {"1110001", "1110011", "1111001"};

    for (int i = 0; i < nb_tuples; i++) {
        assert(strcmp(windows[i], windows_expected[i]) == 0);
        assert(strcmp(bitmaps[i], bitmaps_expected[i]) == 0);
    }

    // Another
    char ack2_bin[] =
            "0000001110001101110011000000000000000000000000000000000000000000";
    char ack2_bytes[8] = "";
    bin_to_bytes(ack2_bytes, ack2_bin, DOWNLINK_MTU_BITS);
    CompoundACK ack2;
    memcpy(ack2.message, ack2_bytes, DOWNLINK_MTU_BYTES);

    int nb_tuples2 = get_ack_nb_tuples(&rule, &ack2);
    char windows2[nb_tuples2][rule.m + 1];
    char bitmaps2[nb_tuples2][rule.window_size + 1];

    get_ack_tuples(&rule, &ack2, nb_tuples2, windows2, bitmaps2);

    char windows_expected2[3][3] = {"00", "10"};
    char bitmaps_expected2[3][8] = {"1110001", "1110011"};

    for (int i = 0; i < nb_tuples2; i++) {
        assert(strcmp(windows2[i], windows_expected2[i]) == 0);
        assert(strcmp(bitmaps2[i], bitmaps_expected2[i]) == 0);
    }

    // Tuples of complete ACK
    char complete_ack_bin[] =
            "0001110000000000000000000000000000000000000000000000000000000000";
    char complete_ack_bytes[8] = "";
    bin_to_bytes(complete_ack_bytes, complete_ack_bin, DOWNLINK_MTU_BITS);
    CompoundACK complete_ack;
    memcpy(complete_ack.message, complete_ack_bytes, DOWNLINK_MTU_BYTES);

    int complete_nb_tuples = get_ack_nb_tuples(&rule, &complete_ack);
    char complete_windows[complete_nb_tuples][rule.m + 1];
    char complete_bitmaps[complete_nb_tuples][rule.window_size + 1];

    get_ack_tuples(&rule, &complete_ack, complete_nb_tuples, complete_windows,
                   complete_bitmaps);

    char complete_windows_expected[1][3] = {"11"};
    char complete_bitmaps_expected[1][8] = {"0000000"};

    for (int i = 0; i < complete_nb_tuples; i++) {
        assert(strcmp(complete_windows[i], complete_windows_expected[i]) == 0);
        assert(strcmp(complete_bitmaps[i], complete_bitmaps_expected[i]) == 0);
    }

    return 0;
}

int test_generate_ack() {

    // ====== 1 BYTE HEADER ======

    // Normal ACK

    Rule rule;
    init_rule(&rule, "000");

    CompoundACK ack;

    char bitmaps[rule.max_window_number][rule.window_size + 1];
    strncpy(bitmaps[0], "1110101\0", rule.window_size + 1);
    strncpy(bitmaps[1], "1111111\0", rule.window_size + 1);
    strncpy(bitmaps[2], "1110001\0", rule.window_size + 1);
    strncpy(bitmaps[3], "0000000\0", rule.window_size + 1);

    assert(generate_ack(&rule, &ack, 2, '0', bitmaps) == 0);

    int expected_tuples = 2;
    char expected_windows[expected_tuples][rule.m + 1];
    char expected_bitmaps[expected_tuples][rule.window_size + 1];
    strncpy(expected_windows[0], "00\0", rule.m + 1);
    strncpy(expected_windows[1], "10\0", rule.m + 1);
    strncpy(expected_bitmaps[0], "1110101\0", rule.window_size + 1);
    strncpy(expected_bitmaps[1], "1110001\0", rule.window_size + 1);

    test_routine_ack(rule, ack,
                     "000", "", "00",
                     "0", "1110101",
                     expected_tuples,
                     expected_windows,
                     expected_bitmaps,
                     1, 0);

    // Complete ACK

    CompoundACK ack_complete;
    char bitmaps_complete[rule.max_window_number][rule.window_size + 1];
    strncpy(bitmaps_complete[0], "1111111\0", rule.window_size + 1);
    strncpy(bitmaps_complete[1], "1111111\0", rule.window_size + 1);
    strncpy(bitmaps_complete[2], "1111111\0", rule.window_size + 1);
    strncpy(bitmaps_complete[3], "1111111\0", rule.window_size + 1);

    assert(generate_ack(&rule, &ack_complete, 3, '1', bitmaps_complete) == 0);

    int expected_tuples_complete = 1;
    char expected_windows_complete[expected_tuples_complete][rule.m + 1];
    char expected_bitmaps_complete[expected_tuples_complete][
            rule.window_size + 1];
    strncpy(expected_windows_complete[0], "11\0", rule.m + 1);
    strncpy(expected_bitmaps_complete[0], "0000000\0", rule.window_size + 1);

    test_routine_ack(rule, ack_complete,
                     "000", "", "11",
                     "1", "0000000",
                     expected_tuples_complete,
                     expected_windows_complete,
                     expected_bitmaps_complete,
                     0, 1);

    // Invalid ACK

    CompoundACK ack_invalid;
    char bitmaps_invalid[rule.max_window_number][rule.window_size + 1];
    strncpy(bitmaps_invalid[0], "0000000\0", rule.window_size + 1);
    strncpy(bitmaps_invalid[1], "0001000\0", rule.window_size + 1);
    strncpy(bitmaps_invalid[2], "0000000\0", rule.window_size + 1);
    strncpy(bitmaps_invalid[3], "0000000\0", rule.window_size + 1);

    assert(generate_ack(&rule, &ack_invalid, 3, '1', bitmaps_invalid) == -1);

    // ====== 2 BYTE HEADER OP.1 ======

    // Normal ACK

    Rule rule_2b1;
    init_rule(&rule_2b1, "111010");

    CompoundACK ack_2b1;

    char bitmaps_2b1[rule_2b1.max_window_number][rule_2b1.window_size + 1];
    strncpy(bitmaps_2b1[0], "111111111111\0", rule_2b1.window_size + 1);
    strncpy(bitmaps_2b1[1], "111010110010\0", rule_2b1.window_size + 1);
    strncpy(bitmaps_2b1[2], "111000101111\0", rule_2b1.window_size + 1);
    strncpy(bitmaps_2b1[3], "000000000000\0", rule_2b1.window_size + 1);

    assert(generate_ack(&rule_2b1, &ack_2b1, 2, '0', bitmaps_2b1) == 0);

    int expected_tuples_2b1 = 2;
    char expected_windows_2b1[expected_tuples_2b1][rule_2b1.m + 1];
    char expected_bitmaps_2b1[expected_tuples_2b1][rule_2b1.window_size + 1];
    strncpy(expected_windows_2b1[0], "01\0", rule_2b1.m + 1);
    strncpy(expected_windows_2b1[1], "10\0", rule_2b1.m + 1);
    strncpy(expected_bitmaps_2b1[0], "111010110010\0",
            rule_2b1.window_size + 1);
    strncpy(expected_bitmaps_2b1[1], "111000101111\0",
            rule_2b1.window_size + 1);

    test_routine_ack(rule_2b1, ack_2b1,
                     "111010", "", "01",
                     "0", "111010110010",
                     expected_tuples_2b1,
                     expected_windows_2b1,
                     expected_bitmaps_2b1,
                     1, 0);

    // Complete ACK

    CompoundACK ack_2b1_complete;

    char bitmaps_2b1_complete[rule_2b1.max_window_number][
            rule_2b1.window_size + 1];
    strncpy(bitmaps_2b1_complete[0], "111111111111\0",
            rule_2b1.window_size + 1);
    strncpy(bitmaps_2b1_complete[1], "111111111111\0",
            rule_2b1.window_size + 1);
    strncpy(bitmaps_2b1_complete[2], "111111111111\0",
            rule_2b1.window_size + 1);
    strncpy(bitmaps_2b1_complete[3], "111111111111\0",
            rule_2b1.window_size + 1);

    assert(generate_ack(&rule_2b1, &ack_2b1_complete, 3, '1',
                        bitmaps_2b1_complete) == 0);

    int expected_tuples_2b1_complete = 1;
    char expected_windows_2b1_complete[expected_tuples_2b1_complete][
            rule_2b1.m + 1];
    char expected_bitmaps_2b1_complete[expected_tuples_2b1_complete][
            rule_2b1.window_size + 1];
    strncpy(expected_windows_2b1_complete[0], "11\0", rule_2b1.m + 1);
    strncpy(expected_bitmaps_2b1_complete[0], "000000000000\0",
            rule_2b1.window_size + 1);

    test_routine_ack(rule_2b1, ack_2b1_complete,
                     "111010", "", "11",
                     "1", "000000000000",
                     expected_tuples_2b1_complete,
                     expected_windows_2b1_complete,
                     expected_bitmaps_2b1_complete,
                     0, 1);

    // ====== 2 BYTE HEADER OP.2 ======

    // Normal ACK

    Rule rule_2b2;
    init_rule(&rule_2b2, "11111101");

    CompoundACK ack_2b2;

    char bitmaps_2b2[rule_2b2.max_window_number][rule_2b2.window_size + 1];
    strncpy(bitmaps_2b2[0], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2[1], "1110101100101000101110010010011\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2[2], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2[3], "0000000000000000000000000000000\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2[4], "0000000000000000000000000000000\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2[5], "0000000000000000000000000000000\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2[6], "0000000000000000000000000000000\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2[7], "0000000000000000000000000000000\0",
            rule_2b2.window_size + 1);

    assert(generate_ack(&rule_2b2, &ack_2b2, 2, '0', bitmaps_2b2) == 0);

    int expected_tuples_2b2 = 1;
    char expected_windows_2b2[expected_tuples_2b2][rule_2b2.m + 1];
    char expected_bitmaps_2b2[expected_tuples_2b2][rule_2b2.window_size + 1];
    strncpy(expected_windows_2b2[0], "001\0", rule_2b2.m + 1);
    strncpy(expected_bitmaps_2b2[0], "1110101100101000101110010010011\0",
            rule_2b2.window_size + 1);

    test_routine_ack(rule_2b2, ack_2b2,
                     "11111101", "", "001",
                     "0", "1110101100101000101110010010011",
                     expected_tuples_2b2,
                     expected_windows_2b2,
                     expected_bitmaps_2b2,
                     1, 0);

    // Normal ACK, larger than downlink MTU

    CompoundACK ack_2b2_larger;

    char bitmaps_2b2_larger[rule_2b2.max_window_number][
            rule_2b2.window_size + 1];
    strncpy(bitmaps_2b2_larger[0], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_larger[1], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_larger[2], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_larger[3], "1000110110111010111010100010101\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_larger[4], "0000000000000000000000000000000\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_larger[5], "0000000000000111111100000000000\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_larger[6], "1011010000101011011010101000011\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_larger[7], "0000000000000000000000000000000\0",
            rule_2b2.window_size + 1);

    assert(generate_ack(&rule_2b2, &ack_2b2_larger, 6, '0',
                        bitmaps_2b2_larger) == 0);

    int expected_tuples_2b2_larger = 1;
    char expected_windows_2b2_larger[expected_tuples_2b2][rule_2b2.m + 1];
    char expected_bitmaps_2b2_larger[expected_tuples_2b2][
            rule_2b2.window_size + 1];
    strncpy(expected_windows_2b2_larger[0], "011\0", rule_2b2.m + 1);
    strncpy(expected_bitmaps_2b2_larger[0], "1000110110111010111010100010101\0",
            rule_2b2.window_size + 1);

    test_routine_ack(rule_2b2, ack_2b2_larger,
                     "11111101", "", "011",
                     "0", "1000110110111010111010100010101",
                     expected_tuples_2b2_larger,
                     expected_windows_2b2_larger,
                     expected_bitmaps_2b2_larger,
                     1, 0);

    // Complete ACK

    CompoundACK ack_2b2_complete;

    char bitmaps_2b2_complete[rule_2b2.max_window_number][
            rule_2b2.window_size + 1];
    strncpy(bitmaps_2b2_complete[0], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_complete[1], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_complete[2], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_complete[3], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_complete[4], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_complete[5], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_complete[6], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);
    strncpy(bitmaps_2b2_complete[7], "1111111111111111111111111111111\0",
            rule_2b2.window_size + 1);

    assert(generate_ack(&rule_2b2, &ack_2b2_complete, 7, '1',
                        bitmaps_2b2_complete) == 0);

    int expected_tuples_2b2_complete = 1;
    char expected_windows_2b2_complete[expected_tuples_2b2_complete][
            rule_2b2.m + 1];
    char expected_bitmaps_2b2_complete[expected_tuples_2b2_complete][
            rule_2b2.window_size + 1];
    strncpy(expected_windows_2b2_complete[0], "111\0", rule_2b2.m + 1);
    strncpy(expected_bitmaps_2b2_complete[0],
            "0000000000000000000000000000000\0", rule_2b2.window_size + 1);

    test_routine_ack(rule_2b2, ack_2b2_complete,
                     "11111101", "", "111",
                     "1", "0000000000000000000000000000000",
                     expected_tuples_2b2_complete,
                     expected_windows_2b2_complete,
                     expected_bitmaps_2b2_complete,
                     0, 1);

    return 0;
}

int test_generate_receiver_abort() {

    // Generate Receiver-Abort
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    CompoundACK receiver_abort;
    generate_receiver_abort(&rule, &receiver_abort);

    // Check fields
    char w[rule.m + 1];
    char c[2];
    int header_length = rule.ack_header_length;
    int header_remainder = header_length % L2_WORD_SIZE;
    int padding_size = header_remainder == 0
                       ? L2_WORD_SIZE
                       : 2 * L2_WORD_SIZE - header_remainder;
    int after_padding_size = DOWNLINK_MTU_BITS - header_length - padding_size;
    int padding_idx = rule.ack_header_length;
    int after_padding_idx = padding_idx + padding_size;
    char padding[padding_size + 1];
    char after_padding[after_padding_size + 1];

    get_ack_w(&rule, &receiver_abort, w);
    get_ack_c(&rule, &receiver_abort, c);
    char as_bin[UPLINK_MTU_BITS + 1];
    ack_to_bin(&receiver_abort, as_bin);
    memcpy(padding, as_bin + padding_idx, padding_size);
    padding[padding_size] = '\0';
    memcpy(after_padding, as_bin + after_padding_idx, after_padding_size);
    after_padding[after_padding_size] = '\0';

    assert(is_ack_receiver_abort(&rule, &receiver_abort));
    assert(is_monochar(w, '1'));
    assert(strcmp(c, "1") == 0);
    assert(is_monochar(padding, '1'));
    assert(is_monochar(after_padding, '0'));
    assert(!is_ack_compound(&rule, &receiver_abort));
    assert(!is_ack_complete(&rule, &receiver_abort));

    return 0;
}

int test_generate_null_ack() {
    CompoundACK ack;
    Rule rule;
    init_rule(&rule, "000");
    generate_null_ack(&ack);

    char empty[DOWNLINK_MTU_BYTES];
    memset(empty, '\0', DOWNLINK_MTU_BYTES);
    assert(memcmp(ack.message, empty, DOWNLINK_MTU_BYTES) == 0);
    assert(ack.byte_size == -1);

    return 0;
}

int test_is_ack_null() {
    // Null ACK
    CompoundACK ack;
    Rule rule;
    init_rule(&rule, "000");
    generate_null_ack(&ack);
    assert(is_ack_null(&ack));
    assert(!is_ack_complete(&rule, &ack));
    assert(!is_ack_compound(&rule, &ack));
    assert(!is_ack_receiver_abort(&rule, &ack));

    // Normal ack
    CompoundACK ack1 = {"\x15\x88\x88\x88", 4};
    assert(!is_ack_null(&ack1));

    return 0;
}

int test_is_ack_equal() {
    CompoundACK ack1 = {"\x15\x88\x88\x88\x00\x00\x00\x00", 8};
    CompoundACK ack2 = {"\x15\x88\x88\x88\x00\x00\x00\x00", 8};
    CompoundACK ack3 = {"\x32\x88\x88\x88\x00\x00\x00\x00", 8};
    CompoundACK ack4;
    generate_null_ack(&ack4);

    assert(ack_equal(&ack1, &ack1) == 1);
    assert(ack_equal(&ack2, &ack2) == 1);
    assert(ack_equal(&ack3, &ack3) == 1);
    assert(ack_equal(&ack4, &ack4) == 1);
    assert(ack_equal(&ack1, &ack2) == 1);
    assert(ack_equal(&ack1, &ack3) == 0);
    assert(ack_equal(&ack1, &ack4) == 0);

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
    printf("%d test_get_ack_tuples\n", test_get_ack_tuples());
    printf("%d test_generate_ack\n", test_generate_ack());
    printf("%d test_generate_receiver_abort\n", test_generate_receiver_abort());
    printf("%d test_generate_null_ack\n", test_generate_null_ack());
    printf("%d test_is_ack_null\n", test_is_ack_null());
    printf("%d test_is_ack_equal\n", test_is_ack_equal());

    return 0;
}