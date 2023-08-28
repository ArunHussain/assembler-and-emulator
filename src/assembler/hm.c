#include "hm.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    void *key;
    void *value;
} kv_pair;

static kv_pair *make_pair(void *key, void *value) {
    kv_pair *pair = malloc(sizeof(kv_pair));
    pair->key = key;
    pair->value = value;
    return pair;
}

void free_key(void* key){
    free(key);
}
void free_value(void* value){
    free(value);
}

hm* hm_make(int key_size, int value_size, hm_hashf hashfunc){
    hm* hashmap = (hm*)malloc(sizeof(hm));
    hashmap->key_size = key_size;
    hashmap->value_size = value_size;
    hashmap->hash_func = hashfunc;
    hashmap->free_key = free_key;
    hashmap->free_value = free_value;
    int i = 0;
    for (i=0; i<HM_CAPACITY;i++){
	    ll_callbacks callbacks = {free_value, NULL};
        hashmap->array[i] = ll_make(value_size, callbacks);
    }
    return(hashmap);
}

extern void hm_put(hm *hashmap, void *key, void *value) {
    int index = hashmap->hash_func(key);
    ll *linkedlist = hashmap->array[index];
    kv_pair *pair = make_pair(key, value);
    if (linkedlist == NULL) {
        ll_callbacks callbacks = {NULL, NULL};
        linkedlist = ll_make(sizeof(kv_pair), callbacks);
    }
    for (ll_node *pointer = linkedlist->head; pointer != NULL; pointer = pointer -> next) {
        kv_pair *pair = pointer->value;
        // eventually replace with eq callback
        char *testkey = pair->key;
        if (strcmp(testkey, key) == 0) {
            pair->value = value;
            return;
        }

    }

    ll_add(linkedlist, pair);
    hashmap->array[index] =  linkedlist;
}

extern void *hm_get(hm *hashmap, void *key) {
    int index = hashmap->hash_func(key);
    ll *linkedlist = hashmap->array[index];

    if (linkedlist == NULL) return NULL;

    for (ll_node *pointer = linkedlist->head; pointer != NULL; pointer = pointer -> next) {
        kv_pair *pair = pointer->value;
        // eventually replace with eq callback
        char *testkey = pair->key;
        if (strcmp(testkey, key) == 0) {
            return pair->value;
        }
    }

    return NULL;

    // printf("%p\n", ll_get(linkedlist, 0));
    // return(ll_get(linkedlist,0));
}

extern bool hm_in(hm *hashmap, void *key) {
    int index = hashmap->hash_func(key);
    ll *linkedlist = hashmap->array[index];

    if (linkedlist == NULL) {
        return false;
    }

    for (ll_node *pointer = linkedlist->head; pointer != NULL; pointer = pointer -> next) {
        kv_pair *pair = pointer->value;
        // eventually replace with eq callback
        char *testkey = pair->key;
        if (strcmp(testkey, key) == 0) {
            return true;
        }
    }

    return false;
}

extern void hm_free(hm *hashmap){
    for (int i=0; i<HM_CAPACITY; i++){
        hashmap->free_value(hashmap->array[i]);
    }
    free_key(hashmap->array);

}
