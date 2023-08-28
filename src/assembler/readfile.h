#include <stdio.h>
#include "ll.h"

// Reads the .s file at the provided file pointer `fp` and produces a
// linked list of lines, consolidating any labels with the line that
// follows them.
extern ll *readfile(FILE *fp);
