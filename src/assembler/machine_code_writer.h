#include "ir.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Writes a single instruction's worth of machine code (32 bits) from the
// internal representation of an instruction.
extern void write_build_machine_code(FILE *fp, ir_instruction inst);