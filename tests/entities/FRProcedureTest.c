#include <stdio.h>
#include <assert.h>
#include "misc.h"
#include "fr_procedure.h"
#include "casting.h"

/* ------ AUX FUNCTIONS ------ */

void assert_fragmentation(Rule rule, Fragment fragments[], int nb_fragments) {
    for (int i = 0; i < nb_fragments; i++) {
        Fragment fragment_i;
        memcpy(&fragment_i, &fragments[i], sizeof(Fragment));

        if (i % rule.window_size == rule.window_size - 1 &&
            i != nb_fragments - 1) {
            assert(is_frg_all_0(&rule, &fragment_i));
            assert(!is_frg_all_1(&rule, &fragment_i));
        } else if (i == nb_fragments - 1) {
            assert(!is_frg_all_0(&rule, &fragment_i));
            assert(is_frg_all_1(&rule, &fragment_i));
        } else {
            assert(!is_frg_all_0(&rule, &fragment_i));
            assert(!is_frg_all_1(&rule, &fragment_i));
        }
    }
}

void test_fragmentation(Rule rule, int byte_size) {
    char schc_packet[byte_size];
    generate_packet(schc_packet, byte_size);
    int nb_fragments = get_nb_fragments(&rule, byte_size);
    Fragment fragments[nb_fragments];
    assert(fragment(&rule, fragments, schc_packet, byte_size) == 0);
    assert_fragmentation(rule, fragments, nb_fragments);

    char reassembled[byte_size];
    for (int i = 0; i < nb_fragments; i++) {
        get_frg_payload(
                &rule,
                &fragments[i],
                reassembled + i * (rule.regular_payload_len / 8)
        );
    }

    assert(memcmp(reassembled, schc_packet, byte_size) == 0);
}

void test_packet_length(Rule rule, int byte_size) {
    char schc_packet[byte_size + 1];
    generate_packet(schc_packet, byte_size);
    int nb_fragments = get_nb_fragments(&rule, byte_size);
    Fragment fragments[nb_fragments];
    assert(fragment(&rule, fragments, schc_packet, byte_size) == 0);

    Fragment received[rule.max_fragment_nb];
    int i = 0;
    while (i < nb_fragments) {
        memcpy(&(received[i]), &(fragments[i]), sizeof(Fragment));
        i++;
    }
    Fragment null;
    generate_null_frg(&null);
    memcpy(&(received[i]), &null, sizeof(Fragment));

    assert(get_packet_length_from_array(&rule, received) == byte_size);
}

void test_reassembly(Rule rule, int byte_size) {
    char schc_packet[byte_size + 1];
    generate_packet(schc_packet, byte_size);
    int nb_fragments = get_nb_fragments(&rule, byte_size);
    Fragment fragments[nb_fragments];
    assert(fragment(&rule, fragments, schc_packet, byte_size) == 0);

    Fragment received[rule.max_fragment_nb];
    int i = 0;
    while (i < nb_fragments) {
        memcpy(&(received[i]), &(fragments[i]), sizeof(Fragment));
        i++;
    }
    Fragment null;
    generate_null_frg(&null);
    memcpy(&(received[i]), &null, sizeof(Fragment));

    int packet_length = get_packet_length_from_array(&rule, received);

    char reassembled[packet_length];
    assert(reassemble(&rule, reassembled, received) == 0);

    assert(memcmp(reassembled, schc_packet, packet_length) == 0);
}


/* ------ MAIN FUNCTIONS ------ */

int test_get_number_of_fragments() {
    // Single byte header
    Rule rule_single_header;
    init_rule(&rule_single_header, "000");
    int byte_size;

    // 11 bytes
    byte_size = 11;
    assert(get_nb_fragments(&rule_single_header, byte_size) == 2);
    // 100 bytes
    byte_size = 100;
    assert(get_nb_fragments(&rule_single_header, byte_size) == 10);
    // 121 bytes
    byte_size = 121;
    assert(get_nb_fragments(&rule_single_header, byte_size) == 12);
    // 300 bytes
    byte_size = 300;
    assert(get_nb_fragments(&rule_single_header, byte_size) == 28);
    assert(get_nb_fragments(&rule_single_header, byte_size) ==
           rule_single_header.max_fragment_nb);

    // Double byte header op. 1
    Rule rule_two_byte_op_1;
    init_rule(&rule_two_byte_op_1, "111010");

    // 10 bytes
    byte_size = 10;
    assert(get_nb_fragments(&rule_two_byte_op_1, byte_size) == 1);
    // 100 bytes
    byte_size = 100;
    assert(get_nb_fragments(&rule_two_byte_op_1, byte_size) == 10);
    // 121 bytes
    byte_size = 121;
    assert(get_nb_fragments(&rule_two_byte_op_1, byte_size) == 13);
    // 300 bytes
    byte_size = 300;
    assert(get_nb_fragments(&rule_two_byte_op_1, byte_size) == 30);
    // 480 bytes
    byte_size = 480;
    assert(get_nb_fragments(&rule_two_byte_op_1, byte_size) == 48);
    assert(get_nb_fragments(&rule_two_byte_op_1, byte_size) ==
           rule_two_byte_op_1.max_fragment_nb);

    // Double byte header op. 2
    Rule rule_two_byte_op_2;
    init_rule(&rule_two_byte_op_2, "11111110");

    // 10 bytes
    byte_size = 10;
    assert(get_nb_fragments(&rule_two_byte_op_2, byte_size) == 2);
    // 100 bytes
    byte_size = 100;
    assert(get_nb_fragments(&rule_two_byte_op_2, byte_size) == 11);
    // 121 bytes
    byte_size = 121;
    assert(get_nb_fragments(&rule_two_byte_op_2, byte_size) == 13);
    // 300 bytes
    byte_size = 300;
    assert(get_nb_fragments(&rule_two_byte_op_2, byte_size) == 31);
    // 480 bytes
    byte_size = 480;
    assert(get_nb_fragments(&rule_two_byte_op_2, byte_size) == 49);
    // 2400 bytes
    byte_size = 2400;
    assert(get_nb_fragments(&rule_two_byte_op_2, byte_size) == 241);
    assert(get_nb_fragments(&rule_two_byte_op_2, byte_size) !=
           rule_two_byte_op_2.max_fragment_nb);

    return 0;
}

