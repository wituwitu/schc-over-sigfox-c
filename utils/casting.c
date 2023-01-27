
#include <stdlib.h>
#include <string.h>
#include "misc.h"

long bin_to_int(char* bits) {
    return strtol(bits, NULL, 2);
}

void int_to_bin(char* dest, unsigned int n, unsigned int size) {
    unsigned int nb_bits;
    unsigned int aux = n;
    for (nb_bits = 0; aux != 0; ++nb_bits) aux >>= 1;

    int i = 0;
    char res[nb_bits + 1];
    while (n != 0) {
        res[i++] = n % 2 == 0 ? '0' : '1';
        n /= 2;
    }
    res[nb_bits] = '\0';

    strrev(res);
    zfill(dest, res, size - strlen(dest));
}

