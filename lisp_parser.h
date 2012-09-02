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

// States for the parser's loop
#define LP_STATE_RDY			0

// Status codes used by the token finding routine
#define NO_TOKEN				0
#define FOUND_TOKEN				1

// Token types
#define LPT_NULL				0
#define LPT_OPEN_PAREN			1
#define LPT_CLOSE_PAREN			2
#define LPT_OPEN_BRACKET		3
#define LPT_CLOSE_BRACKET		4
#define LPT_QUOTE				5
#define LPT_DOUBLE_QUOTE		6
#define LPT_SYMBOL				7

// Linked list for storing all of the top-level s-expressions
struct s_list {
	struct s_exp *exp;
	struct s_list *next;
};

// Structure for storing a token that was read out during parsing, these get chained together
struct lp_token {
	uint32_t type;
	char *text;
	uint32_t lineNumber;
	struct lp_token *next;
};

// Parsing interface--parses lines and files
void lisp_parse_file(FILE *fp, void (*callback)(struct s_exp*));

// Helper functions, used internally
int find_next_token(char *buf, struct lp_token **token, char **nextBuf);

#endif
