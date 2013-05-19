#ifndef _LISP_VALUES_H_
#define _LISP_VALUES_H_
/**
 * Predefined values within the lisp environment, which should be
 * put into the environment during initialization
 */

// Standard headers
#include <inttypes.h>

// Project headers
#include "lisp.h"
#include "lisp_primitives.h"

// Primitive symbols and special values
extern struct s_exp *lisp_undefined;
extern struct s_exp *lisp_nil;
extern struct s_exp *lisp_true;
extern struct s_exp *lisp_false;

// Symbols for special forms
extern struct s_exp *lisp_quote;
extern struct s_exp *lisp_cond;
extern struct s_exp *lisp_define;
extern struct s_exp *lisp_lambda;

// Symbols for built-ins/primitive functions
extern struct s_exp *lisp_cons;
extern struct s_exp *lisp_car;
extern struct s_exp *lisp_cdr;
extern struct s_exp *lisp_eq;
extern struct s_exp *lisp_atom;

#endif
