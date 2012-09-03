/**
 * Routines for parsing strings and files into s-expressions
 *
 * TODO: In order to properly handle strings we need to add a token for the stripped whitespace
 * TODO: Whitespace will be mostly ignored, but it'll allow us to convert whitespace between two
 * TODO: double quotes into a part of the string. Technically, it would also enable us to write
 * TODO: syntax rules involving whitespace, which someone somewhere might care about. But I hate them.
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
struct s_list *lisp_parse_file(FILE *fp) {
	int lineNumber;
	int result;
	char lineBuf[LINE_BUFFER_SIZE];
	char *res;
	struct lp_token *startToken;
	struct lp_token *prevToken;
	struct lp_token *nextToken;
	struct s_exp *exp;
	struct s_list *firstExp;
	struct s_list *expList;

	// Check file for validity, although this really should be checked in the calling scope too
	if (fp == NULL) {
		lisp_error("File not opened before calling lisp_parse_file()!\n");
		return 0;
	}

	// Allocate our first token as a start token, just to regularize later code
	prevToken = (struct lp_token *) malloc(sizeof(struct lp_token));
	prevToken->type = LPT_START;
	prevToken->text = 0;
	prevToken->lineNumber = 0;
	prevToken->next = 0;
	startToken = prevToken;

	// Loop over all lines and parse into tokens individually
	lineNumber = 1;
	res = fgets(lineBuf, LINE_BUFFER_SIZE, fp);
	while (res != NULL) {
//		printf(">>Attempting to tokenize #%s", res);
		// Farm out work to the line tokenizer instead of duplicating the code
		prevToken = tokenize_line(res, lineNumber, prevToken);

		// Read next line for loop
		res = fgets(lineBuf, LINE_BUFFER_SIZE, fp);
		lineNumber++;
	}
	
	// Now that we have all the tokens, parse them into a series of S-expressions
	firstExp = 0;
	expList = 0;
	prevToken = startToken;
	while (prevToken != 0) {
		result = parse_s_expression(prevToken, &exp, &nextToken);
		if (result == SEP_SUCCESS) {
			if (firstExp == 0) {
				firstExp = (struct s_list *) malloc(sizeof(struct s_list));
				firstExp->exp = exp;
				firstExp->next = 0;
				expList = firstExp;
			}
			else {
				expList->next = (struct s_list *) malloc(sizeof(struct s_list));
				expList = expList->next;
				expList->exp = exp;
				expList->next = 0;
			}
		}
		else {
			lisp_error("File parsing terminated with an error -- see earlier messages for details.\n");
			return 0;
		}

		prevToken = nextToken;
	}

	// Deallocate all of the memory we used to create our tokens
	// TODO: Clean up token memory
	
	// Pass back the list of expressions
	return firstExp;
}

/**
 * Describe a token by printing flags, text, line number, etc.
 */
void describe_token(struct lp_token *token) {
	const char *typeStr;

	// Identify token type as a string
	switch(token->type) {
		case LPT_NULL: typeStr = "LPT_NULL"; break;
		case LPT_OPEN_PAREN: typeStr = "LPT_OPEN_PAREN"; break;
		case LPT_CLOSE_PAREN: typeStr = "LPT_CLOSE_PAREN"; break;
		case LPT_OPEN_BRACKET: typeStr = "LPT_OPEN_BRACKET"; break;
		case LPT_CLOSE_BRACKET: typeStr = "LPT_CLOSE_BRACKET"; break;
		case LPT_QUOTE: typeStr = "LPT_QUOTE"; break;
		case LPT_DOUBLE_QUOTE: typeStr = "LPT_DOUBLE_QUOTE"; break;
		case LPT_SYMBOL: typeStr = "LPT_SYMBOL"; break;
		case LPT_START: typeStr = "LPT_START"; break;
		default: typeStr = "UNIDENTIFIED TOKEN"; break;
	}

	// Print out information
	printf("---TOKEN---\n\tType: %s\n\tLine: %d\n\tText: %s\n", typeStr, token->lineNumber, token->text);
}

/**
 * This parses a line into a series of tokens and returns a pointer to the last 
 * token that was found. Intermediate tokens are appended to the list given. 
 * If there was no token found, it'll return the given starting token
 */
struct lp_token *tokenize_line(char *line, int lineNumber, struct lp_token *prevToken) {
	struct lp_token *nextToken;
	char *res;
	char *nextBuf;
	
	// Initialize nextToken so that we can return it even if we don't find anything
	nextToken = prevToken;
	res = line;

	// Loop over the line searching for tokens
	while (find_next_token(res, &nextToken, &nextBuf) == FOUND_TOKEN) {
//		describe_token(nextToken);

		// Append the newly found token to our list of tokens
		prevToken->next = nextToken;
		nextToken->lineNumber = lineNumber;
		prevToken = nextToken;
		
		// Set res to the next buffer for our next loop iteration
		res = nextBuf;
	}
	
