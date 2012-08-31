/**
 * Entry point for the my mini lisp interpreter
 */

// Standard headers
#include <stdio.h>
#include <stdlib.h>

// Project headers
#include "lisp.h"

/**
 * Loads in the program, calls the parser, evaluates the code, and then prints the output
 */
int main(int argc, char **argv) {
	struct lisp_env *env;

	// Initialize the lisp environment, then dump the defined symbols and call it a day
	env = lisp_init();

	return 0;
}
