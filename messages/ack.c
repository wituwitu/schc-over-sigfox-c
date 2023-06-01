#include <stdio.h>
#include "ack.h"
#include "casting.h"
#include "misc.h"

void ack_to_bin(CompoundACK *ack, char dest[DOWNLINK_MTU_BITS + 1]) {
    memset(dest, '\0', DOWNLINK_MTU_BITS + 1);
    char *message = ack->message;
    bytes_to_bin(dest, message, sizeof(message));
}

void init_rule_from_ack(Rule *dest, CompoundACK *ack) {
    char as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, as_bin);
    init_rule(dest, as_bin);
}

void get_ack_rule_id(Rule *rule, CompoundACK *ack,
                     char dest[rule->rule_id_size + 1]) {
    char ack_as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, ack_as_bin);
    strncpy(dest, ack_as_bin + rule->ack_indices.rule_id_idx,
            rule->rule_id_size);
    dest[rule->rule_id_size] = '\0';
}

void get_ack_dtag(Rule *rule, CompoundACK *ack, char dest[rule->t + 1]) {
    char ack_as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, ack_as_bin);
    strncpy(dest, ack_as_bin + rule->ack_indices.dtag_idx, rule->t);
    dest[rule->t] = '\0';
}

void get_ack_w(Rule *rule, CompoundACK *ack, char dest[rule->m + 1]) {
    char ack_as_bin[UPLINK_MTU_BITS + 1];
    ack_to_bin(ack, ack_as_bin);
    strncpy(dest, ack_as_bin + rule->ack_indices.w_idx, rule->m);
    dest[rule->m] = '\0';
}

void get_ack_c(Rule *rule, CompoundACK *ack, char dest[2]) {
    char ack_as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, ack_as_bin);
    strncpy(dest, ack_as_bin + rule->ack_indices.c_idx, 1);
    dest[1] = '\0';
}

void get_ack_bitmap(Rule *rule, CompoundACK *ack,
                    char dest[rule->window_size + 1]) {
    char ack_as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, ack_as_bin);
    strncpy(dest, ack_as_bin + rule->ack_indices.bitmap_idx, rule->window_size);
    dest[rule->window_size] = '\0';
}

int get_ack_nb_tuples(Rule *rule, CompoundACK *ack) {
    char as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, as_bin);
    char window[rule->m + 1];
    char first_window[rule->m + 1];
    memset(window, '\0', rule->m + 1);
    memset(first_window, '0', rule->m);
    first_window[rule->m] = '\0';
    char *p = as_bin + rule->ack_indices.tuple_idx;

    int nb_tuples = 1;
    strncpy(window, p, rule->m);
    while (strcmp(window, first_window) != 0 &&
            p < (as_bin + DOWNLINK_MTU_BITS - rule->ack_indices.tuple_idx)) {
        nb_tuples++;
        p += rule->m + rule->window_size;
        strncpy(window, p, rule->m);
    }

    return nb_tuples;
}

void get_ack_tuples(Rule *rule, CompoundACK *ack, int nb_tuples,
                    char windows[nb_tuples][rule->m + 1],
                    char bitmaps[nb_tuples][rule->window_size + 1]) {

    char as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, as_bin);
    char window[rule->m + 1];
    char bitmap[rule->window_size + 1];
    get_ack_w(rule, ack, window);
    get_ack_bitmap(rule, ack, bitmap);
    strncpy(windows[0], window, rule->m + 1);
    strncpy(bitmaps[0], bitmap, rule->window_size + 1);

    char *p = as_bin + rule->ack_indices.tuple_idx;

    for (int i = 1; i <= nb_tuples - 1; i++) {
        strncpy(windows[i], p, rule->m);
        strncpy(bitmaps[i], p + rule->m, rule->window_size);
        windows[i][rule->m] = '\0';
        bitmaps[i][rule->window_size] = '\0';
        p += rule->m + rule->window_size;
    }
}

int is_ack_receiver_abort(Rule *rule, CompoundACK *ack) {
    if (is_ack_null(ack)) return 0;

    char w[rule->m + 1], c[2];
    get_ack_w(rule, ack, w);
    get_ack_c(rule, ack, c);

    if (is_monochar(w, '1') && strcmp(c, "1") == 0) {
        char as_bin[DOWNLINK_MTU_BITS + 1];
        ack_to_bin(ack, as_bin);

        int header_length = rule->ack_header_length;
        char *padding_i = as_bin + header_length;
        char *padding_f = strrchr(as_bin, '1') + 1;
        if (padding_f <= padding_i) return 0;

        int padding_size = (int) (padding_f - padding_i);
        char padding[padding_size + 1];
        strncpy(padding, as_bin + header_length, padding_size);
        padding[padding_size] = '\0';

        int padding_start_size = padding_size - L2_WORD_SIZE;
        char padding_start[padding_start_size + 1];
        char padding_end[L2_WORD_SIZE + 1];
        strncpy(padding_start, padding, padding_start_size);
        strncpy(padding_end, padding + padding_start_size, L2_WORD_SIZE);
        padding_start[padding_start_size] = '\0';
        padding_end[L2_WORD_SIZE] = '\0';

        if (is_monochar(padding_end, '1') &&
                strlen(padding_end) == L2_WORD_SIZE) {
            if (strcmp(padding_start, "") != 0 &&
                    header_length % L2_WORD_SIZE != 0) {
                return is_monochar(padding_start, '1') &&
                        (header_length + padding_start_size) % L2_WORD_SIZE == 0;
            }
            return header_length % L2_WORD_SIZE == 0;
        }
    }
    return 0;
}

