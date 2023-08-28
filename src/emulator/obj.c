#include "obj.h"

extern void load_object_file(machine *machine, const char *path) {
    FILE *fp = fopen(path, "r");
    fread(machine->memory, MEMORY_SIZE, 1, fp);
    fclose(fp);
}
