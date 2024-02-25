#include <stdio.h>
#include <stdlib.h>
#include "bqueue.h"
#include "life.h"

#define NUM_THREADS 4

typedef struct {
	BoundedQueue *cmd_queue;
	BoundedQueue *done_queue;
	Grid *grid;
} Work;

typedef struct {
	unsigned start_row, end_row;
} Task;

void *worker(void *arg) {
	Work *w = arg;

	while (1) {
		Task *t = bqueue_dequeue(w->cmd_queue);
		if (t->end_row == 0) { break; }

		/* do sequential computation */
		life_compute_next(w->grid, t->start_row, t->end_row);

		/* inform main thread that task is done */
		bqueue_enqueue(w->done_queue, t);
	}

	return NULL;
}

void distribute_work(Work *w, int done) {
	unsigned rows_per_thread = (w->grid->nrows - 2) / NUM_THREADS;
	for (unsigned i = 0; i < NUM_THREADS; i++) {
		Task *task = malloc(sizeof(Task));
		if (done) {
			task->end_row = 0;
		} else {
			task->start_row = 1 + (i*rows_per_thread);
			if (i == NUM_THREADS-1) {
				task->end_row = w->grid->nrows - 1;
			} else {
				task->end_row = task->start_row + rows_per_thread;
			}
		}

		bqueue_enqueue(w->cmd_queue, task);
	}
}

void wait_until_done(Work *w) {
	for (unsigned i = 0; i < NUM_THREADS; i++) {
		Task *t = bqueue_dequeue(w->done_queue);
		free(t);
	}
}

/*
 * Parallel game of life program
 * Usage:
 *
 *   ./life_par board_filename num_gens output_filename
 */

int main(int argc, char **argv) {
	if (argc != 4) {
		fatal("Usage: life_par <input filename> <num gens> <output filename>");
	}

	FILE *in = fopen(argv[1], "r");
	if (!in) { fatal("Could not open input file"); }
	Grid *grid = life_read_board(in);
	fclose(in);

	unsigned num_gens = (unsigned) atoi(argv[2]);

	unsigned long start, end;

	start = utime();

	/* start workers */
	Work w;
	w.cmd_queue = bqueue_create(NUM_THREADS);
	w.done_queue = bqueue_create(NUM_THREADS);
	w.grid = grid;
	pthread_t threads[NUM_THREADS];
	for (unsigned i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, worker, &w);
	}

	/* main computation loop */
	for (unsigned i = 0; i < num_gens; i++) {
		distribute_work(&w, 0);
		wait_until_done(&w);
		grid_flip(grid);
	}

	/* tell workers to finish */
	distribute_work(&w, 1);

	/* join worker threads */
	for (unsigned i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	end = utime();

	printf("Computation finished in %lu ms\n", (end - start) / 1000);

	FILE *out = fopen(argv[3], "w");
	if (!out) { fatal("Could not open output file"); }
	life_write_board(grid, out);
	fclose(out);

	return 0;
}
