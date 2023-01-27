#include <stdio.h>
#include <string.h>
#include <malloc.h>
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
    int n = 210;
    char* expected_eight = "11010010";
    char actual_eight[9] = "";
    int_to_bin(actual_eight, n, 8);
    int eight_bit = strcmp(actual_eight, expected_eight) == 0;

    char expected_sixt[17] = "0000000011010010";
    char actual_sixt[17] = "";
    int_to_bin(actual_sixt, n, 16);
    int sixt_bit = strcmp(actual_sixt, expected_sixt) == 0;

    int n_sec = 75;
    char* expected_sec = "000000000000000001001011";
    char actual_sec[32] = "";
    int_to_bin(actual_sec, n_sec, 24);
    int twfo_bit = strcmp(actual_sec, expected_sec) == 0;

    return eight_bit && sixt_bit && twfo_bit;
}

int main() {
    printf("%d bin_to_int_test\n", bin_to_int_test());
    printf("%d int_to_bin_test\n", int_to_bin_test());
    return 0;
}