	return nextToken;
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
		return NO_TOKEN;
	}

	// Check if this is the start of a comment, if so return no token because this applies to the line
	if (buf[0] == ';') {
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
//		printf("Searching for symbol:\n\t");
		while (!isspace(*end) && (*end != '\0') && (*end != ']') && (*end != ')')) {
//			printf("%c,(0x%x)", *end, *end);
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

/**
 * Given a starting token, this extracts a complete S-expression or returns an error, and
 * it sets a pointer to the next token from the chain after the complete S-expression found here
 */
int parse_s_expression(struct lp_token *startToken, struct s_exp **newExp, struct lp_token **nextStartToken) {
	struct s_exp *startExp;
	struct s_exp *exp;
	struct s_exp *prevExp;
	struct lp_token *nextToken;
	struct lp_token *nextExpToken;
	int result;

	// If this is a null or a start, just pass through the next token
	if (startToken->type == LPT_NULL || startToken->type == LPT_START) {
		return parse_s_expression(startToken->next, newExp, nextStartToken);
	}

	// If we're looking at a symbol, that is the expression, return it
	if (startToken->type == LPT_SYMBOL) {
		// TODO: Add the ability to parse bools, numbers, and floats directly
		exp = (struct s_exp *) malloc(sizeof(struct s_exp));
		exp->flags = FLAG_ATOM | FLAG_SYMBOL;
		exp->lisp_car.label = strdup(startToken->text);
		
		// Export the results
		*nextStartToken = startToken->next;
		*newExp = exp;
		return SEP_SUCCESS;
	}

	// This is not a symbol, so we need to apply our rules for other token types
	switch (startToken->type) {
		case LPT_OPEN_PAREN:
		case LPT_OPEN_BRACKET:
			// Create the S-expression to point to the list
			prevExp = (struct s_exp *) malloc(sizeof(struct s_exp));
			prevExp->flags = 0;
			prevExp->lisp_car.car = lisp_nil;
			prevExp->lisp_cdr.cdr = lisp_nil;
			startExp = prevExp;

			// Move along the token list and add elements to the S-expression in list form
			nextToken = startToken->next;
			while (nextToken != 0 && nextToken->type != LPT_CLOSE_PAREN && nextToken->type != LPT_CLOSE_BRACKET) {
				// Recursively find the next expression, which will form the car of the current expression
				result = parse_s_expression(nextToken, &exp, &nextExpToken);
				if (result == SEP_ERROR) {
					// TODO: Deallocate the entire tree that we've allocated so far
					return SEP_ERROR;
				}

				// Save the sub expression and create the next list element
				prevExp->lisp_car.car = exp;
				exp = (struct s_exp *) malloc(sizeof(struct s_exp));
				prevExp->lisp_cdr.cdr = exp;
				exp->flags = 0;
				exp->lisp_car.car = lisp_nil;
				exp->lisp_cdr.cdr = lisp_nil;

				// Advanced our list pointers for the next iteration
				prevExp = exp;
				nextToken = nextExpToken;
			}
			
			// If we broke to avoid a null pointer, that is a parse error
			if (nextToken == 0) {
				lisp_error("Unmatched ) or ] found in source! End of file reached.\n");
				// TODO: Deallocate the memory used to construct this branch of the S-expression
				return SEP_ERROR;
			}

			// Otherwise, we broke because we got a matching parenthesis, so we need to advance our next token
			// and return to the caller
			*nextStartToken = nextToken->next;
			*newExp = startExp;
			return SEP_SUCCESS;
		case LPT_CLOSE_PAREN:
		case LPT_CLOSE_BRACKET:
			// These get picked up by the open paren/bracket expressions, so we should never see
			// them as the first element passed to the parser
			lisp_error("Unexpected ) or ] on line %d.\n", startToken->lineNumber);
			return SEP_ERROR;
		case LPT_QUOTE:
			// TODO: Determine how to properly insert quotes into the S-expression tree as sugar
			lisp_error("Unexpected ' on line %d. Quotes are not yet supported!\n", startToken->lineNumber);
			return SEP_ERROR;
		case LPT_DOUBLE_QUOTE:
			// TODO: Look for a matching double quote and take everything in between as a string
			lisp_error("Unexpected \" on line %d. Double quotes are not yet supported!\n", startToken->lineNumber);
			return SEP_ERROR;
		default:
			// This should never happen as we've handled the other three token types previously!
			lisp_error("Unidentified token type %d found on line %d!\n", startToken->type, startToken->lineNumber);
			return SEP_ERROR;
	}
}

