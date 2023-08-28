#include <stdio.h>

#ifndef LL_DEFINED
#define LL_DEFINED

typedef struct ll_node {
    void *value;
    struct ll_node *next;
} ll_node;

typedef void (*ll_free_nodef)(ll_node *to_free);
typedef void (*ll_fprintf_nodef)(FILE *fp, ll_node *to_print);
typedef void (*ll_foreach_nodef)(ll_node *node, void *stateptr);

typedef struct {
    ll_free_nodef    free;
    ll_fprintf_nodef fprintf;
} ll_callbacks;

typedef struct {
    ll_node      *head;
    int          size;
    ll_callbacks callbacks;
} ll;

// Create an empty linked list with data type of size `size`
// and callbacks given by `callbacks`.
extern ll *ll_make(int size, ll_callbacks callbacks);

// Insert a new node with value `value` into the linked list given by `linkedlist`.
extern void ll_add(ll *linkedlist, void *value);

// Removes the `n`th element of the linked list given by `linkedlist` and returns a pointer to it.
// If n < 0, then remove the tail.
extern ll_node *ll_pop(ll *linkedlist, int n);

// Returns the `n`th element of the zero-indexed linked list `linkedlist`.
extern ll_node *ll_get(ll *linkedlist, int n);

// Calculates the current length of the linked list.
extern int ll_length(ll *linkedlist);

// Frees the entire linked list.
extern void ll_free(ll *linkedlist);

// Calls the function `*foreach` on each element, modifying a generic state pointer.
//extern void ll_foreach(ll *linkedlist, ll_foreach_nodef foreach);

// Prints the linked list to stdout.
extern void ll_printf(ll *linkedlist);

// Prints the linked list to a FILE *.
extern void ll_fprintf(ll *linkedlist, FILE *file);

// Gets the tail of the linked list, inclusive of the index
extern ll *ll_tail(ll *linkedlist, int i);

#endif
