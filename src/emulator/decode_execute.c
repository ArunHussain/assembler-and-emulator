#include "decode_execute.h"

extern bool decode_execute(machine *machine, instruction instruction, int *errcode) {
    *errcode = 0;
    if (instruction == INST_HALT) {
        return true;
    }
    if (instruction == INST_NOP) {
        next_pc(machine);
        return false;
    }

    int op0 = (instruction >> OP0_SHIFT) & OP0_SHIFTED_MASK;

    switch (op0) {
        case 0b1000:
        case 0b1001:
        // data processing instruction - immediate
        de_dpi(machine, instruction);
        next_pc(machine);
        break;

        case 0b0101:
        case 0b1101:
        // data processing instruction - register
        de_dpr(machine, instruction);
        next_pc(machine);
        break;

        case 0b0100:
        case 0b0110:
        case 0b1100:
        case 0b1110:
        // loads and stores - single data transfer instructions
        de_sdt(machine, instruction);
        next_pc(machine);
        break;

        case 0b1010:
        case 0b1011:
        // branches
        de_brc(machine, instruction);
        break;

        default:
        // for now, halt on invalid instruction
        printf("Invalid instruction.\n");
        *errcode = 1;
        return true;
        break;
    }

    return false;
}
