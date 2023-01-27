
#include <stdlib.h>
#include <string.h>
#include "misc.h"

long bin_to_int(char* bits) {
    return strtol(bits, NULL, 2);
}

void int_to_bin(char* dest, int n, unsigned int length) {
    size_t int_sz = sizeof(int) * 8;

    char res[int_sz];
    int shift = (int) int_sz - 1;

    int i = 0;
    for (; shift >= 0; shift--) {
        int k = n >> shift;
        res[i++] = k & 1 ? '1' : '0';
    }

    unsigned int trim = int_sz - length;
    int j = 0;
    for (; j < length; j++) {
        dest[j] = res[trim + j];
    }
    dest[j] = '\0';
}


void bytes_to_bin(char* dest, char* bytes, unsigned int length) {
    int size = (int) strlen(bytes) * 8;
    char bits[size + 1];
    char* p = bits;

    for (int i = 0; bytes[i] != 0; i++) {
        int as_int = (int) bytes[i] + 0;
        char as_bin[8];
        int_to_bin(as_bin, as_int, 8);

        for (int j = 0; j < 8; j++) {
            *p++ = as_bin[j];
        }
    }

    bits[size] = '\0';
    zfill(dest, bits, length - strlen(dest));
}