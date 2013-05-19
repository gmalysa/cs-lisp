/**
 * Implementation of the lisp interpreter/running routines
 */

// Standard headers
#include <stdlib.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

// Project headers
#include "lisp.h"

/**
 * The core of the lisp evaluator, this function takes in an s-expression and evalautes it.
 * This essentially attempts to follow the evaluator given in the paper, with a few modifications
 * to support things like self-evaluating types (i.e. a string or a number, as opposed to only
 * symbols), as well as any other primitives that we decide to implement.
 */
struct s_exp *eval(struct s_exp *exp, struct lisp_env *env) {
	struct s_exp *rtn;
	struct s_exp *car;
	struct s_exp *cdr;

	// Check if this is an atom or a pair
	if (IS_ATOM(exp)) {
		if (IS_SYMBOL(exp)) {
			// TODO: Look up this label in the environment given
			rtn = lookup_label(exp->lisp_car.label, env);
			if (IS_UNDEFINED(rtn)) {
				// TODO: Print error message
			}
			return rtn;
		}
		else {
			// It is self-evaluating and should be returned as is.
			return exp;
		}
	}
	else {
		car = exp->lisp_car.car;
		cdr = exp->lisp_cdr.cdr;

		// Handle a few specific special forms first, and then fall back to a symbol lookup
		if (c_lisp_eq(car, lisp_quote) == 1) {
			// Quote doesn't evaluate its arguments, just pass through
			return cdr;
		}
		else if (c_lisp_eq(car, lisp_cond) == 1) {
			// TODO: Handle cond statements
		}
		else if (c_lisp_eq(car, lisp_lambda) == 1) {
			// TODO: Handle lambdas
		}
		else if (c_lisp_eq(car, lisp_define) == 1) {
			// TODO: Define an expression
		}
		return lisp_nil;
	}
}
