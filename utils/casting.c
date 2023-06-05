#include <stdlib.h>
#include <string.h>
#include "misc.h"

int bin_to_int(const char *bits) {
    return (int) strtol(bits, NULL, 2);
}

void int_to_bin(char dest[], signed int src, int dest_size) {
    memset(dest, '\0', dest_size + 1);

    size_t int_sz = sizeof(int) * 8;

    char res[int_sz];
    int shift = (int) int_sz - 1;

    int i = 0;
    for (; shift >= 0; shift--) {
        int k = src >> shift;
        res[i++] = k & 1 ? '1' : '0';
    }

    strncpy(dest, res + int_sz - dest_size, dest_size);
}

int bytes_to_bin(char *dest, const char *src, int src_size) {
    int dest_size = src_size * 8;
    char bits[dest_size + 1];
    char *p = bits;

    memset(dest, '\0', dest_size + 1);
    memset(bits, '\0', dest_size + 1);

    for (int i = 0; i != src_size; i++) {
        char as_bin[8];
        int_to_bin(as_bin, src[i], 8);

        for (int j = 0; j < 8; j++) {
            *p++ = as_bin[j];
        }
    }

    bits[dest_size] = '\0';
    memcpy(dest, bits, dest_size);
    return dest_size;
}

int bin_to_bytes(char *dest, char *src, int src_size) {
    int dest_size = src_size / 8;
    char bytes[dest_size + 1];
    char *p = src;
    int i = 0;
    while (*p != '\0') {
        char oct[8];
        strncpy(oct, p, 8);
        char byt = (char) bin_to_int(oct);
        bytes[i++] = byt;
        p += 8;
    }
    bytes[i] = '\0';
    memcpy(dest, bytes, dest_size);
    return dest_size;
}