int test_get_number_of_windows() {

    // Single byte header
    Rule rule_single_header;
    init_rule(&rule_single_header, "000");
    int byte_size;

    // 11 bytes
    byte_size = 11;
    assert(get_nb_windows(&rule_single_header, byte_size) == 1);
    // 100 bytes
    byte_size = 100;
    assert(get_nb_windows(&rule_single_header, byte_size) == 2);
    // 121 bytes
    byte_size = 121;
    assert(get_nb_windows(&rule_single_header, byte_size) == 2);
    // 300 bytes
    byte_size = 300;
    assert(get_nb_windows(&rule_single_header, byte_size) == 4);
    assert(get_nb_windows(&rule_single_header, byte_size) ==
           rule_single_header.max_window_nb);

    // Double byte header op. 1
    Rule rule_two_byte_op_1;
    init_rule(&rule_two_byte_op_1, "111010");

    // 10 bytes
    byte_size = 10;
    assert(get_nb_windows(&rule_two_byte_op_1, byte_size) == 1);
    // 100 bytes
    byte_size = 100;
    assert(get_nb_windows(&rule_two_byte_op_1, byte_size) == 1);
    // 121 bytes
    byte_size = 121;
    assert(get_nb_windows(&rule_two_byte_op_1, byte_size) == 2);
    // 300 bytes
    byte_size = 300;
    assert(get_nb_windows(&rule_two_byte_op_1, byte_size) == 3);
    // 480 bytes
    byte_size = 480;
    assert(get_nb_windows(&rule_two_byte_op_1, byte_size) == 4);
    assert(get_nb_windows(&rule_two_byte_op_1, byte_size) ==
           rule_two_byte_op_1.max_window_nb);

    // Double byte header op. 2
    Rule rule_two_byte_op_2;
    init_rule(&rule_two_byte_op_2, "11111110");

    // 10 bytes
    byte_size = 10;
    assert(get_nb_windows(&rule_two_byte_op_2, byte_size) == 1);
    // 100 bytes
    byte_size = 100;
    assert(get_nb_windows(&rule_two_byte_op_2, byte_size) == 1);
    // 121 bytes
    byte_size = 121;
    assert(get_nb_windows(&rule_two_byte_op_2, byte_size) == 1);
    // 300 bytes
    byte_size = 300;
    assert(get_nb_windows(&rule_two_byte_op_2, byte_size) == 1);
    // 480 bytes
    byte_size = 480;
    assert(get_nb_windows(&rule_two_byte_op_2, byte_size) == 2);
    // 2400 bytes
    byte_size = 2400;
    assert(get_nb_windows(&rule_two_byte_op_2, byte_size) == 8);
    assert(get_nb_windows(&rule_two_byte_op_2, byte_size) ==
           rule_two_byte_op_2.max_window_nb);

    return 0;
}

