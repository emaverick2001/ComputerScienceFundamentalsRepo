#include <stdlib.h>
#include "bqueue.h"

BoundedQueue *bqueue_create(unsigned max_items) {
	BoundedQueue *bq = malloc(sizeof(BoundedQueue));
	bq->data = malloc(max_items * sizeof(void *));
	bq->max_items = max_items;
	bq->head = bq->tail = 0;
	sem_init(&bq->slots, 0, max_items);
	sem_init(&bq->items, 0, 0);
	pthread_mutex_init(&bq->lock, NULL);
	return bq;
}

void bqueue_destroy(BoundedQueue *bq) {
	pthread_mutex_destroy(&bq->lock);
	sem_destroy(&bq->items);
	sem_destroy(&bq->slots);
	free(bq->data);
	free(bq);
}

void bqueue_enqueue(BoundedQueue *bq, void *item) {
	sem_wait(&bq->slots); /* wait for empty slot */

	pthread_mutex_lock(&bq->lock);
	bq->data[bq->head] = item;
	bq->head = (bq->head + 1) % bq->max_items;
	pthread_mutex_unlock(&bq->lock);

	sem_post(&bq->items); /* item is available */
}

void *bqueue_dequeue(BoundedQueue *bq) {
	sem_wait(&bq->items); /* wait for item */

	pthread_mutex_lock(&bq->lock);
	void *item = bq->data[bq->tail];
	bq->tail = (bq->tail + 1) % bq->max_items;
	pthread_mutex_unlock(&bq->lock);

	sem_post(&bq->slots); /* empty slot is available */

	return item;
}
