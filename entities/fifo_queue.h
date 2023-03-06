#include <stddef.h>
#include "fragment.h"

#ifndef SCHC_OVER_SIGFOX_C_FRAGMENT_QUEUE_H
#define SCHC_OVER_SIGFOX_C_FRAGMENT_QUEUE_H

#endif //SCHC_OVER_SIGFOX_C_FRAGMENT_QUEUE_H

typedef struct {
    size_t head;
    size_t tail;
    size_t size;
    void **objs;
} FIFOQueue;

int fq_is_empty(FIFOQueue *q);

void *fq_read(FIFOQueue *q);

int fq_write(FIFOQueue *q, void *obj);