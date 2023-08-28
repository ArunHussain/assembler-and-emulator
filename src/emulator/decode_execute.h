#include "arch.h"
#include "utils.h"

#include "dpi.h"
#include "dpr.h"
#include "brc.h"
#include "sdt.h"

// special instructions
#define INST_HALT        0x8a000000
#define INST_NOP         0b11010101000000110010000000011111

// instruction decoding masks
#define OP0_SHIFTED_MASK 0b1111
#define OP0_SHIFT        25

extern bool decode_execute(machine *machine, instruction instruction, int *errcode);
