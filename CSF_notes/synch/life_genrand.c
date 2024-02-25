#include <stdio.h>
#include "life.h"

#define NROWS 1000
#define NCOLS 1000

/*
 * Generate a large random life board to standard output.
 * Usage:
 *
 *   ./life_genrand > board.txt
 */

int main(void) {
	Grid *grid = life_create_random(NROWS, NCOLS);
	life_write_board(grid, stdout);
	return 0;
}
