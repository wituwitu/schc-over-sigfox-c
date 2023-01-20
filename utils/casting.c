#include <stdlib.h>
#include <stdio.h>

int bin_to_int(char* bits) {
    return strtol(bits, NULL, 2);
}

char* int_to_bin(int n, int length) {
    char bin[length + 1];
    char* res = malloc(length + 1);
    int i = 0;
    while (n > 0) {
        bin[i++] = n % 2 == 0 ? '0' : '1';
        n /= 2;
    }
    bin[length + 1] = '\0';
    for (int j = 0; j < length + 1; j++) {
        res[j] = bin[length - 1 - j];
    }
    res[length + 1] = '\0';
    return res;
}

/*
void bytes_to_bin(char* bytes) {
    for (int i = 0; bytes[i] != 0; i++) {

    }
}
*/