#ifndef BQUEUE_H
#define BQUEUE_H

#include <pthread.h>
#include <semaphore.h>

typedef struct {
	void **data;
	unsigned max_items, head, tail;
	sem_t slots, items;
	pthread_mutex_t lock;
} BoundedQueue;

BoundedQueue *bqueue_create(unsigned max_items);
void bqueue_destroy(BoundedQueue *bq);
void bqueue_enqueue(BoundedQueue *bq, void *item);
void *bqueue_dequeue(BoundedQueue *bq);

#endif /* BQUEUE_H */
