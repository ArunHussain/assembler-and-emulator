#include "arch.h"
#include "utils.h"
#include "machine.h"

#define DPI_OPI_AR 0b010

#define DPI_AR_OPC_ADD     0b00
#define DPI_AR_OPC_ADDS    0b01
#define DPI_AR_OPC_SUB     0b10
#define DPI_AR_OPC_SUBS    0b11

#define DPI_WM_OPC_MOVN    0b00
#define DPI_WM_OPC_MOVZ    0b10
#define DPI_WM_OPC_MOVK    0b11

// execution constants
#define DPI_AR_SH_SHIFT    12
#define DPI_AR_SPECIAL_RN  0b11111
#define DPI_AR_SPECIAL_RD  0b11111

#define DPI_WM_HW_SHIFT    16
#define DPI_WM_MOVK_MASK   0xFFFF

// macro helper functions
#define min(a, b)          a > b ? b : a
 
 
// decode and execute data processing instructions - immediate.
extern void de_dpi(machine *machine, instruction instruction);
