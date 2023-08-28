#include <stdio.h>
#include <stdbool.h>
#include "ll.h"

#ifndef HM_DEFINED
#define HM_DEFINED

#define HM_CAPACITY 5000

typedef int (*hm_hashf)(void *key);
typedef void (*hm_free_keyf)(void *key);
typedef void (*hm_free_valuef)(void *value);

typedef struct {
    ll             *array[HM_CAPACITY];
    int            key_size;
    int            value_size;
    hm_hashf       hash_func;
    hm_free_keyf   free_key;
    hm_free_valuef free_value;
} hm;

// Creates an empty generic hashmap with key of size `key_size` and value of size `value_size`. (returns hm*)
extern hm* hm_make(int key_size, int value_size, hm_hashf hash_func);

// Sets the value associated with the key `*key` to `*value*`.
extern void hm_put(hm *hashmap, void *key, void *value);

// Gets the value associated with the key `*key`. 
extern void *hm_get(hm *hashmap, void *key);

// Frees the entire hashmap.
extern void hm_free(hm *hashmap);

// Checks if key is in the hashmap
extern bool hm_in(hm *hashmap, void *key);

#endif
