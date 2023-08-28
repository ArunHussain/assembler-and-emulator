#include "brc.h"

static void brc_uncond(machine *machine, instruction instruction);
static void brc_reg(machine *machine, instruction instruction);
static void brc_cond(machine *machine, instruction instruction);

void de_brc(machine *machine, instruction instruction) {
    uint32_t shifted = instruction >> 26;
    if (shifted == 5) {
        brc_uncond(machine, instruction);
    } else if (shifted == 53) {
        brc_reg(machine, instruction);
    } else if (shifted == 21) {
        brc_cond(machine, instruction);
    }
}

static void brc_uncond(machine *machine, instruction instruction) {
    uint32_t simm26_mask = (1 << 26) - 1;
    uint64_t offset = PC_INCREMENT * sign_ext(instruction & simm26_mask, 26);

    // uint64_t offset = sign_ext(PC_INCREMENT * (instruction & simm26_mask), 26);
    address cur_pc = get_register64(machine, PC_INDEX);
    set_pc(machine, cur_pc + offset);
}

static void brc_reg(machine *machine, instruction instruction) {
    // unsigned xn_mask = 31;
    uint32_t xn_mask = 0b11111;
    uint32_t shifted = instruction >> 5;
    register_no reg_xn = shifted & xn_mask;
    // address jump_to = machine->registers[reg_xn];
    address jump_to = get_register64(machine, reg_xn);
    set_pc(machine, jump_to);
}

static void brc_cond(machine *machine, instruction instruction) {
    int cond = instruction & 15;
    bool execute = false;
    switch (cond) {
    case 0:
        if (machine->pstateZ == true) {
            execute = true;
        }
        break;
    case 1:
        if (machine->pstateZ == false) {
            execute = true;
        }
        break;
    case 10:
        if (machine->pstateN == machine->pstateV) {
            execute = true;
        }
        break;
    case 11:
        if (machine->pstateN != machine->pstateV) {
            execute = true;
        }
        break;
    case 12:
        if (machine->pstateZ == 0 && machine->pstateN == machine->pstateV) {
            execute = true;
        }
        break;
    case 13:
        if (!(machine->pstateZ == 0 && machine->pstateN == machine->pstateV)) {
            execute = true;
        }
        break;
    case 14:
        execute = true;
        break;
    }
    if (execute == true) {
        uint32_t shifted = instruction >> 5;
        unsigned int simm19_mask = (1 << 19) - 1;
        uint64_t offset = PC_INCREMENT * sign_ext(shifted & simm19_mask, 19);
        address cur_pc = get_register64(machine, PC_INDEX);
        set_pc(machine, cur_pc + offset);
    } else {
        next_pc(machine);
    }
}
