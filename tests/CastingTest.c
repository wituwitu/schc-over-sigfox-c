#include <stdio.h>
#include <string.h>
#include "casting.h"
#include "misc.h"

int bin_to_int_test(void) {
    char bin[] = "11010010";
    int expected = 210;
    int actual = bin_to_int(bin);

    int equal_first = actual == expected;

    char bin_second[] = "000011010010";
    int actual_second = bin_to_int(bin_second);
    int equal_second = actual_second == expected;

    char bin_third[] = "111111111111";
    int expected_third = 4095;
    int actual_third = bin_to_int(bin_third);
    int equal_third = actual_third == expected_third;

    return equal_first
    && equal_second
    && equal_third;
}

int int_to_bin_test(void) {
    int n = 120;
    char* expected_eight = "01111000";
    char actual_eight[9] = "";
    int_to_bin(actual_eight, n, 8);
    int eight_bit = strcmp(actual_eight, expected_eight) == 0;

    char expected_sixt[17] = "0000000001111000";
    char actual_sixt[17] = "";
    int_to_bin(actual_sixt, n, 16);
    int sixt_bit = strcmp(actual_sixt, expected_sixt) == 0;

    int n_sec = 75;
    char* expected_sec = "000000000000000001001011";
    char actual_sec[33] = "";
    int_to_bin(actual_sec, n_sec, 24);
    int twfo_bit = strcmp(actual_sec, expected_sec) == 0;


    int neg = -46;
    char* expected_neg = "11010010";
    char actual_neg[9] = "";
    int_to_bin(actual_neg, neg, 8);
    int neg_bit = strcmp(actual_neg, expected_neg) == 0;

    int neg_sec = -120;
    char* expected_neg_sec = "10001000";
    char actual_neg_sec[9] = "";
    int_to_bin(actual_neg_sec, neg_sec, 8);
    int neg_bit_sec = strcmp(actual_neg_sec, expected_neg_sec) == 0;

    return eight_bit && sixt_bit && twfo_bit && neg_bit && neg_bit_sec;
}

int bytes_to_bin_test(void) {
    char* by = "\xd2";
    char expected[] = "11010010";
    char actual[9] = "";

    bytes_to_bin(actual, by, 8);
    int eight_bit = strcmp(actual, expected) == 0;

    char* by_sec = "test";
    char expected_sec[] = "01110100011001010111001101110100";
    char actual_sec[33] = "";

    bytes_to_bin(actual_sec , by_sec, 32);
    int empty = strcmp(actual_sec, expected_sec) == 0;

    return eight_bit && empty;
}

int bin_to_bytes_test(void) {
    char* bits = "11010010";
    char expected[] = "\xd2";
    char actual[2] = "";
    bin_to_bytes(actual, bits, 1);
    int fir = strcmp(actual, expected) == 0;

    return fir;

}

int main() {
    printf("%d bin_to_int_test\n", bin_to_int_test());
    printf("%d int_to_bin_test\n", int_to_bin_test());
    printf("%d bytes_to_bin_test\n", bytes_to_bin_test());
    printf("%d bin_to_bytes_test\n", bin_to_bytes_test());
    return 0;
}