#ifndef _LISP_PRIMITIVES_H_
#define _LISP_PRIMITIVES_H_
/**
 * Here we define the primitive functions that are core to our lisp
 * implementation
 */

// Lisp-space functions, operate on S-expressions
struct s_exp *_atom(struct s_exp *s);
struct s_exp *_eq(struct s_exp *a, struct s_exp *b);
struct s_exp *_car(struct s_exp *s);
struct s_exp *_cdr(struct s_exp *s);
struct s_exp *_cons(struct s_exp *a, struct s_exp *b);

// C-space functions where appropriate, which are more useful inside the evaluator
int c_lisp_eq(struct s_exp *a, struct s_exp *b);

#endif
