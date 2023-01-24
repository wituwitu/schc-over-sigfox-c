/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bsd/string.h>
#include "misc.h"

int bin_to_int(char* bits) {
    return strtol(bits, NULL, 2);
}

void int_to_bin(char* dest, int n, int size) {
    char* zeroes = repeat_char('0', size);
    strlcpy(dest, zeroes, size);
    free(zeroes);

    printf("dest after zeroes: %s\n", dest);

    int i = 0;
    while (n > 0) {
        dest[i++] = n % 2 == 0 ? '0' : '1';
        n /= 2;
    }
    dest[length + 1] = '\0';
    printf("bin: %s\n", dest);

    str_rev(dest);

    printf("res (inner): %s\n", dest);
}

void bytes_to_bin(char* bytes) {
    for (int i = 0; bytes[i] != 0; i++) {

    }
}
*/