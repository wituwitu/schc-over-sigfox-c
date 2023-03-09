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

/*
 * Function:  fq_construct
 * --------------------
 * Initializes a FIFOQueue struct, allocating memory for the objs pointer.
 *
 *  q: FIFOQueue structure to be initialized.
 *  size: Size of the queue (nb. of elements + 1).
 */
void fq_construct(FIFOQueue *q, size_t size);

/*
 * Function:  fq_destroy
 * --------------------
 * Frees the allocated memory for the FIFOQueue.
 *
 *  q: FIFOQueue structure to be destroyed.
 */
void fq_destroy(FIFOQueue *q);

/*
 * Function:  fq_is_empty
 * --------------------
 * Checks whether the FIFOQueue is empty.
 *
 *  q: FIFOQueue structure to be checked.
 */
int fq_is_empty(FIFOQueue *q);

/*
 * Function:  fq_put
 * --------------------
 * Inserts an element into the FIFOQueue.
 * Returns -1 if the queue is full.
 *
 *  q: FIFOQueue structure to be processed.
 *  obj: void pointer to the object to be queued.
 */
int fq_put(FIFOQueue *q, void *obj);

/*
 * Function:  fq_get
 * --------------------
 * Removes the element pointed by the tail pointer of the queue.
 * Returns its address as a void pointer, or NULL if the queue is empty.
 *
 *  q: FIFOQueue structure to be processed.
 */
void *fq_get(FIFOQueue *q);