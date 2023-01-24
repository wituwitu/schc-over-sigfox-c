#include <string.h>
#include <stdio.h>
#include "casting.h"
#include "misc.h"

int repeat_char_test(void) {
    char c = '1';
    char dest[9] = "";
    size_t size = repeat_char(dest, c, 8);

    char* expected = "11111111";

    int equal = strcmp(dest, expected) == 0;
    int size_check = size == 8;
    int equal_size = size == strlen(expected);
    int ends_in_NUL = dest[9] == '\0';

    return equal
    && size_check
    && equal_size
    && ends_in_NUL;
}

int str_rev_test(void) {
    char s[] = "asdfghjkl";
    size_t length = strlen(s);
    char original[length + 1];
    strncpy(original, s, length);
    strrev(s);

    char* expected = "lkjhgfdsa";

    size_t size = strlen(s);
    int size_check = size == 9;
    int equal_size = size == strlen(expected);
    int equal = strcmp(s, expected) == 0;
    int ends_in_NUL = s[10] == '\0';

    return equal
    && size_check
    && equal_size
    && ends_in_NUL;
}

int zfill_test(void) {
    char* src = "test";
    char dest[10] = "";
    zfill(dest, src, 9);

    char* expected = "00000test";

    int equal = strcmp(dest, expected) == 0;
    int size_check = strlen(dest) == 9;
    int ends_in_NUL = dest[9] == '\0';

    return equal
    && size_check
    && ends_in_NUL;
    // TODO: Make this not smash the stack
}

int main() {

    printf("%d\n", repeat_char_test());
    printf("%d\n", str_rev_test());
    printf("%d\n", zfill_test());

    return 0;
}
