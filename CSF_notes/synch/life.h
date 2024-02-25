#ifndef LIFE_H
#define LIFE_H

typedef struct {
	unsigned nrows, ncols;
	char *cur_buf, *next_buf;
} Grid;

unsigned long utime(void);
void fatal(const char *msg);

Grid *grid_create(unsigned nrows, unsigned ncols);
void grid_destroy(Grid *grid);
void grid_flip(Grid *grid);

void life_compute_next(Grid *grid, unsigned start_row, unsigned end_row);
Grid *life_create_random(unsigned nrows, unsigned ncols);
Grid *life_read_board(FILE *in);
void life_write_board(Grid *grid, FILE *out);

#endif /* LIFE_H */
