#include "arch.h"
#include "utils.h"

#define REGISTER32_MASK 0x00000000FFFFFFFF

// initializes the machine state
extern void init_machine(machine *machine);

// sets the program counter to the argument `pc`
extern void set_pc(machine *machine, register64 pc);
// increments the counter after each cycle
extern void next_pc(machine *machine);

// procedures to set the processor state values
extern void set_pstateN(machine *machine, bool n);
extern void set_pstateZ(machine *machine, bool z);
extern void set_pstateC(machine *machine, bool c);
extern void set_pstateV(machine *machine, bool v);

// procedures and functions to manipulate general purpose registers
extern register64 get_register64(machine *machine, register_no r);
extern register32 get_register32(machine *machine, register_no r);
extern void set_register64(machine *machine, register_no r, register64 value);
extern void set_register32(machine *machine, register_no r, register32 value);

// procedures and functions to manipulate memory
extern void set_memory(machine *machine, address addr, byte *bytes, int nbytes);
extern byte get_byte(machine *machine, address addr);
extern void get_bytes(machine *machine, address addr, byte *dest, int nbytes);
// stores a 1, 2, 4, or 8-byte integer in memory in little endian format with no sign extension.
extern void set_memory_uint(machine *machine, address addr, const int intsize, longint value);
// stores a 1, 2, 4, or 8-byte integer in memory in little endian format with sign extension.
extern void set_memory_sint(machine *machine, address addr, const int intsize, longint value, int bit_count);
// prints nbytes of memory starting at addr to stdout.
extern void memory_dump(machine *machine, address addr, int nbytes);

