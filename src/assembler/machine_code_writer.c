#include "machine_code_writer.h"

static void set_bytes(instruction_u *bin_inst, int hi, int lo, instruction_u bytes);

static instruction_u wbmc_dp(ir_dp *inst, bool is_32);
static instruction_u wbmc_sdt(ir_sdt *inst, bool is_32);
static instruction_u wbmc_brc(ir_brc *inst);
static instruction_u wbmc_nop();
static instruction_u wbmc_dirc(ir_int_dir_u dirc);

// bytes is expected to be exactly hi - lo + 1 long.
// for example, set_bytes(&bin_inst, 28, 26, 0b100)
// should provide the op0 for a dpi instruction.
//
// Another exmaple:
// int main() {
//     instruction_u template = 0u;
//     set_bytes(&template, 7, 0, 0x44);
//     set_bytes(&template, 15, 8, 0x33);
//     set_bytes(&template, 23, 16, 0x22);
//     set_bytes(&template, 31, 24, 0x11);
//     // Should be 0x11223344
//     printf("formatted: %08x\n", template);
// }

static void set_bytes(instruction_u *binst_ptr, int hi, int lo, instruction_u bytes) {
    // need to deal with negative numbers
    // trim down `bytes` first


    instruction_u hilo_1s = (1 << (hi - lo + 1)) - 1;
    instruction_u negmask = ((instruction_u) 0u) | (hilo_1s << lo);
    *binst_ptr &= ~negmask;
    *binst_ptr |= ((bytes & hilo_1s) << lo);
}

void write_build_machine_code(FILE *fp, ir_instruction inst) {
    instruction_u bits32 = 0;
    bool is_32 = inst.is_32;
    // bool already_little_endian = false;

    switch(inst.type) {
        // data processing instructions (dpi and dpr)
        case IR_INST_DP:
            printf("(debug) bits32 = wbmc_dp(&inst->repr.dp, is_32);\n");
            bits32 = wbmc_dp(&inst.repr.dp, is_32);
        break;

        // single data transfer instructions (sdt)
        case IR_INST_SDT:
            printf("(debug) bits32 = wbmc_sdt(&inst->repr.sdt, is_32);\n");
            bits32 = wbmc_sdt(&inst.repr.sdt, is_32);
        break;

        // branch instructions (brc)
        case IR_INST_BRC:
            printf("(debug) bits32 = wbmc_brc(&inst->repr.brc);\n");
            bits32 = wbmc_brc(&inst.repr.brc);
        break;

        // nop special instruction
        case IR_INST_NOP:
            printf("(debug) bits32 = wbmc_nop();\n");
            bits32 = wbmc_nop();
        break;

        // the .int directive
        case IR_INST_DIRC:
            printf("(debug) bits32 = wbmc_dirc(inst->repr.directive);\n");
            bits32 = wbmc_dirc(inst.repr.directive);
            // already_little_endian = true;
        break;
    }

    // store in little endian format
    byte_u bytes[sizeof(instruction_u)];
    memset(bytes, 0, sizeof(instruction_u));

    for (int i = 0; i < sizeof(instruction_u); i ++) {
        bytes[i] = bits32 & 0xFF;
        bits32 >>= 8;
    }

    // if (already_little_endian) {
    //     // reverse the list
    //     for (int i = 0; i < sizeof(instruction_u)/2; i++) {
    //         byte_u temp = bytes[i];
    //         bytes[i] = bytes[sizeof(instruction_u) - i];
    //         bytes[sizeof(instruction_u) - i] = temp;
    //     }
    // }
    
    // fwrite(bytes, sizeof(instruction_u), 4, fp);
    fwrite(bytes, sizeof(byte_u), sizeof(instruction_u), fp);
}

