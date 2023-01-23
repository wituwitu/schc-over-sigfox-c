//
// Created by witu on 23-01-23.
//

#include <string.h>
#include <malloc.h>
#include "misc.h"

char* repeat_char(char c, int length) {
    char* res = malloc(length + 1);
    for (int i = 0; i < length; i++) {
        res[i] = c;
    }
    res[length + 1] = '\0';
    return res;
}

void str_rev(char* s) {
    int length = (int) strlen(s);
    char rev[length + 1];
    for (int i = 0; i < length + 1; i++) {
        rev[i] = s[length - 1 - i];
    }
    for (int j = 0; j < length; j++) {
        s[j] = rev[j];
    }
}