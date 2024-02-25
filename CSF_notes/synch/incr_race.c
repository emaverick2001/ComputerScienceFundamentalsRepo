#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int NUM_INCR=100000000, NTHREADS=2;
typedef struct { volatile int count; } Shared;

void *worker(void *arg) {
	Shared *obj = arg;
	for (int i = 0; i < NUM_INCR/NTHREADS; i++)
		obj->count++;
	return NULL;
}

int main(void) {
	Shared *obj = calloc(1, sizeof(Shared));
	pthread_t threads[NTHREADS];
	for (int i = 0; i < NTHREADS; i++)
		pthread_create(&threads[i], NULL, worker, obj);
	for (int i = 0; i < NTHREADS; i++)
		pthread_join(threads[i], NULL);
	printf("%d\n", obj->count);
	return 0;
}
