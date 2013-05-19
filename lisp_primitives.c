/**
 * Defines the lisp primitves, both the ones that are used directly within the evaluator
 * and the ones that are exposed to user space.
 */

// Standard headers
#include <stdlib.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Project headers
#include "lisp.h"
#include "lisp_primitives.h"

/**
 * Determines if two S-expressions are equal. Note that they must be atoms, a version of this that operates
 * on lists can be constructed within the language itself (although a fast version may be provided eventually).
 */
int c_lisp_eq(struct s_exp *a, struct s_exp *b) {

	// If either is not atomic, they are not equal
	if (!IS_ATOM(a) || !IS_ATOM(b)) {
		return 0;
	}

	// If either is undefined, they cannot be equal (undefined is not equal to anything, including itself)
	if (IS_UNDEFINED(a) || IS_UNDEFINED(b)) {
		return 0;
	}

	// If they have different flags, they can't be equal
	if (a->flags != b->flags) {
		return 0;
	}

	// If we're dealing with a string or symbol, use strcmp
	if (IS_STRING(a) || IS_SYMBOL(a)) {
		if (strcmp(a->lisp_car.label, b->lisp_car.label) == 0) {
			return 1;
		}
		else {
			return 0;
		}
	}

	// Otherwise, for float, int, and boolean types, they are all handled by pretending it's an integer
	return (a->lisp_car.siVal == b->lisp_car.siVal) ? 1 : 0;
}

/**
 * S-expression wrapper for the C-environment version gives us our lisp version
 */
struct s_exp *_eq(struct s_exp *a, struct s_exp *b) {
	int is_eq;
	
	// If either pointer is invalid, this is a problem
	if (a == 0 || b == 0) {
		lisp_error("Error: Not enough arguments supplied to eq?");
		return lisp_undefined;
	}

	is_eq = c_lisp_eq(a, b);

	if (is_eq == 1) {
		return lisp_true;
	}

	return lisp_false;
}

/**
 * S-expression wrapper for our IS_ATOM() macro
 */
struct s_exp *_atom(struct s_exp *s) {
	if (IS_ATOM(s) && !IS_UNDEFINED(s)) {
		return lisp_true;
	}
	return lisp_false;
}

/**
 * Constructs a new S-expression by pairing two existing ones
 */
struct s_exp *_cons(struct s_exp *a, struct s_exp *b) {
	struct s_exp *rtn;
	
	// Validate pointers before going any farther
	if (a == 0 || b == 0) {
		lisp_error("Error: Invalid arguments supplied to cons");
		return lisp_undefined;
	}

	rtn = find_free_s_exp();

	// No flags are set for a cons pair, because it matches none of our special forms
	rtn->flags = 0;
	rtn->lisp_car.car = a;
	rtn->lisp_cdr.cdr = b;

	return rtn;
}

/**
 * Access the car pointer, throws an error if this is not a pair
 */
struct s_exp *_car(struct s_exp *s) {
	if (s == 0 || IS_ATOM(s)) {
		lisp_error("Error: Invalid argument (non-pair) to car");
		return lisp_undefined;
	}

	return s->lisp_car.car;
}

/**
 * Same thing, but for the cdr pointer
 */
struct s_exp *_cdr(struct s_exp *s) {
	if (s == 0 || IS_ATOM(s)) {
		lisp_error("Error: Invalid argument (non-pair) to cdr");
		return lisp_undefined;
	}

	return s->lisp_cdr.cdr;
}
