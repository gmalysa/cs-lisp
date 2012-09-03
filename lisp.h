#ifndef _LISP_H_
#define _LISP_H_
/**
 * This defines the core of the lisp interpreting/executing engine
 */

// Standard headers
#include <stdarg.h>
#include <inttypes.h>

// Project definitions
// None

// Some possible flags for the flags field in our s-expression storage
#define FLAG_ATOM			1
#define FLAG_SYMBOL			2
#define FLAG_INT			4
#define FLAG_FLOAT			8
#define FLAG_BOOL			16
#define FLAG_STRING			32
#define FLAG_UNDEFINED		64
#define FLAG_NIL			128

// Helper macros to check for types
#define IS_ATOM(x) ((x->flags & FLAG_ATOM) == FLAG_ATOM)
#define IS_SYMBOL(x) ((x->flags & FLAG_SYMBOL) == FLAG_SYMBOL)
#define IS_BOOL(x) ((x->flags & FLAG_BOOL) == FLAG_BOOL)
#define IS_INT(x) ((x->flags & FLAG_INT) == FLAG_INT)
#define IS_FLOAT(x) ((x->flags & FLAG_FLOAT) == FLAG_FLOAT)
#define IS_STRING(x) ((x->flags & FLAG_STRING) == FLAG_STRING)
#define IS_UNDEFINED(x) ((x->flags & FLAG_UNDEFINED) == FLAG_UNDEFINED)
#define IS_NIL(x) ((x->flags & FLAG_NIL) == FLAG_NIL)

/**
 * This structure defines the storage for any s-expression, which is effectively
 * completely regular, regardless of what it does or represents.
 */
struct s_exp {
	uint32_t flags;
	union {
		// If this is not an atom, then it is a list and car points to the first item, and
		// cdr points to the second
		struct s_exp *car;

		// If this is an atom, then these are used based on flags
		uint64_t uiVal;
		double dVal;
		int64_t siVal;
		char *strVal;
		char *label;
	} lisp_car;
	union {
		// If this is not an atom, cdr points to the rest of the list
		struct s_exp *cdr;
	} lisp_cdr;
};

/**
 * This is used to store label->s-expression mappings in the environment, and it is a linked list
 * because that is the easiest way to chain them together (although not the most efficient).
 */
struct lisp_mapping {
	char *label;
	struct s_exp *exp;
	struct lisp_mapping *next;
};

/**
 * Environments (each of which contain definitions for bound variables) are stored in
 * a linked list format that approximates a stack, where each environment points to the
 * more general environment below it (or, outside its scope, if you will).
 */
struct lisp_env {
	struct lisp_mapping *mapping;
	struct lisp_env *parent;
};

///////////////////////////////////
// Execution helpers that operate internally within the lisp environment, defined in lisp_helper.c
///////////////////////////////////

// Environment management/execution
struct lisp_env *lisp_init(void);
struct s_exp *alloc_s_exp(int count);
struct s_exp *find_free_s_exp(void);
struct s_exp *lookup_label(char *label, struct lisp_env *env);
void define_label(char *label, struct s_exp *val, struct lisp_env *env);
void cleanup_environment(struct lisp_env *env);

// Error reporting
void lisp_error(char *fmt, ...);

// External function interface
struct s_exp *call_function(struct s_exp *function, struct s_exp *args);

// Printing expressions to the screen
void pretty_print_exp(struct s_exp *exp);
void pp_atomic(struct s_exp *exp);
void pp_helper(struct s_exp *exp, int symbolCount, int tabLevel);

///////////////////////////////////
// Primitive functions, defined in lisp_primitives.c
///////////////////////////////////
struct s_exp *atom(struct s_exp *s);
struct s_exp *eq(struct s_exp *a, struct s_exp *b);
struct s_exp *car(struct s_exp *s);
struct s_exp *cdr(struct s_exp *s);
struct s_exp *cons(struct s_exp *a, struct s_exp *b);
struct s_exp *appq(struct s_exp *args);

///////////////////////////////////
// The main evaluator functions, defined in lisp.c
///////////////////////////////////
struct s_exp *apply(struct s_exp *function, struct s_exp *args);
struct s_exp *eval(struct s_exp *exp);

///////////////////////////////////
// There are a bunch of built-in values used by these functions, their storage is allocated in lisp_values.c
///////////////////////////////////
extern struct s_exp *lisp_nil;
extern struct s_exp *lisp_true;
extern struct s_exp *lisp_false;

#endif
