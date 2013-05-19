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
#define FLAG_FUNCTION		256

// Helper macros to check for types
#define IS_ATOM(x) ((x->flags & FLAG_ATOM) == FLAG_ATOM)
#define IS_SYMBOL(x) ((x->flags & FLAG_SYMBOL) == FLAG_SYMBOL)
#define IS_BOOL(x) ((x->flags & FLAG_BOOL) == FLAG_BOOL)
#define IS_INT(x) ((x->flags & FLAG_INT) == FLAG_INT)
#define IS_FLOAT(x) ((x->flags & FLAG_FLOAT) == FLAG_FLOAT)
#define IS_STRING(x) ((x->flags & FLAG_STRING) == FLAG_STRING)
#define IS_UNDEFINED(x) ((x->flags & FLAG_UNDEFINED) == FLAG_UNDEFINED)
#define IS_NIL(x) ((x->flags & FLAG_NIL) == FLAG_NIL)
#define IS_FUNCTION(x) ((x->flags & FLAG_FUNCTION) == FLAG_FUNCTION)

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
		struct s_exp *(*fn)(struct s_exp *);
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
struct s_exp *lookup_label(char *label, struct lisp_env *env);
void define_label(char *label, struct s_exp *val, struct lisp_env *env);
void cleanup_environment(struct lisp_env *env);

// S-Expression memory management
// Raw memory management, directly allocates and deallocates memory, used during parsing and compilation
struct s_exp *malloc_sexp(void);
void *free_sexp(struct s_exp *s);
void free_sexp_list(struct s_exp *s);
// Reference counting functions, these will track and tag expressions for garbage collection
void addref(struct s_exp *s);
void rmref(struct s_exp *s);
// Manages the free store of elements, where unusued s-expressions are kept
struct s_exp *alloc_s_exp_to_free(int count);
struct s_exp *find_free_s_exp(void);

// Error reporting
void lisp_error(char *fmt, ...);

// External function interface
struct s_exp *call_function(struct s_exp *function, struct s_exp *args);

// Printing expressions to the screen
void simple_print_exp(struct s_exp *exp);
void pretty_print_exp(struct s_exp *exp);
void pp_atomic(struct s_exp *exp);
void pp_helper(struct s_exp *exp, int symbolCount, int tabLevel);

// Primitives for use inside the language
#include "lisp_primitives.h"

///////////////////////////////////
// The main evaluator functions, defined in lisp.c
///////////////////////////////////
struct s_exp *apply(struct s_exp *function, struct s_exp *args);
struct s_exp *eval(struct s_exp *exp, struct lisp_env *env);
struct s_exp *evcond(struct s_exp *c, struct lisp_env *env);
struct s_exp *eval_each(struct s_exp *exp, struct lisp_env *env);

// Symbol definitions to expose primitives and handle builtins
#include "lisp_values.h"

#endif
