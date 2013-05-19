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
	
		if (IS_ATOM(car)) {
			// Handle a few specific special forms first, and then fall back to a symbol lookup
			if (c_lisp_eq(car, lisp_quote) == 1) {
				// Quote doesn't evaluate its arguments, just pass through
				return _car(cdr);
			}
			else if (c_lisp_eq(car, lisp_atom) == 1) {
				return _atom(eval(_car(cdr), env));
			}
			else if (c_lisp_eq(car, lisp_eq) == 1) {
				return _eq(eval(_car(cdr), env), eval(_car(_cdr(cdr)), env));
			}
			else if (c_lisp_eq(car, lisp_cond) == 1) {
				// TODO: Handle cond statements
			}
			else if (c_lisp_eq(car, lisp_car) == 1) {
				return _car(eval(_car(cdr), env));
			}
			else if (c_lisp_eq(car, lisp_cdr) == 1) {
				return _cdr(eval(_car(cdr), env));
			}
			else if (c_lisp_eq(car, lisp_cons) == 1) {
				return _cons(eval(_car(cdr), env), eval(_car(_cdr(cdr)), env));
			}
			else {
				car = eval(car, env);
				// todo call apply with this function
				return lisp_undefined;
			}
		}
		else if (c_lisp_eq(car, lisp_lambda) == 1) {
			// TODO: Handle lambdas
		}
		else if (c_lisp_eq(car, lisp_define) == 1) {
			// TODO: Define an expression
		}
		else {
			// It has to be a native function, so evaluate it and see what we get
			car = eval(car, env);

			// Check that we've actually produced a function
			if (!IS_FUNCTION(car)) {
				lisp_error("Expected a function, received something else, in eval()");
				return lisp_undefined;
			}

			// Evaluate the arguments, and then pass it to the function call handler
			cdr = eval_each(cdr, env);
			return call_function(car, cdr);
		}
		return lisp_nil;
	}
}

/**
 * Helper to evaluate the arguments list and return them as a single list to be
 * passed to the function call, as is expected
 */
struct s_exp *eval_each(struct s_exp *exp, struct lisp_env *env) {
	if (IS_NIL(exp))
		return lisp_nil;
	
	if (IS_ATOM(exp))
		return eval(exp, env);
	
	return _cons(eval(_car(exp), env), eval_each(_cdr(exp), env));
}

/**
 * Calls a function whose pointer is stored in the given s-expression, with the
 * given, pre-evaluated arguments
 */
struct s_exp *call_function(struct s_exp *function, struct s_exp *args) {
	return function->lisp_car.fn(args);
}