int is_ack_compound(Rule *rule, CompoundACK *ack) {
    if (is_ack_null(ack)) return 0;

    char as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, as_bin);
    char *padding = as_bin + rule->ack_header_length;

    return !is_ack_receiver_abort(rule, ack) && !is_monochar(padding, '0');
}

int is_ack_complete(Rule *rule, CompoundACK *ack) {
    if (is_ack_null(ack)) return 0;

    char c[2];
    get_ack_c(rule, ack, c);
    char as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, as_bin);
    char *padding = as_bin + rule->ack_header_length;

    return !is_ack_receiver_abort(rule, ack)
           && strcmp(c, "1") == 0
           && is_monochar(padding, '0');
}

int generate_ack(Rule *rule, CompoundACK *dest,
                 int wdw, char c,
                 char bitmaps[rule->max_window_number][rule->window_size + 1]) {

    char rule_id[rule->rule_id_size + 1];
    char dtag[rule->t + 1];
    char as_bin[DOWNLINK_MTU_BITS + 1];
    get_rule_id_bin(rule, rule_id);
    dtag[0] = '\0';

    memset(as_bin, '0', DOWNLINK_MTU_BITS);
    strncpy(as_bin + rule->ack_indices.rule_id_idx, rule_id,
            rule->rule_id_size);
    strncpy(as_bin + rule->ack_indices.dtag_idx, dtag, rule->t);
    as_bin[rule->ack_indices.c_idx] = c;

    if (c == '1') {
        for (int i = 0; i <= wdw; i++) {
            if (!is_monochar(bitmaps[i], '1')) return -1;
        }

        char window[rule->m + 1];
        int_to_bin(window, wdw, rule->m);
        strncpy(as_bin + rule->ack_indices.w_idx, window, rule->m);
        memset(as_bin + rule->ack_indices.bitmap_idx, '0', rule->window_size);
    } else {
        int tuple_size = rule->m + rule->window_size;
        int tuple_nb = 0;
        int curr_len = 0;

        for (int i = 0;
             i <= wdw && curr_len + tuple_size <= DOWNLINK_MTU_BITS;
             i++) {
            if (bitmaps[i][0] == '\0' || is_monochar(bitmaps[i], '1')) continue;

            char window[rule->m + 1];
            int_to_bin(window, i, rule->m);

            if (tuple_nb == 0) {
                strncpy(as_bin + rule->ack_indices.w_idx, window, rule->m);
                strncpy(as_bin + rule->ack_indices.bitmap_idx, bitmaps[i],
                        rule->window_size);
                curr_len = rule->ack_indices.tuple_idx;
            } else {
                int tuple_idx =
                        rule->ack_indices.tuple_idx +
                        (tuple_nb - 1) * tuple_size;
                strncpy(as_bin + tuple_idx, window, rule->m);
                strncpy(as_bin + tuple_idx + rule->m, bitmaps[i],
                        rule->window_size);
                curr_len += tuple_size;
            }
            tuple_nb++;
        }
    }

    bin_to_bytes(dest->message, as_bin, DOWNLINK_MTU_BITS);
    dest->byte_size = DOWNLINK_MTU_BYTES;
    return 0;
}

void generate_receiver_abort(Rule *rule, CompoundACK *dest) {
    char rule_id[rule->rule_id_size + 1];
    char dtag[rule->t + 1];
    char w[rule->m + 1];
    char c[] = "1";

    get_rule_id_bin(rule, rule_id);
    dtag[0] = '\0';
    memset(w, '1', rule->m);

    int header_remainder = rule->ack_header_length % L2_WORD_SIZE;
    int padding_size = header_remainder == 0
                       ? L2_WORD_SIZE
                       : 2 * L2_WORD_SIZE - header_remainder;
    char padding[padding_size + 1];
    memset(padding, '1', padding_size);
    padding[padding_size] = '\0';

    char ra_bin[DOWNLINK_MTU_BITS + 1];
    memset(ra_bin, '0', DOWNLINK_MTU_BITS);
    ra_bin[DOWNLINK_MTU_BITS] = '\0';

    strncpy(ra_bin + rule->ack_indices.rule_id_idx, rule_id,
            rule->rule_id_size);
    strncpy(ra_bin + rule->ack_indices.dtag_idx, dtag, rule->t);
    strncpy(ra_bin + rule->ack_indices.w_idx, w, rule->m);
    strncpy(ra_bin + rule->ack_indices.c_idx, c, 1);
    strncpy(ra_bin + rule->ack_header_length, padding, padding_size);

    memset(dest, '\0', sizeof(CompoundACK));
    int byte_size = bin_to_bytes(dest->message, ra_bin, DOWNLINK_MTU_BITS);
    dest->byte_size = byte_size;
}

void generate_null_ack(CompoundACK *dest) {
    char message[DOWNLINK_MTU_BYTES];
    memset(message, '\0', DOWNLINK_MTU_BYTES);

    memcpy(dest->message, message, DOWNLINK_MTU_BYTES);
    dest->byte_size = -1;
}

int is_ack_null(CompoundACK *ack) {
    char empty[DOWNLINK_MTU_BYTES];
    memset(empty, '\0', DOWNLINK_MTU_BYTES);

    return memcmp(ack->message, empty, DOWNLINK_MTU_BYTES) == 0
           && ack->byte_size == -1;
}

int ack_equal(CompoundACK *ack1, CompoundACK *ack2) {
    return ack1->byte_size == ack2->byte_size
           && memcmp(ack1->message,
                     ack2->message,
                     ack1->byte_size) == 0;
}