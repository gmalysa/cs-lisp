#ifndef _LISP_PARSER_H_
#define _LISP_PARSER_H_
/**
 * This defines everything for our parser, which does not evaluate anything.
 */

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

// Project headers
#include "lisp.h"

// Parse a file and emit each s-expression found there to the callback
void lisp_parse_file(FILE *fp, void (*callback)(struct s_exp*));

#endif
