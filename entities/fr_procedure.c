#include <stdio.h>
#include <math.h>
#include "fr_procedure.h"
#include "casting.h"


int get_nb_fragments(Rule *rule, int byte_size) {
    int res;
    int payload_byte_size = rule->regular_payload_length / 8;
    res = (int) -floor((double) byte_size / -payload_byte_size);

    if (rule->header_length < rule->all1_header_length
        && byte_size % payload_byte_size == 0) {
        res += 1;
    }

    return res;
}

int get_nb_windows(Rule *rule, int byte_size) {
    int nb_fragments = get_nb_fragments(rule, byte_size);
    return ceil((double) nb_fragments / rule->window_size);
}

int fragment(Rule *rule, Fragment dest[],
             const char schc_packet[], int byte_size) {
    if (byte_size > rule->max_schc_packet_byte_size) {
        printf("SCHC Packet is larger than allowed "
               "by Rule %d (%d > %d).\n",
               rule->id, byte_size, rule->max_schc_packet_byte_size);
        return -1;
    }

    if (byte_size == 0) {
        printf("Cannot fragment a SCHC Packet of size 0.\n");
        return -1;
    }

    int reg_payload_byte_size = rule->regular_payload_length / 8;
    int nb_fragments = get_nb_fragments(rule, byte_size);

    int all_1, payload_size;
    for (int i = 0; i < nb_fragments; i++) {
        all_1 = i == nb_fragments - 1;

        if (all_1) {
            payload_size = rule->all1_header_length > rule->header_length &&
                           byte_size % reg_payload_byte_size == 0
                           ? 0
                           : (byte_size - 1) % reg_payload_byte_size + 1;
        } else {
            payload_size = reg_payload_byte_size;
        }

        char payload[payload_size];
        memcpy(payload, schc_packet + i * reg_payload_byte_size, payload_size);

        Fragment fragment;
        generate_frg(rule, &fragment, payload, payload_size, i, all_1);
        memcpy(&dest[i], &fragment, sizeof(Fragment));
    }

    return 0;
}

int get_packet_length_from_array(Rule *rule, Fragment *fragments) {
    int i = 0;
    while (!is_frg_null(&fragments[i])) {
        if (i == rule->max_fragment_number) {
            printf("No null Fragment was found in the array.");
            return -1;
        }
        if (is_frg_all_1(rule, &fragments[i])) {
            return (rule->regular_payload_length / 8) * i
                   + get_frg_payload_byte_size(rule, &fragments[i]);
        }
        i++;
    }
}

int reassemble(Rule *rule, char dest[], Fragment fragments[]) {
    for (int i = 0; !is_frg_null(&fragments[i]); i++) {
        if (i == rule->max_fragment_number) {
            printf("No null Fragment was found in the array.");
            return -1;
        }
        get_frg_payload(rule, &fragments[i],
                        dest + i * (rule->regular_payload_length / 8));
    }
    return 0;
}