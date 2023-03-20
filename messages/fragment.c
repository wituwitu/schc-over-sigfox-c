#include "fragment.h"
#include "casting.h"
#include "misc.h"
#include <stdio.h>

void frg_to_bin(Fragment *fragment,
                char dest[fragment->byte_size * 8 + 1]) {
    memset(dest, '\0', UPLINK_MTU_BITS + 1);
    bytes_to_bin(dest, fragment->message, fragment->byte_size);
}

void init_rule_from_frg(Rule *dest, Fragment *fragment) {
    char as_bin[UPLINK_MTU_BITS + 1];
    frg_to_bin(fragment, as_bin);
    init_rule(dest, as_bin);
}

void get_frg_rule_id(Rule *rule, Fragment *fragment,
                     char dest[rule->rule_id_size + 1]) {
    char fragment_as_bin[UPLINK_MTU_BITS + 1];
    frg_to_bin(fragment, fragment_as_bin);
    strncpy(dest, fragment_as_bin + rule->frg_indices.rule_id_idx,
            rule->rule_id_size);
    dest[rule->rule_id_size] = '\0';
}

void get_frg_dtag(Rule *rule, Fragment *fragment, char dest[rule->t + 1]) {
    char fragment_as_bin[UPLINK_MTU_BITS + 1];
    frg_to_bin(fragment, fragment_as_bin);
    strncpy(dest, fragment_as_bin + rule->frg_indices.dtag_idx, rule->t);
    dest[rule->t] = '\0';
}

void get_frg_w(Rule *rule, Fragment *fragment, char dest[rule->m + 1]) {
    char fragment_as_bin[UPLINK_MTU_BITS + 1];
    frg_to_bin(fragment, fragment_as_bin);
    strncpy(dest, fragment_as_bin + rule->frg_indices.w_idx, rule->m);
    dest[rule->m] = '\0';
}

int get_frg_window(Rule *rule, Fragment *frg) {
    char w[rule->m + 1];
    get_frg_w(rule, frg, w);
    return bin_to_int(w);
}

void get_frg_fcn(Rule *rule, Fragment *fragment, char dest[rule->n]) {
    char fragment_as_bin[UPLINK_MTU_BITS + 1];
    frg_to_bin(fragment, fragment_as_bin);
    strncpy(dest, fragment_as_bin + rule->frg_indices.fcn_idx, rule->n);
    dest[rule->n] = '\0';
}

int is_frg_all_0(Rule *rule, Fragment *fragment) {
    if (is_frg_null(fragment)) return 0;

    char fcn[rule->n + 1];
    get_frg_fcn(rule, fragment, fcn);
    return is_monochar(fcn, '0');
}

int is_frg_all_1(Rule *rule, Fragment *fragment) {
    if (is_frg_null(fragment)) return 0;

    char fcn[rule->n + 1];
    get_frg_fcn(rule, fragment, fcn);

    if (!is_monochar(fcn, '1')) return 0;

    int all_1_header_length = rule->all1_header_length;
    int payload_size = UPLINK_MTU_BITS - all_1_header_length;
    char frg_as_bin[UPLINK_MTU_BITS + 1];
    char payload[payload_size + 1];

    frg_to_bin(fragment, frg_as_bin);
    memcpy(payload, frg_as_bin + all_1_header_length, payload_size);
    payload[payload_size] = '\0';

    if (payload[0] != '\0')
        return 1;

    if (strlen(frg_as_bin) == all_1_header_length)
        return 1;

    return 0;
}

int frg_expects_ack(Rule *rule, Fragment *fragment) {
    if (is_frg_null(fragment)) return 0;

    return is_frg_all_0(rule, fragment) ||
           is_frg_all_1(rule, fragment);
}

int is_frg_sender_abort(Rule *rule, Fragment *fragment) {
    if (is_frg_null(fragment)) return 0;

    char w[rule->m + 1], fcn[rule->n + 1];
    get_frg_w(rule, fragment, w);
    get_frg_fcn(rule, fragment, fcn);

    if (!is_monochar(w, '1') || !is_monochar(fcn, '1'))
        return 0;

    char fragment_as_bin[UPLINK_MTU_BITS + 1];
    frg_to_bin(fragment, fragment_as_bin);

    return strlen(fragment_as_bin) < rule->all1_header_length;
}

void get_frg_rcs(Rule *rule, Fragment *fragment, char dest[rule->u + 1]) {
    if (!is_frg_all_1(rule, fragment)) {
        memset(dest, '\0', rule->u + 1);
        return;
    }

    char fragment_as_bin[UPLINK_MTU_BITS + 1];
    frg_to_bin(fragment, fragment_as_bin);
    strncpy(dest, fragment_as_bin + rule->frg_indices.rcs_idx, rule->u);
    dest[rule->u] = '\0';
}

int get_frg_header_byte_size(Rule *rule, Fragment *fragment) {
    return is_frg_all_1(rule, fragment)
           ? rule->all1_header_length / 8
           : rule->header_length / 8;
}

int get_frg_max_payload_byte_size(Rule *rule, Fragment *fragment) {
    return UPLINK_MTU_BYTES - get_frg_header_byte_size(rule, fragment);
}

int get_frg_payload_byte_size(Rule *rule, Fragment *fragment) {
    return fragment->byte_size - get_frg_header_byte_size(rule, fragment);
}

void get_frg_header(Rule *rule, Fragment *fragment, char dest[]) {
    int header_size = get_frg_header_byte_size(rule, fragment);

    memset(dest, '\0', header_size);
    memcpy(dest, fragment->message, header_size);
}