instruction_u wbmc_dp(ir_dp *inst, bool is_32) {
    instruction_u binst = 0;
    int sf = !is_32;
    ir_dp_operand operand;
    ir_dp_operand_shift shift;

    switch(inst -> type) {
        case IR_DP_MULT:
            printf("(debug)\tIR_DP_MULT\n");
            // multiplication
            binst = 0b00011011000000000000000000000000;
            ir_dp_mult mult = inst->repr.mult;

            int x = (mult.opcode == IR_DP_OPC_MSUB);

            printf("(debug)\t\tMULT: x = %d, rm = %d, rn = %d, rd = %d, ra = %d\n",
                x, mult.rm, mult.ra, mult.rn, mult.rd);

            set_bytes(&binst, 31, 31, sf);
            set_bytes(&binst, 20, 16, mult.rm);
            set_bytes(&binst, 15, 15, x);
            set_bytes(&binst, 14, 10, mult.ra);  
            set_bytes(&binst, 9, 5,   mult.rn);
            set_bytes(&binst, 4, 0,   mult.rd);
        break;

        case IR_DP_OP2:
            printf("(debug)\tIR_DP_OP2\n");
            // arithmetic and logical operations (dpr and dpi)
            ; // TODO: This makes no sense, but for some reason won't compile without it.
            ir_dp_op2 op2 = inst->repr.op2;
            operand = op2.operand;
            shift = op2.operand.shift;
            ir_shift_type shift_type = op2.operand.shift.type;

            if (operand.is_register) {
                printf("(debug)\tDPR (arithmetic and logical)\n");
                // dpr (arithmetic and logical)
                binst = 0b00001010000000000000000000000000;
                // bop is the binary operand
                int opc = 0, opr = 0, bop = shift.amount;
                if (op2.opcode >= IR_DP_OPC_AND && op2.opcode <= IR_DP_OPC_BICS) {
                    // logical
                    int opcn = op2.opcode - IR_DP_OPC_AND;
                    opc      = opcn >> 1;
                    int n    = opcn & 1;
                    opr      = (shift_type << 1) + n;
                    
                    printf("(debug)\t\tDPR (Logical): opc = %d, opr = %d\n", opc, opr);
                } else {
                    // arithmetic
                    opc      = op2.opcode - IR_DP_OPC_ADD;
                    opr      = 0b1000 + (shift_type << 1);
                }
                printf("(debug)\t\tDPR (AR/L): rm = %d, bop = %d\n",
                    op2.operand.value.rm, bop);
                set_bytes(&binst, 30, 29, opc);
                set_bytes(&binst, 24, 21, opr);
                set_bytes(&binst, 20, 16, op2.operand.value.rm);
                set_bytes(&binst, 15, 10, bop);
            } else {
                printf("(debug) \tDPI (arithmetic)\n");
                // dpi (arithmetic)
                binst = 0b00010001000000000000000000000000;
                printf("(debug)\t\tDPI (AR): opcode = %d, shift.amount = %d, imm = %d\n",
                    op2.opcode - IR_DP_OPC_ADD, shift.amount, op2.operand.value.imm);
                set_bytes(&binst, 30, 29, op2.opcode - IR_DP_OPC_ADD);
                set_bytes(&binst, 22, 22, (shift.amount > 0));
                set_bytes(&binst, 21, 10, op2.operand.value.imm);
            }
            set_bytes(&binst, 31, 31, sf);
            set_bytes(&binst, 9, 5,   op2.rn);
            set_bytes(&binst, 4, 0,   op2.rd);  
        break;

        case IR_DP_OP1:
            printf("(debug)\tIR_DP_OP1\n");
            // wide moves (single operand with destination)
            binst = 0b00010010100000000000000000000000;
            ir_dp_op1 op1 = inst->repr.op1;
            operand = op1.operand;
            shift = op1.operand.shift;

            int opc = op1.opcode - IR_DP_OPC_MOVN;
            if (opc > 0) opc++; // Because movz is 10 instead of 01.

            printf("(debug)\t\tWM: opc=%d, shift.amount=%d, imm=%d, rd=%d\n",
                opc, shift.amount, operand.value.imm, op1.rd);

            set_bytes(&binst, 31, 31, sf);
            set_bytes(&binst, 30, 29, opc);
            set_bytes(&binst, 22, 21, shift.amount / 16);
            set_bytes(&binst, 20, 5, operand.value.imm);
            set_bytes(&binst, 4, 0, op1.rd);
        break;
    }

    return binst;
}

