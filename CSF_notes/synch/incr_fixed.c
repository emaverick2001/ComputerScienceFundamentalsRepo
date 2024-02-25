#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int NUM_INCR=100000000, NTHREADS=2;
typedef struct { volatile int count; pthread_mutex_t lock; } Shared;

void *worker(void *arg) {
	Shared *obj = arg;
	for (int i = 0; i < NUM_INCR/NTHREADS; i++) {
		pthread_mutex_lock(&obj->lock);
		obj->count++;
		pthread_mutex_unlock(&obj->lock);
	}
	return NULL;
}

int main(void) {
	Shared *obj = calloc(1, sizeof(Shared));
	pthread_mutex_init(&obj->lock, NULL);
	pthread_t threads[NTHREADS];
	for (int i = 0; i < NTHREADS; i++)
		pthread_create(&threads[i], NULL, worker, obj);
	for (int i = 0; i < NTHREADS; i++)
		pthread_join(threads[i], NULL);
	printf("%d\n", obj->count);
	pthread_mutex_destroy(&obj->lock);
	return 0;
}
