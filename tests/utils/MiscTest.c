#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "casting.h"
#include "misc.h"

int repeat_char_test(void) {
    char c = '1';
    char dest[9] = "";
    size_t size = repeat_char(dest, c, 8);

    char *expected = "11111111";

    assert(strcmp(dest, expected) == 0);
    assert(size == 8);
    assert(size == strlen(expected));
    assert(dest[8] == '\0');

    return 0;
}

int str_rev_test(void) {
    char s[] = "asdfghjkl";
    size_t length = strlen(s);
    char original[length + 1];
    strncpy(original, s, length);
    strrev(s);

    char *expected = "lkjhgfdsa";

    size_t size = strlen(s);
    assert(size == 9);
    assert(size == strlen(expected));
    assert(strcmp(s, expected) == 0);
    assert(s[9] == '\0');

    return 0;
}

int zfill_test(void) {
    char src[] = "test";
    char dest[10] = "";
    zfill(dest, src, 9);

    char expected[] = "00000test";

    assert(strcmp(dest, expected) == 0);
    assert(strlen(dest) == 9);
    assert(dest[9] == '\0');

    return 0;
}

int replace_char_test(void) {
    char s[] = "1001111";
    replace_char(s, 2, '1');
    char expected_fir[] = "1011111";
    assert(strcmp(s, expected_fir) == 0);

    char t[] = "1001111";
    replace_char(t, 10, '1');
    char expected_sec[] = "1001111";
    assert(strcmp(t, expected_sec) == 0);

    return 0;
}

int is_monochar_test(void) {
    char s[] = "11111111";
    assert(is_monochar(s, '1') == 1);
    assert(is_monochar(s, '0') == 0);

    char t[] = "10010110";
    assert(is_monochar(t, '1') == 0);
    assert(is_monochar(t, '0') == 0);

    char u[] = "";
    assert(is_monochar(u, '1') == 0);

    return 0;
}

int generate_packet_test(void) {
    int byte_size_fir = 10;
    char dest_fir[byte_size_fir + 1];

    generate_packet(dest_fir, byte_size_fir);

    assert(strcmp(dest_fir, "0123456789") == 0);

    int byte_size_sec = 20;
    char dest_sec[byte_size_sec + 1];

    generate_packet(dest_sec, byte_size_sec);

    assert(strcmp(dest_sec, "01234567890123456789") == 0);

    return 0;
}

int round_to_next_multiple_test(void) {
    assert(round_to_next_multiple(8, 7) == 14);
    assert(round_to_next_multiple(14, 7) == 14);
    assert(round_to_next_multiple(15, 7) == 21);
    assert(round_to_next_multiple(0, 7) == 0);
    assert(round_to_next_multiple(-20, 7) == -14);

    return 0;
}

int main() {

    printf("%d repeat_char_test\n", repeat_char_test());
    printf("%d str_rev_test\n", str_rev_test());
    printf("%d zfill_test\n", zfill_test());
    printf("%d replace_char_test\n", replace_char_test());
    printf("%d is_monochar_test\n", is_monochar_test());
    printf("%d generate_packet_test\n", generate_packet_test());
    printf("%d round_to_next_multiple_test\n", round_to_next_multiple_test());

    return 0;
}
