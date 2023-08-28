#include "tokenizer.h"
#include <ctype.h>

// static char *skipws(char *str) {
//     while (isspace(*str)) str++;
//     return str;
// }

static void node_fprintf(FILE *fp, ll_node *node) {
    fprintf(fp, "node: %s\n", ((char *) node->value));
}

static void node_free(ll_node *node) {
    free(node->value);
    free(node);
}

extern ll *tokenize(char *line) {
    ll_callbacks *cb = (ll_callbacks *) malloc(sizeof(ll_callbacks));
    cb->fprintf = &node_fprintf;
    cb->free = &node_free;
    ll *tokens_h = ll_make(TOKEN_SIZE, *cb);

    // ll may be altered later
    char *delimiters = " ,";
    // uncomment to skip th first token (the instruction)
    char *token;
    // token = strtok(NULL, delimiters);

    // skip the label, if any
    // no need to do this, as it is already done in the parser
    // strtok(line, ":");
    // skipws(line);

    printf("(debug) \tTOKENIZE REMAINING LINE: (%s)\n", line);

    token = strtok(line, delimiters);
    ll_add(tokens_h, token);
    printf("(debug) \tFIRST TOKEN: %s\n", token);

    while ((token = strtok(NULL, delimiters)) != NULL) {
        ll_add(tokens_h, token);
        printf("(debug) \tTOKEN: %s\n", token);
    }

    return tokens_h;
}
