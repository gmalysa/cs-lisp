/**
 * Implementation of the lisp interpreter/running routines
 */

// Standard headers
#include <stdlib.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Project headers
#include "lisp.h"

// This is our pre-allocated list of free s-expressions. In the event that we need more, find_free_s_exp()
// will just call the allocator again
struct s_exp *next_free_exp = 0;

/**
 * This function creates the global environment, adds labels for our default symbols, and creates some
 * free s expressions to start working with
 */
struct lisp_env *lisp_init(void) {
	struct lisp_env *env;

	// Allocate the environment and define the built-in values first
	env = (struct lisp_env *) calloc(1, sizeof(struct lisp_env));
	define_label("nil", lisp_nil, env);
	define_label("#t", lisp_true, env);
	define_label("#f", lisp_false, env);

	// Allocate the initial batch of free s-expressions
	next_free_exp = alloc_s_exp(100);

	return env;
}

/**
 * This allocates a bunch of s-expression structures and chains them together properly, so that
 * they can be used with find_free_s_exp()
 */
struct s_exp *alloc_s_exp(int count) {
	struct s_exp *head;
	int i;

	head = (struct s_exp *) calloc(count, sizeof(struct s_exp));
	head[count-1].lisp_cdr.cdr = 0;
	for (i = 0; i < count-1; ++i) {
		head[i].lisp_cdr.cdr = &head[i+1];
	}

	return head;
}

/**
 * Print an error message, for some nice abstraction. Eventually this might prepend or something
 */
void lisp_error(char *fmt, ...) {
	va_list args;
	
	// Set up variadic arguments and then call printf with them
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

/**
 * This function traverses the given environment tables and looks up the s-expression that a label
 * points to.
 */
struct s_exp *lookup_label(char *label, struct lisp_env *env) {
	struct lisp_mapping *map;

	// First check to make sure we have environment left in which to search
	if (env == 0 || env->mapping == 0) {
		lisp_error("Label %s not found!", label);
		return lisp_nil;
	}

	// Now walk this environment's mapping to look for this label
	map = env->mapping;
	while (map != 0) {
		if (strcmp(label, map->label) == 0) {
			return map->exp;
		}
		map = map->next;
	}
	
	// Not found within this scope, recurse to look into parent scope
	return lookup_label(label, env->parent);
}

/**
 * Insert a label into the current environment with an s-expression value.
 * Note that this doesn't bother checking if it already exists, because we prepend it
 * will effectively be overwritten anyway, from the lookup perspective.
 *
 * Also, since s-expressions are immutable, we don't need to make a copy, ever.
 *
 * TODO: Preallocate a bunch of mapping structures to speed up their allocation/deallocation
 */
void define_label(char *label, struct s_exp *val, struct lisp_env *env) {
	struct lisp_mapping *mapping;
	
	// Allocate our mapping, copy the label to make it easier on the caller, and then insert
	mapping = (struct lisp_mapping *) calloc(1, sizeof(struct lisp_mapping));
	mapping->label = strdup(label);
	mapping->exp = val;
	mapping->next = env->mapping;
	env->mapping = mapping;
}

/**
 * Walk an environment and deallocate all of the mappings and labels found therein.
 * If only it were this easy in real life to clean up the environment!
 *
 * Note: this doesn't recurse on the parent. This is just deallocating this particular
 * call frame/locale (for instance, within a let statement, or something).
 *
 * Also, the environment struct itself must still be deallocated external to this function
 */
void cleanup_environment(struct lisp_env *env) {
	struct lisp_mapping *next;
	struct lisp_mapping *prev;

	// Just iterate over the entire linked list and deallocate everything we originally
	// allocated with malloc()
	next = env->mapping;
	while (next != 0) {
		free(next->label);
		prev = next;
		next = next->next;
		free(prev);
	}
}

/**
 * This finds a free s-expression to allocate to cons (that is, one who is pointed to by our free list)
 */
struct s_exp *find_free_s_exp(void) {
	struct s_exp *rtn;

	// Make sure we have space
	if (next_free_exp == 0) {
		next_free_exp = alloc_s_exp(100);
	}
	
	// Update our linked list and return the first free s-expression
	rtn = next_free_exp;
	next_free_exp = next_free_exp->lisp_cdr.cdr;
	return rtn;
}

/**
 * This wraps a call to a function that is provided as some kind of external primitive. In reality,
 * this delegates to another lisp-based handler, which knows the function's signature and parses
 * out the arguments.
 */
struct s_exp *call_function(struct s_exp *function, struct s_exp *args) {
	return lisp_nil;
}
