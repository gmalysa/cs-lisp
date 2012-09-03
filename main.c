/**
 * Entry point for the my mini lisp interpreter
 */

// Standard headers
#include <stdio.h>
#include <stdlib.h>

// Project headers
#include "lisp.h"
#include "lisp_parser.h"

/**
 * Loads in the program, calls the parser, evaluates the code, and then prints the output
 */
int main(int argc, char **argv) {
	FILE *fp;
	struct s_list *expList;
	struct lisp_env *env;

	// Initialize the lisp environment, then dump the defined symbols and call it a day
	env = lisp_init();

	// Open our test source file
	fp = fopen("test.l", "r");
	if (fp == NULL) {
		perror("Unable to open test file: test.l");
		return 1;
	}

	// Parse it into a list of s-expressions
	expList = lisp_parse_file(fp);

	// Pretty print the expressions back to the console to show that we parsed them properly
	while (expList != 0) {
		pretty_print_exp(expList->exp);
		expList = expList->next;
	}

	return 0;
}
