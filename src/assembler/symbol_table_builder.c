#include "symbol_table_builder.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "arch.h"

static int hashfunc(void *voidkey) {
    char *key = (char *) voidkey;
    int total = (int) *key;
    while (*key++ != '\0') {
        total += *key;
    }
    return (total % 5000);
}

// static char *skipws(char *str) {
//     while (isspace(*str)) str++;
//     return str;
// }

static bool allspace(char *str) {
    if (strlen(str) == 0) return true;
    do {
        if (!isspace(*str)) return false;
    } while (*str++ != '\0');
    return true;
}

hm *build_symbol_table(ll *lines) {
    // Oh this is a problem ... Stuff needs to be stored on heap, so we must free it later.
    // accept a second argument - or more likely, make it return an address_u *, and accept
    // a hashmap pointer, which it then sets.
    int n_labels = 0;
    for (ll_node *pointer = lines->head; pointer != NULL; pointer = pointer -> next) {
        char *line = (char *) pointer->value;
        if (strchr(line, ':') != NULL) {
            n_labels++;
        }
    }

    const int regs = N_REGISTERS * 2 + 2;
    address_u *mem = malloc((n_labels + regs) * sizeof(address_u));
    int mem_index = 0;

    hm *table = hm_make(sizeof(char *), sizeof(address_u), &hashfunc);
    int i = 0;
    printf("(debug) BUILD_SYMBOL_TABLE\n");
    for (ll_node *pointer = lines->head; pointer != NULL; pointer = pointer -> next) {
        char *line = (char *) pointer->value;

        // skip lines that consist entirely of white space
        // char *nws_line = skipws(line);
        if (allspace(strdup(line))) continue;
        // printf(":::: allspace(strdup(line)) = %d\n", allspace(strdup(line)));

        char *lineptr = strdup(line);
        printf("(debug) BUILD_SYMBOL_TABLE: line = %s\n", lineptr);
        if (strchr(lineptr, ':') != NULL) {
            char *label = strtok(lineptr, ":");
            mem[mem_index] = i * PC_INCREMENT;
            hm_put(table, label, mem + mem_index);
            printf("(debug) \tlabel = %s, address = %08x\n", label, mem[mem_index]);
            mem_index++;
        } else {
            i ++;
        }
    }

    // Put in register references
    for (register_no_u reg = 0; reg < N_REGISTERS; reg++) {
        char *buffer = malloc(sizeof(char) * 4);
        
        memset(buffer, 0, sizeof(char) * 4);
        sprintf(buffer, "x%d", reg);
        mem[mem_index] = reg;
        // printf("mem: %d\n", *mem);
        hm_put(table, strdup(buffer), mem + mem_index);
        mem_index++;

        memset(buffer, 0, sizeof(char) * 4);
        sprintf(buffer, "w%d", reg);
        mem[mem_index] = reg;
        hm_put(table, strdup(buffer), mem + mem_index);
        mem_index++;

        free(buffer);
    }

    // Also deal with the zero register
    mem[mem_index] = 31;
    hm_put(table, "wzr", mem + mem_index);
    mem_index++;

    mem[mem_index] = 31;
    hm_put(table, "xzr", mem + mem_index);
    mem_index++;
    
    address_u test = *((address_u *) hm_get(table, "x0"));
    printf("(debug) BUILD_SYMBOL_TABLE: hm_get(table, \"x0\") = %u\n", test);

    return table;
}
