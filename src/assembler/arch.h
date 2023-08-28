#ifndef ARCH_DEFINED
#define ARCH_DEFINED

#include <stdio.h>
#include <stdint.h>

/////////////////////////////////////////////////////////////
// constants and typedefs relating to the emulated machine //
/////////////////////////////////////////////////////////////

#define MEMORY_SIZE         (1 << 21)
#define N_REGISTERS         31
// excluding PSTATE, since it is implemented separately
#define N_SPECIAL_REGISTERS 3
// indices for the special pointers
#define ZR_INDEX            31
#define PC_INDEX            32
#define SP_INDEX            33
// program counter increment after each step
#define PC_INCREMENT        4

typedef uint8_t  byte_u;
typedef uint32_t register32_u;
typedef uint64_t register64_u;
// longest integer type needed
typedef uint64_t longuint_u;
// memory address type
typedef uint32_t address_u;
// register number type
typedef uint8_t  register_no_u;
// binary instruction type
typedef uint32_t instruction_u;

#endif