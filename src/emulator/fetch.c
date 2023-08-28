#include "fetch.h"

extern instruction fetch(machine *machine) {
    register64 pc = machine->registers[PC_INDEX];
    byte dest[4];
    get_bytes(machine, pc, dest, 4);
    instruction value = join_bits(dest, 4);
    printf("instruction %08x\n", value);
    return value;
}
