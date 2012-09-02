/**
 * Routines for parsing strings and files into s-expressions
 */

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

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
	char *nextBuf;
	struct lp_token *prevToken;
	struct lp_token *nextToken;

	// Check file for validity, although this really should be checked in the calling scope too
	if (fp == NULL) {
		lisp_error("File not opened before calling lisp_parse_file()!");
		return;
	}

	// Loop over all lines and parse into them individually
	lineNumber = 1;
	res = fgets(lineBuf, LINE_BUFFER_SIZE, fp);
	prevToken = 0;
	while (res != NULL) {
		// Loop over the line and parse out based on our state
		while (find_next_token(res, &nextToken, &nextBuf) == FOUND_TOKEN) {
			// Append the newly found token to our list of tokens
			if (prevToken != 0) {
				prevToken->next = nextToken;
			}
			nextToken->lineNumber = lineNumber;
			prevToken = nextToken;
			
			// Set res to the next buffer for our next loop iteration
			res = nextBuf;
		}
		
		// Read next line for loop
		res = fgets(lineBuf, LINE_BUFFER_SIZE, fp);
	}

	// Now that we have all the tokens, parse them into a series of S-expressions
	// TODO: Parse into S-expressions
}

/**
 * This function searches the given character buffer for the next interesting thing,
 * which, realistically, is defined by the rules, but traditionally this would include
 * things like (, ), [, ], ', or a symbol. Note that this buffer should represent one "line"
 *
 * TODO: Implement rulelists so that we can define new tokens easily
 */
int find_next_token(char *buf, struct lp_token **token, char **nextBuf) {
	char *end;

	// Search for the first non-whitespace character--you can't put the increment in the loop
	// or you'll miss the first character of the token
	while (isspace(*buf)) {
		buf++;
	}

	// If we're at the end of the string, no token was found
	if (*buf == 0) {
		*token = 0;
		*nextBuf = 0;
		return NO_TOKEN;
	}

	// Check if this is the start of a comment, if so return no token because this applies to the line
	if (buf[0] == ';') {
		*token = 0;
		*nextBuf = 0;
		return NO_TOKEN;
	}

	// There must be a token, so we can just allocate the space now
	*token = (struct lp_token *) calloc(1, sizeof(struct lp_token));

	// Check the token against some statically defined rules
	if (buf[0] == '(') {
		(*token)->type = LPT_OPEN_PAREN;
		(*token)->text = strndup("(", 1);
		*nextBuf = buf+1;
	}
	else if (buf[0] == ')') {
		(*token)->type = LPT_CLOSE_PAREN;
		(*token)->text = strndup(")", 1);
		*nextBuf = buf+1;
	}
	else if (buf[0] == '[') {
		(*token)->type = LPT_OPEN_BRACKET;
		(*token)->text = strndup("[", 1);
		*nextBuf = buf+1;
	}
	else if (buf[0] == ']') {
		(*token)->type = LPT_CLOSE_BRACKET;
		(*token)->text = strndup("]", 1);
		*nextBuf = buf+1;
	}
	else if (buf[0] == '\'') {
		(*token)->type = LPT_QUOTE;
		(*token)->text = strndup("'", 1);
		*nextBuf = buf+1;
	}
	else if (buf[0] == '"') {
		(*token)->type = LPT_DOUBLE_QUOTE;
		(*token)->text = strndup("\"", 1);
		*nextBuf = buf+1;
	}
	else {
		// Doesn't match any single character rules, read out the whole symbol
		end = buf;
		while (!isspace(*end) && (*end != '\0')) {
			end++;
		}

		// Duplicate it into our token type
		(*token)->type = LPT_SYMBOL;
		(*token)->text = strndup(buf, end-buf);
		*nextBuf = end;
	}

	// We've found a token and set it up in the parameters, so return true
	return FOUND_TOKEN;
}

