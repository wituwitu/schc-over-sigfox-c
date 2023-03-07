#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "fifo_queue.h"
#include "misc.h"
#include "fr_procedure.h"
#include "casting.h"

int test_fq_construct() {
    int queue_size = 10;
    FIFOQueue q;
    fq_construct(&q, queue_size);
    assert(q.head == 0);
    assert(q.tail == 0);
    assert(q.size == queue_size);
    assert(q.objs != NULL);
    assert(sizeof(q.objs[0]) * queue_size == sizeof(void *) * queue_size);

    free(q.objs);
    q.objs = 0;

    return 0;
}

int test_fq_destroy() {
    int queue_size = 10;
    FIFOQueue q;
    fq_construct(&q, queue_size);
    fq_destroy(&q);
    assert(q.objs == 0);

    return 0;
}

int test_fq_write() {
    /* ------- with pointers to int ------- */
    int queue_size = 20; // 19 slots for objects
    FIFOQueue q;
    fq_construct(&q, queue_size);

    int i_array[queue_size];
    for (int i = 0; i < queue_size; i++) {
        i_array[i] = i;
    }

    for (int i = 0; i < queue_size; i++) {
        int wr = fq_write(&q, (void *) &i_array[i]);
        if (i == queue_size - 1) {
            assert(wr == -1);
            assert(q.tail == 0);
            assert(q.objs[q.head] == NULL);
        } else {
            assert(wr == 0);
            assert(q.head == i + 1);
            assert(q.tail == 0);
        }
    }

    for (int i = 0; i < queue_size - 1; i++) {
        int *p = (int *) q.objs[i];
        assert(i == *p);
    }

    fq_destroy(&q);

    /* ------- with pointers to Fragment ------- */

    int byte_size = 300;
    Rule rule;
    init_rule(&rule, "000");
    char schc_packet[byte_size];
    generate_packet(schc_packet, byte_size);
    int nb_fragments = get_nb_fragments(&rule, byte_size);
    Fragment fragments[nb_fragments];
    assert(fragment(&rule, fragments, schc_packet, byte_size) == 0);

    int fq_size = nb_fragments + 1;
    FIFOQueue fq;
    fq_construct(&fq, fq_size);

    for (int i = 0; i < nb_fragments; i++) {
        int wr = fq_write(&fq, (void *) &fragments[i]);
        assert(wr == 0);
        assert(fq.head == i + 1);
        assert(fq.tail == 0);
    }

    for (int i = 0; i < nb_fragments; i++) {
        Fragment *p = (Fragment *) fq.objs[i];
        assert(memcmp(fragments[i].message, p->message,
                      fragments[i].byte_size) == 0);
        assert(fragments[i].byte_size == p->byte_size);
    }
    assert(is_frg_all_1(&rule, (Fragment *) fq.objs[nb_fragments - 1]));

    fq_destroy(&fq);

    return 0;
}

int test_fq_read() {
    /* ------- with pointers to int ------- */
    int int_arr_len = 20;
    int queue_size = int_arr_len + 1;
    FIFOQueue q;
    fq_construct(&q, queue_size);

    int i_array[int_arr_len + 1];
    for (int i = 0; i < int_arr_len; i++) {
        i_array[i] = i;
    }

    for (int i = 0; i < int_arr_len; i++) {
        assert(fq_write(&q, (void *) &i_array[i]) == 0);
    }

    for (int i = 0; i < queue_size; i++) {
        void *obj = fq_read(&q);
        if (i == queue_size - 1) {
            assert(obj == NULL);
        } else {
            assert(q.objs[q.tail - 1] == NULL);
            assert(q.tail - 1 == i);
            assert(i == *(int *) obj);
        }
    }

    fq_destroy(&q);

    /* ------- with pointers to Fragment ------- */

    int byte_size = 300;
    Rule rule;
    init_rule(&rule, "000");
    char schc_packet[byte_size];
    generate_packet(schc_packet, byte_size);
    int nb_fragments = get_nb_fragments(&rule, byte_size);
    Fragment fragments[nb_fragments];
    assert(fragment(&rule, fragments, schc_packet, byte_size) == 0);

    int fq_size = nb_fragments + 1;
    FIFOQueue fq;
    fq_construct(&fq, fq_size);

    for (int i = 0; i < nb_fragments; i++) {
        assert(fq_write(&fq, (void *) &fragments[i]) == 0);
    }

    Fragment *p;
    for (int i = 0; i < nb_fragments; i++) {
        p = (Fragment *) fq_read(&fq);
        assert(memcmp(fragments[i].message, p->message,
                      fragments[i].byte_size) == 0);
        assert(fragments[i].byte_size == p->byte_size);
    }
    assert(is_frg_all_1(&rule, p));

    fq_destroy(&fq);

    return 0;
}

int test_fq_is_empty() {
    int queue_size = 28;
    FIFOQueue q;
    fq_construct(&q, queue_size);
    assert(fq_is_empty(&q));

    int a = 1;
    fq_write(&q, (void *) &a);
    assert(!fq_is_empty(&q));

    int *p = (int *) fq_read(&q);
    assert(*p == a);
    assert(fq_is_empty(&q));

    fq_destroy(&q);

    return 0;
}

int test_routine() {
    int int_arr_len = 20;
    int int_arr[int_arr_len + 1];
    for (int i = 0; i < int_arr_len + 1; i++) {
        int_arr[i] = i;
    }

    int queue_size = 10;
    FIFOQueue q;
    fq_construct(&q, queue_size);

    assert(fq_is_empty(&q));

    int i = 0;

    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i]) == -1);
    assert(i == 9);

    assert(*(int *) fq_read(&q) == 0);
    assert(*(int *) fq_read(&q) == 1);
    assert(*(int *) fq_read(&q) == 2);
    assert(*(int *) fq_read(&q) == 3);
    assert(*(int *) fq_read(&q) == 4);

    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i]) == -1);
    assert(i == 14);

    assert(*(int *) fq_read(&q) == 5);
    assert(*(int *) fq_read(&q) == 6);
    assert(*(int *) fq_read(&q) == 7);
    assert(*(int *) fq_read(&q) == 8);
    assert(*(int *) fq_read(&q) == 9);
    assert(*(int *) fq_read(&q) == 10);
    assert(*(int *) fq_read(&q) == 11);
    assert(*(int *) fq_read(&q) == 12);

    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(fq_write(&q, &int_arr[i++]) == 0);
    assert(i == 21);

    assert(*(int *) fq_read(&q) == 13);
    assert(*(int *) fq_read(&q) == 14);
    assert(*(int *) fq_read(&q) == 15);
    assert(*(int *) fq_read(&q) == 16);
    assert(*(int *) fq_read(&q) == 17);
    assert(*(int *) fq_read(&q) == 18);
    assert(*(int *) fq_read(&q) == 19);
    assert(*(int *) fq_read(&q) == 20);
    assert(fq_read(&q) == NULL);

    assert(fq_is_empty(&q));

    fq_destroy(&q);

    return 0;
}

int main() {
    printf("%d test_fq_construct\n", test_fq_construct());
    printf("%d test_fq_destroy\n", test_fq_destroy());
    printf("%d test_fq_write\n", test_fq_write());
    printf("%d test_fq_read\n", test_fq_read());
    printf("%d test_fq_is_empty\n", test_fq_is_empty());
    printf("%d test_routine\n", test_routine());

    return 0;
}