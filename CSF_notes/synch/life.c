#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "life.h"

unsigned long utime(void) {
	struct timeval tv;
	unsigned long result = 0;

	gettimeofday(&tv, NULL);
	result += (tv.tv_sec * 1000000);
	result += tv.tv_usec;

	return result;
}

void fatal(const char *msg) {
	fprintf(stderr, "Error: %s\n", msg);
	exit(1);
}

Grid *grid_create(unsigned nrows, unsigned ncols) {
	Grid *grid = malloc(sizeof(Grid));
	grid->nrows = nrows;
	grid->ncols = ncols;
	grid->cur_buf = calloc(nrows * ncols, sizeof(char));
	grid->next_buf = calloc(nrows * ncols, sizeof(char));
	return grid;
}

void grid_destroy(Grid *grid) {
	free(grid->cur_buf);
	free(grid->next_buf);
	free(grid);
}

void grid_flip(Grid *grid) {
	char *tmp = grid->cur_buf;
	grid->cur_buf = grid->next_buf;
	grid->next_buf = tmp;
}

unsigned life_count_neighbors(Grid *grid, unsigned row, unsigned col) {
	unsigned count = 0;
	unsigned idx = row * grid->ncols + col;
	count += grid->cur_buf[idx - grid->ncols - 1];
	count += grid->cur_buf[idx - grid->ncols];
	count += grid->cur_buf[idx - grid->ncols + 1];
	count += grid->cur_buf[idx - 1];
	count += grid->cur_buf[idx + 1];
	count += grid->cur_buf[idx + grid->ncols - 1];
	count += grid->cur_buf[idx + grid->ncols];
	count += grid->cur_buf[idx + grid->ncols + 1];
	return count;
}

void life_compute_next(Grid *grid, unsigned start_row, unsigned end_row) {
	/* don't update border rows/columns */
	if (start_row == 0) { start_row++; }
	if (end_row >= grid->nrows) { end_row = grid->nrows - 1; }

	/* compute new cell values */
	for (unsigned i = start_row; i < end_row; i++) {
		for (unsigned j = 1; j < grid->ncols - 1; j++) {
			unsigned neighbor_count = life_count_neighbors(grid, i, j);
			unsigned idx = i * grid->ncols + j;
			if (grid->cur_buf[idx] && (neighbor_count == 2 || neighbor_count == 3)) {
				grid->next_buf[idx] = 1; /* cell survives */
			} else if (!grid->cur_buf[idx] && neighbor_count == 3) {
				grid->next_buf[idx] = 1; /* cell is born */
			} else {
				grid->next_buf[idx] = 0;
			}
		}
	}
}

Grid *life_create_random(unsigned nrows, unsigned ncols) {
	Grid *grid = grid_create(nrows, ncols);
	for (unsigned i = 1; i < nrows-1; i++) {
		for (unsigned j = 1; j < ncols-1; j++) {
			unsigned idx = i*ncols + j;
			grid->cur_buf[idx] = rand() & 1;
		}
	}
	return grid;
}

Grid *life_read_board(FILE *in) {
	unsigned nrows, ncols;
	if (fscanf(in, "%u %u", &nrows, &ncols) != 2) { fatal("Bad input"); }
	Grid *grid = grid_create(nrows, ncols);
	for (unsigned i = 0; i < nrows*ncols; i++) {
		unsigned val;
		if (fscanf(in, "%u", &val) != 1) { fatal("Bad input"); }
		grid->cur_buf[i] = val;
	}
	return grid;
}

void life_write_board(Grid *grid, FILE *out) {
	fprintf(out, "%u %u", grid->nrows, grid->ncols);
	for (unsigned i = 0; i < grid->nrows * grid->ncols; i++) {
		if (i % grid->ncols == 0) { fprintf(out, "\n"); }
		fprintf(out, "%u ", (unsigned) grid->cur_buf[i]);
	}
	fprintf(out, "\n");
		
}
