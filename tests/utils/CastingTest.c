#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "casting.h"
#include "misc.h"

int bin_to_int_test(void) {
    char bin[] = "11010010";
    int expected = 210;
    int actual = bin_to_int(bin);

    assert(actual == expected);

    char bin_second[] = "000011010010";
    int actual_second = bin_to_int(bin_second);
    assert(actual_second == expected);

    char bin_third[] = "111111111111";
    int expected_third = 4095;
    int actual_third = bin_to_int(bin_third);
    assert(actual_third == expected_third);

    return 0;
}

int char_to_bin_test(void) {
    char n = 120;
    char *expected_eight = "01111000";
    char actual_eight[9] = "";
    char_to_bin(actual_eight, n, 8);
    assert(strcmp(actual_eight, expected_eight) == 0);

    char expected_sixt[17] = "0000000001111000";
    char actual_sixt[17] = "";
    char_to_bin(actual_sixt, n, 16);
    assert(strcmp(actual_sixt, expected_sixt) == 0);

    char n_sec = 75;
    char *expected_sec = "000000000000000001001011";
    char actual_sec[33] = "";
    char_to_bin(actual_sec, n_sec, 24);
    assert(strcmp(actual_sec, expected_sec) == 0);

    char neg = -46;
    char *expected_neg = "11010010";
    char actual_neg[9] = "";
    char_to_bin(actual_neg, neg, 8);
    assert(strcmp(actual_neg, expected_neg) == 0);

    char neg_sec = -120;
    char *expected_neg_sec = "10001000";
    char actual_neg_sec[9] = "";
    char_to_bin(actual_neg_sec, neg_sec, 8);
    assert(strcmp(actual_neg_sec, expected_neg_sec) == 0);

    return 0;
}

int bytes_to_bin_test(void) {
    char by[] = "\xd2";
    char expected[] = "11010010";
    char actual[9] = "";
    bytes_to_bin(actual, by, 1);
    assert(strcmp(actual, expected) == 0);

    char by_sec[] = "test";
    char expected_sec[] = "01110100011001010111001101110100";
    char actual_sec[33] = "";
    bytes_to_bin(actual_sec, by_sec, 4);
    assert(strcmp(actual_sec, expected_sec) == 0);

    char by_hex[] = "\x15\x88\x88\x88";
    char expected_hex[] = "00010101100010001000100010001000";
    char actual_hex[33] = "";
    bytes_to_bin(actual_hex, by_hex, 4);
    assert(strcmp(actual_hex, expected_hex) == 0);


    return 0;
}

int bin_to_bytes_test(void) {
    char *bits = "11010010";
    char expected[] = "\xd2";
    char actual[2] = "";
    bin_to_bytes(actual, bits, 1);
    assert(strcmp(actual, expected) == 0);

    return 0;
}

int main() {
    printf("%d bin_to_int_test\n", bin_to_int_test());
    printf("%d int_to_bin_test\n", char_to_bin_test());
    printf("%d bytes_to_bin_test\n", bytes_to_bin_test());
    printf("%d bin_to_bytes_test\n", bin_to_bytes_test());
    return 0;
}