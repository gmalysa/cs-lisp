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
	struct s_exp *result;
	struct lisp_env *env;

	// Initialize the lisp environment, then dump the defined symbols and call it a day
	env = lisp_init();

	// Open our test source file
	fp = fopen("test.lisp", "r");
	if (fp == NULL) {
		perror("Unable to open test file: test.lisp");
		return 1;
	}

	// Parse it into a list of s-expressions
	expList = lisp_parse_file(fp);

	// Pretty print the expressions back to the console to show that we parsed them properly
	while (expList != 0) {
		pretty_print_exp(expList->exp);
		printf("\n");
		
		result = eval(expList->exp, env);
		printf("eval() result: ");
		pretty_print_exp(result);
		printf("\n\n");
		expList = expList->next;
	}

	// TODO: Clean up environment

	return 0;
}
