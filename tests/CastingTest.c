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
    char expected[9] = "11010010";
    char actual[9];
    char* res = int_to_bin(n, 8);
    strcpy(actual, res);
    free(res);
    return strcmp(actual, expected) == 0;
}

/*void bytesToBinTest(void) {
    char bytes[] = "cafe";
    int expected = 0b1100101011111110;
    return bytes_to_bin(bytes);
}*/

int main() {
    printf("%d\n", binToIntTest());
    printf("%d\n", intToBinTest());
    return binToIntTest() && intToBinTest();
}
