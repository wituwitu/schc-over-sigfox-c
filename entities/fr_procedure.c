#include <stdio.h>
#include <math.h>
#include "fr_procedure.h"


int get_number_of_fragments(Rule *rule, int byte_size) {
    int res;
    int payload_byte_size = rule->regular_payload_length / 8;
    res = (int) -floor((double) byte_size / -payload_byte_size);

    if (rule->header_length < rule->all1_header_length
        && byte_size % payload_byte_size == 0) {
        res += 1;
    }

    return res;
}

int
fragment(Rule *rule, Fragment dest[], const char schc_packet[], int byte_size) {
    if (byte_size > rule->max_schc_packet_byte_size) {
        printf("SCHC Packet is larger than allowed "
               "by Rule %d (%d > %d).",
               rule->id, byte_size, rule->max_schc_packet_byte_size);
        return -1;
    }

    int payload_byte_size = rule->regular_payload_length / 8;
    int nb_fragments = get_number_of_fragments(rule, byte_size);

    int all_1, payload_size;
    for (int i = 0; i < nb_fragments; i++) {
        all_1 = i == nb_fragments - 1;

        payload_size = all_1
                       ? byte_size % payload_byte_size
                       : payload_byte_size;

        char payload[payload_size + 1];
        memcpy(payload, schc_packet + i * payload_size, payload_size);
        payload[payload_size] = '\0';

        Fragment fragment;
        generate_fragment(rule, &fragment, payload, payload_size, i, all_1);
        memcpy(&dest[i], &fragment, sizeof(Fragment));
    }

    return 0;
}