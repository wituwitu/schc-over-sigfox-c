#include <malloc.h>
#include "fifo_queue.h"

void fq_construct(FIFOQueue *q, size_t size) {
    q->head = 0;
    q->tail = 0;
    q->size = size;
    q->objs = malloc(sizeof(void *) * size);
}

void fq_destroy(FIFOQueue *q) {
    free(q->objs);
    q->objs = 0;
}

int fq_is_empty(FIFOQueue *q) {
    return q->head == q->tail;
}

int fq_put(FIFOQueue *q, void *obj) {
    if (((q->head + 1) % q->size) == q->tail) {
        printf("Queue full.\n");
        return -1;
    }

    q->objs[q->head] = obj;
    q->head = (q->head + 1) % q->size;

    return 0;
}

void *fq_get(FIFOQueue *q) {
    if (fq_is_empty(q)) {
        printf("Queue empty.\n");
        return NULL;
    }

    void *obj = q->objs[q->tail];
    q->objs[q->tail] = NULL;
    q->tail = (q->tail + 1) % q->size;
    return obj;
}