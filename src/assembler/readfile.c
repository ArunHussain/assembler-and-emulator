#include "readfile.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024

// static bool is_label(char *line) {
//     return (strchr(line, ':') != NULL);
// }

static char *skipws(char *str) {
    while (isspace(*str)) str++;
    return str;
}

static bool allspace(char *str) {
    if (strlen(str) == 0) return true;
    do {
        if (!isspace(*str)) return false;
    } while (*str++ != '\0');
    return true;
}

ll *readfile(FILE *fp){
    ll_callbacks callbacks = {NULL, NULL};
    ll *linkedlist = ll_make(sizeof(char *), callbacks);

    char line[1024]; //how many characters max in line of assembly
    // char label_line[1024];
    // bool was_label = false;

    // memset(label_line, 0, 1024 * sizeof(char));

    while(fgets(line, 1024, fp) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        // replace CRLF carraige returns with null string terminators
        if (line[strlen(line) - 2] == '\r') {
            line[strlen(line) - 2] = '\0';
        }

        // skip all whitespace lines
        char *testptr = line;
        if (allspace(testptr)) continue;

        char *nws_line = skipws(line);

        // if (is_label(nws_line)) {
        //     strcpy(label_line, nws_line);
        //     was_label = true;
        //     continue;
        // }
        
        // if (was_label) {
        //     strcat(label_line, " ");
        //     strcat(label_line, nws_line);
        //     strcpy(nws_line, label_line);
        //     memset(label_line, 0, 1024 * sizeof(char));
        //     was_label = false;
        // }

	    ll_add(linkedlist,  strdup(nws_line));
        memset(line, 0, 1024 * sizeof(char));
    }

    return(linkedlist);
}


