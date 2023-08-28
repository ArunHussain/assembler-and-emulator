#include <stdlib.h>
#include <string.h>
#include "readfile.h"
#include "parser.h"
#include "symbol_table_builder.h"
#include "machine_code_writer.h"
#include "ll.h"
#include "hm.h"

#define BASE_LINE_NUMBER_OFFSET 0

static char *skipws(char *str) {
    while (isspace(*str)) str++;
    return str;
}

int main(int argc, char **argv) {
    char *source = argv[1];
    FILE *fp = fopen(source, "r");
    ll *lines = readfile(fp);
    fclose(fp);

    hm *table = build_symbol_table(lines);

    char *dest = argv[2];
    fp = fopen(dest, "w+");

    int i = BASE_LINE_NUMBER_OFFSET;
    for (ll_node *pointer = lines->head; pointer != NULL; pointer = pointer -> next) {
        char *line = (char *) pointer->value;

        // skip lines that consist entirely of white space
        skipws(line);
        if (strlen(line) == 0) continue;

        // skip lines that contain labels
        if (strchr(line, ':') != NULL) continue;

        printf("(debug) LINE: %s\n", line);
        ir_instruction instruction = parse(line, i, table);
        printf("(debug) parse finished\n");
        write_build_machine_code(fp, instruction);
        i++;
    }
    
    fclose(fp);
    return EXIT_SUCCESS;
}
