#include <string.h>
#include <stdio.h>
#include <math.h>
#include "misc.h"

size_t repeat_char(char *dest, char c, int length) {
    for (int i = 0; i < length; i++) dest[i] = c;
    dest[length + 1] = '\0';
    return strlen(dest);
}

void strrev(char *start) {
    char temp, *end;

    if (start == NULL || !(*start)) return;
    end = start + strlen(start) - 1;

    while (end > start) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

void zfill(char *dest, char *src, unsigned int length) {
    size_t len = strlen(src);
    size_t zeros = (len > length) ? 0 : length - len;
    memset(dest, '0', zeros);
    strncpy(dest + zeros, src, length);
}

void replace_char(char *src, int idx, char c) { src[idx] = c; }

int is_monochar(char *s, char c) {
    if (s[0] == '\0')
        return 0;
    size_t len = strlen(s);
    for (int i = 0; i < len; i++) {
        if (s[i] != c)
          return 0;
    }
    return 1;
}

void generate_packet(char *dest, int length) {
    char s[] = "0";
    int i = 0;
    int j = 0;
    for (; j < length; j++) {
        dest[j] = *s;
        i = (i + 1) % 10;
        sprintf(s, "%d", i);
    }
    dest[j] = '\0';
}

int round_to_next_multiple(signed int n, signed int factor) {
    double result = floor((double) n / (-factor));
    return (int) -result * factor;
}