instruction_u wbmc_sdt(ir_sdt *inst, bool is_32) {
    instruction_u binst = 0;
    int sf = !is_32;

    ir_sdt_addr addr = inst->addr;
    
    if (addr.type == IR_SDT_LIT) {
        // load literal
        binst = 0b00011000000000000000000000000000;
        printf("(debug)\tIR_SDT_LIT (with literal %d) \n", addr.repr.lit);
        set_bytes(&binst, 23, 5, addr.repr.lit);
    } else {
        // load/store with other addressing mode
        binst = 0b10111000000000000000000000000000;
        printf("(debug)\t!IR_SDT_LIT (with type %d)\n", addr.type);

        set_bytes(&binst, 24, 24, addr.type == IR_SDT_UIO);
        set_bytes(&binst, 22, 22, inst->is_load);
        int xn = 0;

        switch (addr.type) {
            case IR_SDT_REG:
                set_bytes(&binst, 21, 21, 1);
                set_bytes(&binst, 20, 16, addr.repr.reg.xm);
                set_bytes(&binst, 15, 10, 0b011010);
                xn = addr.repr.reg.xn;

                printf("(debug)\tIR_SDT_REG: xm = %d, xn = %d\n", addr.repr.reg.xm, xn);
            break;

            case IR_SDT_PPI:
                set_bytes(&binst, 21, 21, 0);
                set_bytes(&binst, 20, 12, addr.repr.ppi.simm);
                set_bytes(&binst, 11, 11, addr.repr.ppi.pre_indexed);
                set_bytes(&binst, 10, 10, 1);
                xn = addr.repr.ppi.xn;

                printf("(debug)\tIR_SDT_PPI: pre_indexed = %d, simm = %d, xn = %d\n", 
                    addr.repr.ppi.pre_indexed, addr.repr.ppi.simm, xn);
            break;

            case IR_SDT_UIO:
                set_bytes(&binst, 21, 10, addr.repr.uio.imm);
                xn = addr.repr.uio.xn;

                printf("(debug)\tIR_SDT_UIO: imm = %d, xn = %d\n", 
                    addr.repr.uio.imm, xn);
            break;

            case IR_SDT_LIT:
                // should never happen
            break;
        }

        set_bytes(&binst, 9, 5, xn);
    }

    set_bytes(&binst, 30, 30, sf);
    set_bytes(&binst, 4, 0, inst->rt);
    printf("(debug)\tIR_SDT: rt = %d\n", inst->rt);

    return binst;
}

instruction_u wbmc_brc(ir_brc *inst) {
    instruction_u binst = 0;
    
    switch(inst -> type) {
        case IR_BRC_LIT:
            // literal
            // printf("branch: literal (0x%08x)\n", inst->repr.lit);
            binst = 0b00010100000000000000000000000000;
            set_bytes(&binst, 25, 0, inst->repr.lit/4);
            printf("(debug)\tIR_BRC_LIT: addr = %d\n", inst->repr.lit/4);
        break;

        case IR_BRC_REG:
            // register
            binst = 0b11010110000111110000000000000000;
            set_bytes(&binst, 9, 5, inst->repr.reg);
        break;

        case IR_BRC_CND:
            // conditional
            binst = 0b01010100000000000000000000000000;
            ir_brc_cond condition = inst->repr.cnd.condition;
            int literal = inst->repr.cnd.literal/4;
            int bcond = condition - IR_BRC_EQ;
            // logic to transform bcond into the actual binary condition
            if (bcond > 1) bcond += 0b1000; // conditions after GE starts with a 1
            set_bytes(&binst, 23, 5, literal);
            set_bytes(&binst, 3, 0, bcond);
        break;
    }

    return binst;
}

instruction_u wbmc_nop() {
    return 0b11010101000000110010000000011111;
}

instruction_u wbmc_dirc(ir_int_dir_u dirc) {
    printf("(debug)\tDIRECTIVE: %d\n", dirc);
    return dirc;
}