int test_fragment() {

    // Single byte header
    Rule rule_single_header;
    init_rule(&rule_single_header, "000");
    // 11 bytes
    test_fragmentation(rule_single_header, 11);
    // 100 bytes
    test_fragmentation(rule_single_header, 100);
    // 121 bytes
    test_fragmentation(rule_single_header, 121);
    // 300 bytes
    test_fragmentation(rule_single_header, 300);

    // Double byte header op. 1
    Rule rule_two_byte_op_1;
    init_rule(&rule_two_byte_op_1, "111010");
    // 10 bytes
    test_fragmentation(rule_two_byte_op_1, 10);
    // 100 bytes
    test_fragmentation(rule_two_byte_op_1, 100);
    // 121 bytes
    test_fragmentation(rule_two_byte_op_1, 121);
    // 300 bytes
    test_fragmentation(rule_two_byte_op_1, 300);
    // 480 bytes
    test_fragmentation(rule_two_byte_op_1, 480);

    // Double byte header op. 2
    Rule rule_two_byte_op_2;
    init_rule(&rule_two_byte_op_2, "11111110");
    // 10 bytes
    test_fragmentation(rule_two_byte_op_2, 10);
    // 100 bytes
    test_fragmentation(rule_two_byte_op_2, 100);
    // 121 bytes
    test_fragmentation(rule_two_byte_op_2, 121);
    // 300 bytes
    test_fragmentation(rule_two_byte_op_2, 300);
    // 480 bytes
    test_fragmentation(rule_two_byte_op_2, 480);
    // 2400 bytes
    test_fragmentation(rule_two_byte_op_2, 2400);

    // Errors
    Rule rule = rule_single_header;

    // Byte size 0
    int byte_0 = 0;
    char schc_0[byte_0];
    generate_packet(schc_0, byte_0);
    int nb_0 = get_nb_fragments(&rule, byte_0);
    Fragment frag_0[nb_0];
    assert(fragment(&rule, frag_0, schc_0, byte_0) < 0);

    // Byte size longer than limit
    int byte_1000 = 1000;
    char schc_1000[byte_1000];
    generate_packet(schc_1000, byte_1000);
    int nb_1000 = get_nb_fragments(&rule, byte_1000);
    Fragment frag_1000[nb_1000];
    assert(fragment(&rule, frag_1000, schc_1000, byte_1000) < 0);

    return 0;
}

int test_get_packet_length_from_array() {
    // Single byte header
    Rule rule_single_header;
    init_rule(&rule_single_header, "000");
    // 11 bytes
    test_packet_length(rule_single_header, 11);
    // 100 bytes
    test_packet_length(rule_single_header, 100);
    // 121 bytes
    test_packet_length(rule_single_header, 121);
    // 131 bytes
    test_packet_length(rule_single_header, 131);
    // 300 bytes
    test_packet_length(rule_single_header, 300);

    // Double byte header op. 1
    Rule rule_two_byte_op_1;
    init_rule(&rule_two_byte_op_1, "111010");
    // 10 bytes
    test_packet_length(rule_two_byte_op_1, 10);
    // 100 bytes
    test_packet_length(rule_two_byte_op_1, 100);
    // 121 bytes
    test_packet_length(rule_two_byte_op_1, 121);
    // 131 bytes
    test_packet_length(rule_two_byte_op_1, 131);
    // 300 bytes
    test_packet_length(rule_two_byte_op_1, 300);
    // 480 bytes
    test_packet_length(rule_two_byte_op_1, 480);

    // Double byte header op. 2
    Rule rule_two_byte_op_2;
    init_rule(&rule_two_byte_op_2, "11111110");
    // 10 bytes
    test_packet_length(rule_two_byte_op_2, 10);
    // 100 bytes
    test_packet_length(rule_two_byte_op_2, 100);
    // 121 bytes
    test_packet_length(rule_two_byte_op_2, 121);
    // 131 bytes
    test_packet_length(rule_two_byte_op_2, 131);
    // 300 bytes
    test_packet_length(rule_two_byte_op_2, 300);
    // 480 bytes
    test_packet_length(rule_two_byte_op_2, 480);
    // 2400 bytes
    test_packet_length(rule_two_byte_op_2, 2400);

    return 0;
}

int test_reassemble() {

    // Single byte header
    Rule rule_single_header;
    init_rule(&rule_single_header, "000");
    // 11 bytes
    test_reassembly(rule_single_header, 11);
    // 100 bytes
    test_reassembly(rule_single_header, 100);
    // 121 bytes
    test_reassembly(rule_single_header, 121);
    // 300 bytes
    test_reassembly(rule_single_header, 300);

    // Double byte header op. 1
    Rule rule_two_byte_op_1;
    init_rule(&rule_two_byte_op_1, "111010");
    // 10 bytes
    test_reassembly(rule_two_byte_op_1, 10);
    // 100 bytes
    test_reassembly(rule_two_byte_op_1, 100);
    // 121 bytes
    test_reassembly(rule_two_byte_op_1, 121);
    // 300 bytes
    test_reassembly(rule_two_byte_op_1, 300);
    // 480 bytes
    test_reassembly(rule_two_byte_op_1, 480);

    // Double byte header op. 2
    Rule rule_two_byte_op_2;
    init_rule(&rule_two_byte_op_2, "11111110");
    // 10 bytes
    test_reassembly(rule_two_byte_op_2, 10);
    // 100 bytes
    test_reassembly(rule_two_byte_op_2, 100);
    // 121 bytes
    test_reassembly(rule_two_byte_op_2, 121);
    // 300 bytes
    test_reassembly(rule_two_byte_op_2, 300);
    // 480 bytes
    test_reassembly(rule_two_byte_op_2, 480);
    // 2400 bytes
    test_reassembly(rule_two_byte_op_2, 2400);

    return 0;
}

int main() {
    printf("%d test_get_number_of_fragments\n", test_get_number_of_fragments());
    printf("%d test_get_number_of_windows\n", test_get_number_of_windows());
    printf("%d test_fragment\n", test_fragment());
    printf("%d test_get_packet_length_from_array\n",
           test_get_packet_length_from_array());
    printf("%d test_reassemble\n", test_reassemble());

    return 0;
}