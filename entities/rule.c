#include "rule.h"
#include "casting.h"
#include "misc.h"
#include "schc.h"
#include <math.h>

void init_rule(Rule *rule, const char rule_id_binary[]) {
    int id, rule_id_size, t, m, n, window_size, u,
      header_length, all1_header_length, ack_header_length,
      max_window_number, max_fragment_number;

    memset(rule, 0, sizeof(*rule));

    char single_byte_header[4];
    memcpy(single_byte_header, rule_id_binary, 3);
    single_byte_header[3] = '\0';

    if (strcmp(single_byte_header, "111") != 0) {
        id = bin_to_int(single_byte_header);
        rule_id_size = 3;
        t = 0;
        m = 2;
        n = 3;
        window_size = 7;
        u = 3;
    } else {
        char double_byte_header[7] = {};
        memcpy(double_byte_header, rule_id_binary, 6);
        double_byte_header[6] = '\0';
        if (strcmp(double_byte_header, "111111") != 0) {
            id = bin_to_int(double_byte_header);
            rule_id_size = 6;
            t = 0;
            m = 2;
            n = 4;
            window_size = 12;
            u = 4;
        } else {
            id = bin_to_int(rule_id_binary);
            rule_id_size = 8;
            t = 0;
            m = 3;
            n = 5;
            window_size = 31;
            u = 5;
        }
    }

    header_length = round_to_next_multiple(
            rule_id_size + t + m + n,
            L2_WORD_SIZE
            );

    all1_header_length = round_to_next_multiple(
            rule_id_size + t + m + n + u,
            L2_WORD_SIZE
            );

    ack_header_length = rule_id_size + m + 1;
    max_window_number = (int) pow(2, m);
    max_fragment_number = max_window_number * window_size;

    rule->id = id;
    rule->rule_id_size = rule_id_size;
    rule->t = t;
    rule->m = m;
    rule->n = n;
    rule->window_size = window_size;
    rule->u = u;
    rule->header_length = header_length;
    rule->all1_header_length = all1_header_length;
    rule->ack_header_length = ack_header_length;
    rule->max_window_number = max_window_number;
    rule->max_fragment_number = max_fragment_number;
}

void parse_rule_from_bytes(Rule *rule, const char byt[]) {
    size_t bin_length = strlen(byt) * 8;
    char as_bin[bin_length + 1];
    memset(as_bin, 0, bin_length + 1);
    bytes_to_bin(as_bin, byt, bin_length);

    char first_byte[9];
    memcpy(first_byte, as_bin, 8);
    first_byte[8] = '\0';

    init_rule(rule, first_byte);
}