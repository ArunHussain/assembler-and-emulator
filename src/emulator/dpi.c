#include "dpi.h"

static void de_dpi_ar(machine *machine, int sf, int opc, int operand, int rd); // arithmetic
static void de_dpi_wm(machine *machine, int sf, int opc, int operand, int rd); // wide move
static void update_condition_flags(machine *machine, long long op1, long long op2, int sf, bool add); // set flags
static void set_state_N_Z(machine *m1, long long result, int sf);

// decode and execute data processing instructions - immediate.
extern void de_dpi(machine *machine, instruction instruction) {
    int rd, operand, opi, op0, opc, sf;
    int *dest[6] = {&rd, &operand, &opi, &op0, &opc, &sf};
    int spec[6]  = {5,   18,       3,    3,    2,    1};
    decode_segments(instruction, dest, spec, 6);

    if (opi == DPI_OPI_AR) {
        de_dpi_ar(machine, sf, opc, operand, rd);
    } else {
        de_dpi_wm(machine, sf, opc, operand, rd);
    }
}
static void de_dpi_ar(machine *machine, int sf, int opc, int operand, int rd) {
    long long res;
    instruction todec_operand = operand;
    int rn, imm12, sh;
    int *dest[3] = {&rn, &imm12, &sh};
    int spec[3]  = {5,   12,     1};
    decode_segments(todec_operand, dest, spec, 3);

    register64 op1, op2;

    if (sf == 1) {
        op1 = get_register64(machine, rn);
    } else {
        op1 = get_register32(machine, rn);
    }

    if (sh == 1) {
        op2 = imm12 << DPI_AR_SH_SHIFT;
    } else {
        op2 = imm12;
    }

    switch (opc) {
        case DPI_AR_OPC_ADD:
            res = op1 + op2;
            break;
        case DPI_AR_OPC_ADDS:
            res = op1 + op2;
            update_condition_flags(machine, op1, op2, sf, true);
            break;
        case DPI_AR_OPC_SUB:
            res = op1 - op2;
            break;
        case DPI_AR_OPC_SUBS:
            res = op1 - op2;
            update_condition_flags(machine, op1, op2, sf, false);
            break;
        default:
            break;
    }

    if (sf == 1) {
        set_register64(machine, rd, res);
    } else {
        set_register32(machine, rd, (register32) res);
    }


} // arithmetic

static void de_dpi_wm(machine *machine, int sf, int opc, int operand, int rd) {
    instruction todec_operand = operand;
    unsigned long long res, mask, op;
    mask = 0xFFFFFFFFFFFFFFFF;

    int hw, imm16;
    int *dest[2] = {&imm16, &hw};
    int spec[3]  = {16,     2};
    decode_segments(todec_operand, dest, spec, 2);

    unsigned long long shift = hw * DPI_WM_HW_SHIFT;
    op = ((unsigned long long) imm16) << shift;
    // printf("op: %016llx\n", op);

    if (sf == 1) {
      res = get_register64(machine, rd);
    } else {
      res = get_register32(machine, rd);
    }

    switch (opc) {
        case DPI_WM_OPC_MOVN:
            res = ~op;
            break;
        case DPI_WM_OPC_MOVZ:
            res = op;
            break;
        case DPI_WM_OPC_MOVK:
            // clear section
            mask ^= (((unsigned long long) DPI_WM_MOVK_MASK) << shift);
            res &= mask;
            // replace section
            res += op;
            break;
        default:
            break;
    }

    if (sf == 1) {
        set_register64(machine, rd, res);
    } else {
        set_register32(machine, rd, (register32) res);
    }
} // wide move

static void set_state_N_Z(machine *m1, long long result, int sf) {
    bool sign_bit;
    if (sf) {
        sign_bit = (result >> 63) & 0b1; // 64 bit case.
    } else {
        sign_bit = (result >> 31) & 0b1; 
    }  

    set_pstateN(m1, sign_bit);
    set_pstateZ(m1, result == 0);
}

static void update_condition_flags(machine *machine, long long op1, long long op2, int sf, bool add) {
    long long res;
    int op1_sign, op2_sign, res_sign;
    bool s_of, us_of;

    unsigned long us_op1 = (unsigned long) op1;
    unsigned long us_op2 = (unsigned long) op2;

    int sign_shift = (sf == 1 ? 63 : 31);

    if (add) {

        // addition
        res = op1 + op2;


        // shifting right to get the sign bit
        res_sign = res >> sign_shift;
        op1_sign = op1 >> sign_shift;
        op2_sign = op2 >> sign_shift;

        // overflow conditions
        s_of = (op1_sign == op2_sign) && (res_sign != op1_sign);
        us_of = (us_op1 + us_op2) < min(us_op1, us_op2);

        set_state_N_Z(machine, res, sf);
        set_pstateC(machine, us_of);
        set_pstateV(machine, s_of);
         
    } else {

        // subtraction
        res = op1 - op2;

        // overflow conditions
        us_of = !(us_op1 < us_op2);
        s_of =  (op1 > 0 && op2 < 0 && res < 0) || (op1 < 0 && op2 > 0 && res > 0);

        set_state_N_Z(machine, res, sf);

        set_pstateC(machine, us_of);
        set_pstateV(machine, s_of);
    }
}
