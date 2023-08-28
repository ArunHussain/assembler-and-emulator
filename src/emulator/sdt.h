#include "arch.h"
#include "utils.h"
#include "machine.h"

#define SDT_OFFSET_SIZE 12
#define SDT_XM_SHIFT    6
#define SDT_XM_MASK     0b11111
#define SDT_I_MASK      0b1
#define SDT_I_SHIFT     1
#define SDT_SIMM9_SHIFT 2

// decode and execute single data transfer instructions
extern void de_sdt(machine *machine, instruction instruction);

