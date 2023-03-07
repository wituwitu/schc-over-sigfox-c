#include <stddef.h>

#ifndef SCHC_OVER_SIGFOX_C_FRAGMENT_QUEUE_H
#define SCHC_OVER_SIGFOX_C_FRAGMENT_QUEUE_H

#endif //SCHC_OVER_SIGFOX_C_FRAGMENT_QUEUE_H

typedef struct {
    size_t head;
    size_t tail;
    size_t size;
    void **objs;
} FIFOQueue;

void fq_construct(FIFOQueue *q, size_t size);

void fq_destroy(FIFOQueue *q);

int fq_is_empty(FIFOQueue *q);

int fq_write(FIFOQueue *q, void *obj);

void *fq_read(FIFOQueue *q);