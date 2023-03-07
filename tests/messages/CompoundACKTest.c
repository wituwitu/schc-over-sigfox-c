#include "ack.h"
#include "casting.h"
#include "misc.h"
#include <assert.h>
#include <stdio.h>

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

int test_generate_receiver_abort() {

    // Generate Receiver-Abort
    Fragment fragment = {"\x15\x88\x88\x88", 4};
    Rule rule;
    init_rule_from_frg(&rule, &fragment);
    CompoundACK receiver_abort;
    generate_receiver_abort(&rule, &fragment, &receiver_abort);

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
    printf("%d test_generate_receiver_abort\n", test_generate_receiver_abort());

    return 0;
}