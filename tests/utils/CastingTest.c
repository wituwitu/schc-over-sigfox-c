#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "casting.h"
#include "misc.h"

int bin_to_int_test(void) {

    assert(bin_to_int("11010010") == 210);
    assert(bin_to_int("000011010010") == 210);
    assert(bin_to_int("111111111111") == 4095);
    assert(bin_to_int("111000") == 56);

    return 0;
}

int char_to_bin_test(void) {

    // Eight bits
    char n = 120;
    char *expected_eight = "01111000";
    char actual_eight[9] = "";
    int_to_bin(actual_eight, n, 8);
    assert(strcmp(actual_eight, expected_eight) == 0);

    // Sixteen bits
    char expected_sixt[17] = "0000000001111000";
    char actual_sixt[17] = "";
    char byt_sixt[9] = "";
    int_to_bin(byt_sixt, n, 16);
    zfill(actual_sixt, byt_sixt, 16);
    assert(strcmp(actual_sixt, expected_sixt) == 0);

    // 24 bits
    char n_sec = 75;
    char *expected_sec = "000000000000000001001011";
    char actual_sec[25] = "";
    char byt_sec[9] = "";
    int_to_bin(byt_sec, n_sec, 24);
    zfill(actual_sec, byt_sec, 24);
    assert(strcmp(actual_sec, expected_sec) == 0);

    // Negative number
    char neg = -46;
    char *expected_neg = "11010010";
    char actual_neg[9] = "";
    int_to_bin(actual_neg, neg, 8);
    assert(strcmp(actual_neg, expected_neg) == 0);

    char neg_sec = -120;
    char *expected_neg_sec = "10001000";
    char actual_neg_sec[9] = "";
    int_to_bin(actual_neg_sec, neg_sec, 8);
    assert(strcmp(actual_neg_sec, expected_neg_sec) == 0);

    // Two bits
    char two = 2;
    char *expected_two = "10";
    char actual_two[3] = "";
    int_to_bin(actual_two, two, 2);
    assert(strcmp(actual_two, expected_two) == 0);

    // Four bits
    char three = 3;
    char *expected_three = "0011";
    char actual_three[5] = "";
    int_to_bin(actual_three, three, 4);
    assert(strcmp(actual_three, expected_three) == 0);

    // One
    char one = 1;
    char *expected_one = "01";
    char actual_one[3] = "";
    int_to_bin(actual_one, one, 2);
    assert(strcmp(actual_one, expected_one) == 0);

    return 0;
}

int bytes_to_bin_test(void) {
    char by[] = "\xd2";
    char expected[] = "11010010";
    char actual[9] = "";
    assert(bytes_to_bin(actual, by, 1) == 8);
    assert(strcmp(actual, expected) == 0);

    char by_sec[] = "test";
    char expected_sec[] = "01110100011001010111001101110100";
    char actual_sec[33] = "";
    assert(bytes_to_bin(actual_sec, by_sec, 4) == 32);
    assert(strcmp(actual_sec, expected_sec) == 0);

    char by_hex[] = "\x15\x88\x88\x88";
    char expected_hex[] = "00010101100010001000100010001000";
    char actual_hex[33] = "";
    assert(bytes_to_bin(actual_hex, by_hex, 4) == 32);
    assert(strcmp(actual_hex, expected_hex) == 0);

    return 0;
}

int bin_to_bytes_test(void) {
    char *bits = "11010010";
    char expected[] = "\xd2";
    char actual[2] = "";
    assert(bin_to_bytes(actual, bits, 8) == 1);
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