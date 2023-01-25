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
    int ends_in_NUL = dest[8] == '\0';

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
    int ends_in_NUL = s[9] == '\0';

    return equal
    && size_check
    && equal_size
    && ends_in_NUL;
}

int zfill_test(void) {
    char src[] = "test";
    char dest[10] = "";
    zfill(dest, src, 9);

    char expected[] = "00000test";

    int equal = strcmp(dest, expected) == 0;
    int size_check = strlen(dest) == 9;
    int ends_in_NUL = dest[9] == '\0';

    return equal
    && size_check
    && ends_in_NUL;
}

int replace_char_test(void) {
    char s[] = "1001111";
    replace_char(s, 2, '1');
    char expected_fir[] = "1011111";
    int equal_fir = strcmp(s, expected_fir) == 0;

    char t[] = "1001111";
    replace_char(t, 10, '1');
    char expected_sec[] = "1001111";
    int equal_sec = strcmp(t, expected_sec) == 0;

    return equal_fir
    && equal_sec;
}

int is_monochar_test(void) {
    char s[] = "11111111";
    int fir = is_monochar(s, '1') == 1;
    int sec = is_monochar(s, '0') == 0;

    char t[] = "10010110";
    int thi = is_monochar(t, '1') == 0;
    int fou = is_monochar(t, '0') == 0;

    char u[] = "";
    int fif = is_monochar(u, '1') == 0;

    return fir && sec && thi && fou && fif;
}

int generate_packet_test(void) {
    int byte_size_fir = 10;
    char dest_fir[byte_size_fir + 1];

    generate_packet(dest_fir, byte_size_fir);

    int fir = strcmp(dest_fir, "0123456789") == 0;

    int byte_size_sec = 20;
    char dest_sec[byte_size_sec + 1];

    generate_packet(dest_sec, byte_size_sec);

    int sec = strcmp(dest_sec, "01234567890123456789") == 0;

    return fir && sec;
//    return fir && sec && thi && fou && fif;
}

int main() {

    printf("%d repeat_char_test\n", repeat_char_test());
    printf("%d str_rev_test\n", str_rev_test());
    printf("%d zfill_test\n", zfill_test());
    printf("%d replace_char_test\n", replace_char_test());
    printf("%d is_monochar_test\n", is_monochar_test());
    printf("%d generate_packet_test\n", generate_packet_test());

    return 0;
}
