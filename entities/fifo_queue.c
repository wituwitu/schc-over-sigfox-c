#include "fifo_queue.h"

int fq_is_empty(FIFOQueue *q) {
    return q->head == q->tail;
}

void *fq_read(FIFOQueue *q) {
    if (fq_is_empty(q)) return NULL;

    Fragment *frg = q->objs[q->tail];
    q->objs[q->tail] = NULL;
    q->tail = (q->tail + 1) % q->size;
    return frg;
}

int fq_write(FIFOQueue *q, void *obj) {
    if (((q->head + 1) % q->size) == q->tail) return -1;

    q->objs[q->head] = obj;
    q->head = (q->head + 1) % q->size;

    return 0;
}
