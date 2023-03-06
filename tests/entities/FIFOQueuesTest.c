#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "fifo_queue.h"
#include "misc.h"
#include "fr_procedure.h"

int test_fq_is_empty() {
    int queue_size = 28;
    FIFOQueue q = {0, 0, queue_size, malloc(sizeof(void *) * queue_size)};
    assert(fq_is_empty(&q));
    free(q.objs);

    return 0;
}

int test_fq_write() {
    int queue_size = 28;
    FIFOQueue q = {0, 0, queue_size, malloc(sizeof(void *) * queue_size)};

    Rule rule;
    init_rule(&rule, "000");
    int byte_size = 300;
    char schc_packet[byte_size];
    generate_packet(schc_packet, byte_size);
    int nb_fragments = get_nb_fragments(&rule, byte_size);
    Fragment fragments[nb_fragments];
    assert(fragment(&rule, fragments, schc_packet, byte_size) == 0);

    for (int i = 0; i < queue_size; i++) {
        int wr = fq_write(&q, (void *) &fragments[i]);
        if (i == queue_size - 1) {
            assert(wr == -1);
        } else {
            assert(wr == 0);
        }
    }

    free(q.objs);
    return 0;
}

int test_fq_read() {
    int queue_size = 28;
    FIFOQueue q = {0, 0, queue_size, malloc(sizeof(void *) * queue_size)};

    Rule rule;
    init_rule(&rule, "000");
    int byte_size = 300;
    char schc_packet[byte_size];
    generate_packet(schc_packet, byte_size);
    int nb_fragments = get_nb_fragments(&rule, byte_size);
    Fragment fragments[nb_fragments];

    for (int i = 0; i < queue_size; i++) {
        fq_write(&q, (void *) &fragments[i]);
    }

    for (int j = 0; j < queue_size; j++) {
        void *obj = fq_read(&q);
        if (j == queue_size - 1) {
            assert(obj == NULL);
        } else {
            assert(obj != NULL);
            // Test parse as fragments
        }
    }

    free(q.objs);
    return 0;
}

int main() {
    printf("%d test_fq_is_empty\n", test_fq_is_empty());
    printf("%d test_fq_write\n", test_fq_write());
    printf("%d test_fq_read\n", test_fq_read());

    return 0;
}