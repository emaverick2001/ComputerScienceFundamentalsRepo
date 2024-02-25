#include <stdio.h>
#include <stdlib.h>
#include "life.h"

/*
 * Sequential game of life program
 * Usage:
 *
 *   ./life_seq board_filename num_gens output_filename
 */

int main(int argc, char **argv) {
	if (argc != 4) {
		fatal("Usage: life_seq <input filename> <num gens> <output filename>");
	}

	FILE *in = fopen(argv[1], "r");
	if (!in) { fatal("Could not open input file"); }
	Grid *grid = life_read_board(in);
	fclose(in);

	unsigned num_gens = (unsigned) atoi(argv[2]);

	unsigned long start, end;

	start = utime();
	for (unsigned i = 0; i < num_gens; i++) {
		life_compute_next(grid, 1, grid->nrows - 1);
		grid_flip(grid);
	}
	end = utime();

	printf("Computation finished in %lu ms\n", (end - start) / 1000);

	FILE *out = fopen(argv[3], "w");
	if (!out) { fatal("Could not open output file"); }
	life_write_board(grid, out);
	fclose(out);

	return 0;
}
