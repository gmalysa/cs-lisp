/**
 * Struct declaration of several built-in/default s-expressions that may be used by the internal functions
 */

// Standard headers
#include <inttypes.h>

// Project definitions
#include "lisp.h"
#include "lisp_primitives.h"
#include "lisp_values.h"

/**
 * First we declare them locally and then export a bunch of pointers.
 * Why? Because everything expects struct s_exp *, and this eliminates
 * the need for & everywhere.
 */

/**
 * The built-in primitive and special values
 */

// The undefined symbol, usually returned by errors and stuff
struct s_exp _lisp_undefined = {
	.flags = FLAG_ATOM | FLAG_UNDEFINED,
	.lisp_car = {.label = "#<undefined>"},
	.lisp_cdr = {.cdr = 0}
};

// The list terminator, nil
struct s_exp _lisp_nil = {
	.flags = FLAG_ATOM | FLAG_SYMBOL | FLAG_NIL,
	.lisp_car = {.label = "nil"},
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

/**
 * The special form symbols
 */

// Quote form, does not evaluate its arguments
struct s_exp _lisp_quote = {
	.flags = FLAG_ATOM | FLAG_SYMBOL,
	.lisp_car = {.label = "quote"},
	.lisp_cdr = {.cdr = 0}
};

// Cond form, conditionally evaluates to expressions
struct s_exp _lisp_cond = {
	.flags = FLAG_ATOM | FLAG_SYMBOL,
	.lisp_car = {.label = "cond"},
	.lisp_cdr = {.cdr = 0}
};

// Define form, binds a value for a variable into the environment
struct s_exp _lisp_define = {
	.flags = FLAG_ATOM | FLAG_SYMBOL,
	.lisp_car = {.label = "define"},
	.lisp_cdr = {.cdr = 0}
};

// Lambda form, creates a function
struct s_exp _lisp_lambda = {
	.flags = FLAG_ATOM | FLAG_SYMBOL,
	.lisp_car = {.label = "lambda"},
	.lisp_cdr = {.cdr = 0}
};

/**
 * The primitive symbols
 */

struct s_exp _lisp_cons = {
	.flags = FLAG_ATOM | FLAG_SYMBOL,
	.lisp_car = {.label = "cons"},
	.lisp_cdr = {.cdr = 0}
};

struct s_exp _lisp_car = {
	.flags = FLAG_ATOM | FLAG_SYMBOL,
	.lisp_car = {.label = "car"},
	.lisp_cdr = {.cdr = 0}
};

struct s_exp _lisp_cdr = {
	.flags = FLAG_ATOM | FLAG_SYMBOL,
	.lisp_car = {.label = "cdr"},
	.lisp_cdr = {.cdr = 0}
};

struct s_exp _lisp_eq = {
	.flags = FLAG_ATOM | FLAG_SYMBOL,
	.lisp_car = {.label = "eq?"},
	.lisp_cdr = {.cdr = 0}
};

struct s_exp _lisp_atom = {
	.flags = FLAG_ATOM | FLAG_SYMBOL,
	.lisp_car = {.label = "atom"},
	.lisp_cdr = {.cdr = 0}
};

// Now the structure pointers
struct s_exp *lisp_undefined = &_lisp_undefined;
struct s_exp *lisp_nil = &_lisp_nil;
struct s_exp *lisp_true = &_lisp_true;
struct s_exp *lisp_false = &_lisp_false;

struct s_exp *lisp_quote = &_lisp_quote;
struct s_exp *lisp_cond = &_lisp_cond;
struct s_exp *lisp_define = &_lisp_define;
struct s_exp *lisp_lambda = &_lisp_lambda;

struct s_exp *lisp_cons = &_lisp_cons;
struct s_exp *lisp_car = &_lisp_car;
struct s_exp *lisp_cdr = &_lisp_cdr;
struct s_exp *lisp_eq = &_lisp_eq;
struct s_exp *lisp_atom = &_lisp_atom;
