#include "arch.h"
#include "utils.h"
#include "machine.h"
#include <stdbool.h>
 
#define ADD_IDENTIFIER  0
#define ADDS_IDENTIFIER 1
#define SUB_IDENTIFIER  2
#define SUBS_IDENTIFIER 3

#define LLS_IDENTIFIER  0
#define LRS_IDENTIFIER  1
#define ARS_IDENTIFIER  2
#define ROR_IDENTIFIER  3

#define AND_IDENTIFIER  0
#define OR_IDENTIFIER   1
#define XOR_IDENTIFIER  2
#define ANDS_IDENTIFIER 3
 
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef abs
#define abs ((a) < 0 ? (-1 * (a)) : (a))
#endif

// decode and execute data processing instructions - register.
extern void de_dpr(machine *machine, instruction instruction);
 