void get_frg_payload(Rule *rule, Fragment *fragment, char dest[]) {
    int header_size = get_frg_header_byte_size(rule, fragment);
    int payload_size = get_frg_payload_byte_size(rule, fragment);

    memset(dest, '\0', payload_size);
    memcpy(dest, fragment->message + header_size, payload_size);
}

void generate_sender_abort(Rule *rule, Fragment *src, Fragment *dest) {
    char rule_id[rule->rule_id_size + 1];
    char dtag[rule->t + 1];
    char w[rule->m + 1];
    char fcn[rule->n + 1];

    get_frg_rule_id(rule, src, rule_id);
    get_frg_dtag(rule, src, dtag);
    memset(w, '1', rule->m);
    memset(fcn, '1', rule->n);

    int sa_size = round_to_next_multiple(rule->frg_indices.rcs_idx,
                                         L2_WORD_SIZE);
    int byte_size = sa_size / 8;

    char sa_bin[sa_size + 1];
    memset(sa_bin, '0', sa_size);
    sa_bin[sa_size] = '\0';

    strncpy(sa_bin + rule->frg_indices.rule_id_idx, rule_id,
            rule->rule_id_size);
    strncpy(sa_bin + rule->frg_indices.dtag_idx, dtag, rule->t);
    strncpy(sa_bin + rule->frg_indices.w_idx, w, rule->m);
    strncpy(sa_bin + rule->frg_indices.fcn_idx, fcn, rule->n);

    memset(dest, '\0', sizeof(Fragment));
    memset(dest->message, '0', byte_size);
    bin_to_bytes(dest->message, sa_bin, sa_size);
    dest->byte_size = byte_size;
}

// TODO: Re-test
int generate_frg(Rule *rule, Fragment *dest, const char payload[],
                 int payload_byte_length, int nb_frag, int all_1) {
    int header_length;
    int window_id;
    char rule_id[rule->rule_id_size + 1];
    char dtag[rule->t + 1];
    char w[rule->m + 1];
    char fcn[rule->n + 1];
    char rcs[rule->u + 1];

    memset(dest, '\0', sizeof(Fragment));

    header_length = all_1 ? rule->all1_header_length : rule->header_length;
    int payload_max_length = UPLINK_MTU_BITS - header_length;
    if (payload_byte_length * 8 > payload_max_length) {
        printf("Payload is larger than its maximum size (%d > %d).\n",
               payload_byte_length * 8, payload_max_length);
        return -1;
    }

    get_rule_id_bin(rule_id, rule);
    dtag[0] = '\0';
    window_id = nb_frag / rule->window_size;
    int_to_bin(w, window_id, rule->m);

    if (all_1) {
        memset(fcn, '1', rule->n);
        int nb_fragments_in_last_window = nb_frag % rule->window_size + 1;
        int_to_bin(rcs, nb_fragments_in_last_window, rule->u);
    } else {
        int frag_idx = rule->window_size - (nb_frag % rule->window_size) - 1;
        int_to_bin(fcn, frag_idx, rule->n);
        rcs[0] = '\0';
    }

    char header_as_bin[header_length + 1];
    memset(header_as_bin, '0', header_length);
    strncpy(header_as_bin + rule->frg_indices.rule_id_idx, rule_id,
            rule->rule_id_size);
    strncpy(header_as_bin + rule->frg_indices.dtag_idx, dtag, rule->t);
    strncpy(header_as_bin + rule->frg_indices.w_idx, w, rule->m);
    strncpy(header_as_bin + rule->frg_indices.fcn_idx, fcn, rule->n);
    if (rcs[0] != '\0') {
        strncpy(header_as_bin + rule->frg_indices.rcs_idx, rcs, rule->u);
    }
    header_as_bin[header_length] = '\0';

    int header_byte_length = header_length / 8;
    int fragment_byte_length = header_byte_length + payload_byte_length;
    char header_as_bytes[header_byte_length + 1];
    bin_to_bytes(header_as_bytes, header_as_bin, header_length);

    char message[UPLINK_MTU_BYTES];
    memset(message, '\0', UPLINK_MTU_BYTES);
    memcpy(message, header_as_bytes, header_byte_length);
    memcpy(message + header_byte_length, payload, payload_byte_length);
    memcpy(dest->message, message, fragment_byte_length);
    dest->byte_size = fragment_byte_length;

    return 0;
}

void generate_null_frg(Fragment *dest) {
    char message[UPLINK_MTU_BYTES];
    memset(message, '\0', UPLINK_MTU_BYTES);

    memcpy(dest->message, message, UPLINK_MTU_BYTES);
    dest->byte_size = -1;
}

int is_frg_null(Fragment *frg) {
    char empty[UPLINK_MTU_BYTES];
    memset(empty, '\0', UPLINK_MTU_BYTES);

    return memcmp(frg->message, empty, UPLINK_MTU_BYTES) == 0
           && frg->byte_size == -1;
}

// TODO: Untested
int get_frg_idx(Rule *rule, Fragment *frg) {
    if (is_frg_all_1(rule, frg)) {
        char rcs[rule->u + 1];
        get_frg_rcs(rule, frg, rcs);
        return bin_to_int(rcs) - 1;
    }

    char fcn[rule->n + 1];
    get_frg_fcn(rule, frg, fcn);
    return rule->window_size - bin_to_int(fcn) - 1;
}

int get_frg_nb(Rule *rule, Fragment *frg) {
    char fcn[rule->n + 1];
    int frg_wdw = get_frg_window(rule, frg);
    int frg_idx = get_frg_idx(rule, frg);
    return rule->window_size * frg_wdw + frg_idx;
}

int frg_equal(Fragment *frg1, Fragment *frg2) {
    return frg1->byte_size == frg2->byte_size
           && memcmp(frg1->message,
                     frg2->message,
                     frg1->byte_size) == 0;
}