/**
 * Routines for parsing strings and files into s-expressions
 */

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

// Project definitions
#include "lisp.h"
#include "lisp_parser.h"

// Maximum line buffer size
#define LINE_BUFFER_SIZE	1024

/**
 * Parses a file into a series of top-level S-expressions, emitting each one to the given
 * callback as it is completed. This should operate one line at a time.
 */
void lisp_parse_file(FILE *fp, void (*callback)(struct s_exp*)) {
	int lineNumber;
	char lineBuf[LINE_BUFFER_SIZE];
	char *res;

	// Check file for validity, although this really should be checked in the calling scope too
	if (fp == NULL) {
		lisp_error("File not opened before calling lisp_parse_file()!");
		return;
	}

	// Loop over all lines and parse them individually
	lineNumber = 0;
	res = fgets(lineBuf, LINE_BUFFER_SIZE, fp);
	while (res != NULL) {
		
		// Read next line for loop
		res = fgets(lineBuf, LINE_BUFFER_SIZE, fp);
	}

}

