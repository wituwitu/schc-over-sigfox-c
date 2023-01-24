//
// Created by witu on 23-01-23.
//

#include <string.h>
#include "misc.h"

size_t repeat_char(char* dest, char c, int length) {
    for (int i = 0; i < length; i++) {
        dest[i] = c;
    }
    dest[length + 1] = '\0';
    return strlen(dest);
}

void strrev(char* start) {
    char temp, *end;

    if(start == NULL || !(*start) )
        return;

    end = start + strlen(start) - 1;

    while( end > start ) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

void zfill(char* dest, const char* src, unsigned int length) {
    size_t len = strlen(src);
    size_t zeros = (len > length) ? 0: length - len;
    memset(dest, '0', zeros);
    strncpy(dest + zeros, src, length);
}