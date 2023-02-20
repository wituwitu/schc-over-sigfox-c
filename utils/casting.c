#include <stdlib.h>
#include <string.h>
#include "misc.h"

long bin_to_int(const char *bits) {
    return strtol(bits, NULL, 2);
}

void char_to_bin(char *dest, signed char n, unsigned int length) {
    memset(dest, '\0', length);

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

void bytes_to_bin(char *dest, const char *bytes, unsigned int length) {
    memset(dest, '\0', length);

    int size = (int) strlen(bytes) * 8;
    char bits[size + 1];
    char *p = bits;

    for (int i = 0; bytes[i] != 0; i++) {
        char as_bin[8];
        char_to_bin(as_bin, bytes[i], 8);

        for (int j = 0; j < 8; j++) {
            *p++ = as_bin[j];
        }
    }

    bits[size] = '\0';
    zfill(dest, bits, length - strlen(dest));
}

void bin_to_bytes(char *dest, const char *bits, unsigned int length) {
    memset(dest, '\0', length);

    int size = (int) strlen(bits) / 8;
    char bytes[size + 1];
    char *p = bits;
    int i = 0;
    for (; *p != '\0'; p += 8) {
        char oct[8];
        strncpy(oct, p, 8);
        char byt = (char) bin_to_int(oct);
        bytes[i++] = byt;
    }
    bytes[i] = '\0';

    zfill(dest, bytes, length - strlen(dest));
}