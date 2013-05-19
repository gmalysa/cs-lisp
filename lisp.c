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
 * @todo Add error checking here
 */
struct s_exp *eval(struct s_exp *exp, struct lisp_env *env) {
	struct s_exp *rtn;
	struct s_exp *car;
	struct s_exp *cdr;
	struct s_exp *caar;
	struct s_exp *cadar;
	struct s_exp *cur_arg;
	struct s_exp *ret;
	struct lisp_env *lambda_env;

	// Check if this is an atom or a pair
	if (IS_ATOM(exp)) {
		if (IS_SYMBOL(exp)) {
			rtn = lookup_label(exp->lisp_car.label, env);
			if (IS_UNDEFINED(rtn)) {
				lisp_error("undefined symbol %s\n", exp->lisp_car.label);
			}
			return rtn;
		}
		else {
			// It is self-evaluating and should be returned as is.
			return exp;
		}
	}
	
	// Handle as a pair
	car = _car(exp);
	cdr = _cdr(exp);
	
	if (IS_ATOM(car)) {
		// Handle a few specific special forms first, and then fall back to a symbol lookup
		if (c_lisp_eq(car, lisp_quote) == 1) {
			return _car(cdr);
		}
		else if (c_lisp_eq(car, lisp_atom) == 1) {
			return _atom(eval(_car(cdr), env));
		}
		else if (c_lisp_eq(car, lisp_eq) == 1) {
			return _eq(eval(_car(cdr), env), eval(_car(_cdr(cdr)), env));
		}
		else if (c_lisp_eq(car, lisp_cond) == 1) {
			return evcond(cdr, env);
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
		else if (c_lisp_eq(car, lisp_define) == 1) {
			// Note: cannot use define with lambdas yet, because they only evaluate in place
			define_label(_car(cdr)->lisp_car.label, eval(_car(_cdr(cdr)), env), env);
			return lisp_undefined;
		}
		else {
			// Simply get the corresponding value from the env and then eval with args unmodified
			car = lookup_label(car->lisp_car.label, env);
			return eval(_cons(car, cdr), env);
		}
	}
	else {
		caar = _car(car);
		cadar = _car(_cdr(car));

		if (c_lisp_eq(caar, lisp_lambda) == 1) {
			// Evaluate the rest of this application to the lambda
			cdr = eval_each(cdr, env);

			// Create a new environment, and push each formal argument to it with a value from the cdr
			lambda_env = (struct lisp_env *) calloc(1, sizeof(struct lisp_env));
			lambda_env->parent = env;

			while (!IS_NIL(cadar)) {
				cur_arg = _car(cadar);

				if (!IS_ATOM(cur_arg)) {
					lisp_error("Expected only atoms as formal arguments to lambda\n");
					cleanup_environment(lambda_env);
					return lisp_undefined;
				}

				define_label(cur_arg->lisp_car.label, _car(cdr), lambda_env);

				cadar = _cdr(cadar);
				cdr = _cdr(cdr);
			}

			// Evaluate the body expression in the new environment
			ret = eval(_car(_cdr(_cdr(car))), lambda_env);
			free(lambda_env);
			return ret;
		}
		else if (c_lisp_eq(caar, lisp_label) == 1) {
			// Create a new environment that will store the label for recursion
			lambda_env = (struct lisp_env *) calloc(1, sizeof(struct lisp_env));
			lambda_env->parent = env;
			define_label(cadar->lisp_car.label, car, lambda_env);

			// Evaluate the expression once the label has been added
			ret = eval(_cons(_car(_cdr(_cdr(car))), cdr), lambda_env);
			cleanup_environment(lambda_env);
			return ret;
		}
		else {
			// It has to be a native function, so evaluate it and see what we get
			car = eval(car, env);

			// Check that we've actually produced a function
			if (!IS_FUNCTION(car)) {
				lisp_error("Expected a function, received something else, in eval()\n");
				return lisp_undefined;
			}

			// Evaluate the arguments, and then pass it to the function call handler
			cdr = eval_each(cdr, env);
			return call_function(car, cdr);
		}
	}
}

/**
 * Evaluates a conditional expression represented by c
 */
struct s_exp *evcond(struct s_exp *c, struct lisp_env *env) {
	struct s_exp *car = _car(c);
	struct s_exp *cdr = _cdr(c);

	if (IS_ATOM(car)) {
		lisp_error("atom passed to cond as conditional expression, expected pair\n");
		return lisp_undefined;
	}

	if (c_lisp_eq(eval(_car(car), env), lisp_true) == 1) {
		return eval(_car(_cdr(car)), env);
	}
	else if (c_lisp_eq(cdr, lisp_nil) == 1) {
		return lisp_undefined;
	}
	else {
		return evcond(cdr, env);
	}
}

/**
 * Helper to evaluate the arguments list and return them as a single list to be
 * passed to the function call, as is expected
 * In mccarthy's paper, this is called evlis, I wrote it for the function interface without
 * realizing it was needed for core stuff too
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
