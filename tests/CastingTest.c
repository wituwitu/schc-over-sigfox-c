#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "casting.h"

int binToIntTest(void) {
    char bin[] = "11010010";
    int expected = 210;
    int actual = bin_to_int(bin);
    return actual == expected ? 1 : 0;
}

int intToBinTest(void) {
    int n = 210;
    char* expected_eight = "11010010";
    char actual_eight[9];
    int_to_bin(actual_eight, n);
    int eight_bit = strcmp(actual_eight, expected_eight) == 0;

    char expected_sixt[17] = "0000000011010010";
    char actual_sixt[17];
    int_to_bin(actual_sixt, n);
    int sixt_bit = strcmp(actual_sixt, expected_sixt) == 0;

    printf("expected sixt: %s\n", expected_sixt);
    printf("actual sixt: %s\n", actual_sixt);

    return eight_bit && sixt_bit;
}

int main() {
    // printf("%d\n", binToIntTest());
    // printf("%d\n", intToBinTest());
    return binToIntTest() && intToBinTest();
}
