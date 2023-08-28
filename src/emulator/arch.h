#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef IS_ARCH_DEFINED
#define IS_ARCH_DEFINED true

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

typedef uint8_t  byte;
typedef uint32_t register32;
typedef uint64_t register64;
// longest integer type needed
typedef uint64_t longint;
// memory address type
typedef uint32_t address;
// register number type
typedef uint8_t  register_no;
// binary instruction type
typedef uint32_t instruction;

// the emulated machine structure
typedef struct {
    byte memory[MEMORY_SIZE];
    register64 registers[N_REGISTERS + N_SPECIAL_REGISTERS];
    bool pstateN;
    bool pstateZ;
    bool pstateC;
    bool pstateV;
    
} machine;

#endif
