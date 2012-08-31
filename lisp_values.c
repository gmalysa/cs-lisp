/**
 * Struct declaration of several built-in/default s-expressions that may be used by the internal functions
 */

// Standard headers
#include <inttypes.h>

// Project definitions
#include "lisp.h"

/**
 * First we declare them locally and then export a bunch of pointers.
 * Why? Because everything expects struct s_exp *, and this eliminates
 * the need for & everywhere.
 */

// The list terminator, nil
struct s_exp _lisp_nil = {
	.flags = FLAG_ATOM | FLAG_INT,
	.lisp_car = {.siVal = 0},
	.lisp_cdr = {.cdr = 0}
};

// The truth symbol, #t
struct s_exp _lisp_true = {
	.flags = FLAG_ATOM | FLAG_BOOL,
	.lisp_car = {.uiVal = 1},
	.lisp_cdr = {.cdr = 0}
};

// The false symbol, #f
struct s_exp _lisp_false = {
	.flags = FLAG_ATOM | FLAG_BOOL,
	.lisp_car = {.uiVal = 0},
	.lisp_cdr = {.cdr = 0}
};

// Now the structure pointers
struct s_exp *lisp_nil = &_lisp_nil;
struct s_exp *lisp_true = &_lisp_true;
struct s_exp *lisp_false = &_lisp_false;
