#include "machine.h"
#include <string.h>
#include <stdio.h>

// initializes the machine state
extern void init_machine(machine *machine) {
    memset(machine->memory, 0, MEMORY_SIZE);
    memset(machine->registers, 0, (N_REGISTERS + N_SPECIAL_REGISTERS)*8);
    machine->pstateC = false;
    machine->pstateN = false;
    machine->pstateV = false;
    machine->pstateZ = true;
}

// sets the program counter to the argument `pc`
extern void set_pc(machine *machine, register64 pc) {
    set_register64(machine, PC_INDEX, pc);
}
// increments the counter after each cycle
extern void next_pc(machine *machine) {
    address cur_pc = get_register64(machine, PC_INDEX);
    set_pc(machine, cur_pc + PC_INCREMENT);
}

// procedures to set the processor state values
extern void set_pstateN(machine *machine, bool n) {
    machine->pstateN = n;
}
extern void set_pstateZ(machine *machine, bool z) {
    machine->pstateZ = z;
}
extern void set_pstateC(machine *machine, bool c) {
    machine->pstateC = c;
}
extern void set_pstateV(machine *machine, bool v) {
    machine->pstateV = v;
}

// procedures and functions to manipulate general purpose registers
extern register64 get_register64(machine *machine, register_no r) {
    return machine->registers[r];
}
extern register32 get_register32(machine *machine, register_no r) {
    return get_register64(machine, r); // relies on implicit type conversion
}
extern void set_register64(machine *machine, register_no r, register64 value) {
    // Ignore any attempt to set the zero register.
    if (r == ZR_INDEX) return;
    machine->registers[r] = value;
}
extern void set_register32(machine *machine, register_no r, register32 value) {
    set_register64(machine, r, (value & REGISTER32_MASK));
}

// procedures and functions to manipulate memory
extern void set_memory(machine *machine, address addr, byte *bytes, int nbytes) {
    byte *memstart = (machine->memory) + addr;
    while ((nbytes--) > 0) (*memstart++) = (*bytes++);
}
extern byte get_byte(machine *machine, address addr) {
    return *(machine->memory + addr);
}
extern void get_bytes(machine *machine, address addr, byte *dest, int nbytes) {
    while (nbytes-- > 0) *dest++ = get_byte(machine, addr++);
}

extern void set_memory_uint(machine *machine, address addr, const int intsize, longint value) {
    byte bytes[intsize];
    memset(bytes, 0, intsize);

    for (int i = 0; i < intsize; i ++) {
        bytes[i] = value & 0xFF;
        value >>= 8;
    }

    set_memory(machine, addr, bytes, intsize);
}
// stores a 1, 2, 4, or 8-byte integer in memory in little endian format with sign extension.
extern void set_memory_sint(machine *machine, address addr, const int intsize, longint value, int bit_count) {
    set_memory_uint(machine, addr, intsize, sign_ext(value, bit_count));
}
// prints nbytes of memory starting at addr to stdout.
extern void memory_dump(machine *machine, address addr, int nbytes) {
    for (int i = addr; i < addr + nbytes; i++) {
        printf("%d: %u\n", i, get_byte(machine, i));
    }
}

