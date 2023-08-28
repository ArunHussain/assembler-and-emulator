#include "machine_code_writer.h"
#include "ir.h"
#include "assert.h"

#include <stdio.h>
#include <string.h>

#define N_BRANCH_TESTS 3
#define N_SDT_TESTS 5
#define N_DP_TESTS 9

void general_test(
    char *path,
    instruction_u *expected,
    ir_instruction *instructions,
    int ntests) {

    // clear the file
    fclose(fopen(path, "w"));

    // set up a file, because we do not have fmemopen
    FILE *fp = fopen(path, "w+");
    
    // encode each of the instructions
    for (int i = 0; i < ntests; i++) write_build_machine_code(fp, *instructions++);

    // close file
    fclose(fp);

    // read it again, because we do not have `fmemopen` on macOS.
    // set up the buffer
    int buffsize = sizeof(instruction_u) * ntests;
    byte_u buffer[buffsize];
    memset(buffer, 0, buffsize);

    // read the file
    fp = fopen(path, "r");
    fread(buffer, sizeof(byte_u), ntests * sizeof(instruction_u), fp);
    fclose(fp);

    // since fread respects endianness, we have to do something truly stupid
    instruction_u binsts[ntests];
    memset(binsts, 0, buffsize);

    for (int i = 0; i < ntests; i++) {
        for (int j = 0; j < sizeof(instruction_u); j++) {
            int offset = i * sizeof(instruction_u) + j;
            binsts[i] += buffer[offset] << ((sizeof(instruction_u)-1-j)*8);
        }
    }

    // compare the produced and expected values
    for (int i = 0; i < ntests; i++) {
        printf("test %d: ", i+1);
        if (binsts[i] == expected[i]) printf("success (%08x)\n", expected[i]);
        else {
            printf("incorrect\n");
            printf("expected: %08x\n", expected[i]);
            printf("got:      %08x\n", binsts[i]);
        }
    }
}

void test_branch() {
    // instructions sourced from
    // general: 
    //  1. b01.s    (b)
    //  2. beq01.s  (b.eq)
    //  3. bne01.s  (b.ne)
    // generated/br
    //  4. br0.s    (br)
    // generated/bcond
    //  5. bcond1.s (b.ge)
    //  6. bcond4.s (b.gt)
    //  7. bcond6.s (b.al)
    //  8. bcond7.s (b.lt)
    // apparently, there are no tests for b.le, so I guess we'll never know.

    ir_instruction instructions[N_BRANCH_TESTS]; ir_instruction *cinst = instructions;
    instruction_u expected[N_BRANCH_TESTS] = {0x02000014, 0x40000054, 0x80031fd6};

    // general/b01.s    "b foo"
    ir_brc branch_instruction1 = { .type = IR_BRC_LIT, .repr = { .lit = 0x8 } };
    ir_instruction instruction1 = { .type = IR_INST_BRC, .repr = {.brc = branch_instruction1} };
    *cinst++ = instruction1;

    // general/beq01.s   "b.eq foo"
    ir_brc_cnd test2_cnd = { .condition = IR_BRC_EQ, .literal = 0x8 };
    ir_brc branch_instruction2 = { .type = IR_BRC_CND, .repr = {.cnd = test2_cnd} };
    ir_instruction instruction2 = { .type = IR_INST_BRC, .repr = {.brc = branch_instruction2} };
    *cinst++ = instruction2;

    // generated/br/br0.s "br x28"
    ir_brc branch_instruction3 = { .type = IR_BRC_REG, .repr = {.reg = 28} };
    ir_instruction instruction3 = { .type = IR_INST_BRC, .repr = {.brc = branch_instruction3} };
    *cinst++ = instruction3;
    
    general_test("test_mcw_brc.out", expected, instructions, N_BRANCH_TESTS);
}

void test_sdt() {
    // instructions sourced from
    // general
    //  1. general/ldr01 "ldr x2,[x0]" - unsigned immediate offset
    //  2. general/ldr04 "ldr x20, [x5, x15]" - register
    //  3. general/ldr05 "ldr x1,D0" - load from literal
    //  4. general/ldr10 "ldr x1, [x0], #2" - post-indexed
    // generated
    //  5. generated/ldr/ldr5 "ldr w11, [x20, #60]!" - pre-indexed
    ir_instruction instructions[N_SDT_TESTS]; ir_instruction *cinst = instructions;
    instruction_u expected[N_SDT_TESTS] = {
        0x020040f9,
        0xb4686ff8,
        0x61000058,
        0x012440f8,
        0x8bce43b8
    };

    // general/ldr01 "ldr x2,[x0]" - unsigned immediate offset
    ir_sdt_addr_uio uio = { .imm = 0, .xn = 0 };
    ir_sdt_addr addr1 = { .repr = { .uio = uio }, .type = IR_SDT_UIO };
    ir_sdt sdt_instruction1 = { .addr = addr1, .is_load = true, .rt = 2 };
    ir_instruction instruction1 = { .type = IR_INST_SDT, .repr = { .sdt = sdt_instruction1 } };
    *cinst++ = instruction1;

    // general/ldr04 "ldr x20, [x5, x15]" - register
    ir_sdt_addr_reg reg = { .xn = 5, .xm = 15 };
    ir_sdt_addr addr2 = { .type = IR_SDT_REG, .repr = { .reg = reg } };
    ir_sdt sdt_instruction2 = { .addr = addr2, .is_load = true, .rt = 20 };
    ir_instruction instruction2 = { .type = IR_INST_SDT, .repr = { .sdt = sdt_instruction2 } };
    *cinst++ = instruction2;

    // general/ldr05 "ldr x1,D0" - load from literal
    ir_sdt_addr addr3 = { .type = IR_SDT_LIT, .repr = { .lit = 0x3 } };
    ir_sdt sdt_instruction3 = { .addr = addr3, .is_load = true, .rt = 1 };
    ir_instruction instruction3 = { .type = IR_INST_SDT, .repr = { .sdt = sdt_instruction3 } };
    *cinst++ = instruction3;

    // general/ldr10 "ldr x1, [x0], #2" - post-indexed
    ir_sdt_addr_ppi post = { .pre_indexed = false, .simm = 2, .xn = 0 };
    ir_sdt_addr addr4 = { .type = IR_SDT_PPI, .repr = { .ppi = post }};
    ir_sdt sdt_instruction4 = { .addr = addr4, .is_load = true, .rt = 1 };
    ir_instruction instruction4 = { .type = IR_INST_SDT, .repr = { .sdt = sdt_instruction4 } };
    *cinst++ = instruction4;

    // generated/ldr/ldr5 "ldr w11, [x20, #60]!" - pre-indexed
    ir_sdt_addr_ppi pre = { .pre_indexed = true, .simm = 60, .xn = 20 };
    ir_sdt_addr addr5 = { .type = IR_SDT_PPI, .repr = { .ppi = pre }};
    ir_sdt sdt_instruction5 = { .addr = addr5, .is_load = true, .rt = 11 };
    ir_instruction instruction5 = { .type = IR_INST_SDT, .repr = { .sdt = sdt_instruction5 }, .is_32 = true };
    *cinst++ = instruction5;

    general_test("test_mcw_sdt.out", expected, instructions, N_SDT_TESTS);
}

void test_dp() {
    // instructions sourced from
    // generated
    //  1. generated/madd/madd0 "madd x16, x3, x12, x1"
    //  2. generated/add/add0   "add x22, x20, x22, asr #28"
    //  3. generated/sub/sub0   "sub x13, x13, #0x1, lsl #12"
    //  4. generated/adds/adds0 "adds x7, x6, x6, asr #0x11"
    //  5. generated/orr/orr0   "orr x29, x15, x21"
    //  6. generated/ands/ands0 "ands x17, x28, x11, lsr #21"
    //  7. generated/movn/movn0 "movn x14, #0x1f87, lsl #48"
    //  8. generated/movz/movz0 "movz w18, #0x8beb, lsl #0"
    //  9. generated/movk/movk0 "movk w10, #3839, lsl #16"

    ir_instruction instructions[N_DP_TESTS]; ir_instruction *cinst = instructions;
    instruction_u expected[N_DP_TESTS] = {
        0x70040c9b,
        0x9672968b,
        0xad0540d1,
        0xc74486ab,
        0xfd0115aa,
        0x91574bea,
        0xeef0e392,
        0x727d9152,
        0xeadfa172
    };

    // generated/madd/madd0 "madd x16, x3, x12, x1"
    ir_dp_mult madd = { .opcode = IR_DP_OPC_MADD, .rd = 16, .rn = 3, .rm = 12, .ra = 1 };

    ir_dp dp1 = { .type = IR_DP_MULT, .repr = { .mult = madd }};
    ir_instruction instruction1 = { .type = IR_INST_DP, .repr = { .dp = dp1 }};
    *cinst++ = instruction1;

    // generated/add/add0   "add x22, x20, x22, asr #28"
    ir_dp_operand_shift shift2 = { .amount = 28, .type = IR_SHIFT_ASR };
    ir_dp_operand operand2 = { .is_register = true, .shift = shift2, .value = { .rm = 22 }};
    ir_dp_op2 add = { .opcode = IR_DP_OPC_ADD, .rd = 22, .rn = 20, .operand = operand2};

    ir_dp dp2 = { .type = IR_DP_OP2, .repr = { .op2 = add }};
    ir_instruction instruction2 = { .type = IR_INST_DP, .repr = { .dp = dp2 }};
    *cinst++ = instruction2;

    // generated/sub/sub0   "sub x13, x13, #0x1, lsl #12"
    ir_dp_operand_shift shift3 = { .amount = 12, .type = IR_SHIFT_LSL };
    ir_dp_operand operand3 = { .is_register = false, .shift = shift3, .value = { .imm = 0x1 }};
    ir_dp_op2 sub = { .opcode = IR_DP_OPC_SUB, .rd = 13, .rn = 13, .operand = operand3 };

    ir_dp dp3 = { .type = IR_DP_OP2, .repr = { .op2 = sub }};
    ir_instruction instruction3 = { .type = IR_INST_DP, .repr = { .dp = dp3 }};
    *cinst++ = instruction3;

    // generated/adds/adds0 "adds x7, x6, x6, asr #0x11"
    ir_dp_operand_shift shift4 = { .amount = 0x11, .type = IR_SHIFT_ASR };
    ir_dp_operand operand4 = { .is_register = true, .shift = shift4, .value = { .rm = 6 }};
    ir_dp_op2 adds = { .opcode = IR_DP_OPC_ADDS, .rd = 7, .rn = 6, .operand = operand4 };

    ir_dp dp4 = { .type = IR_DP_OP2, .repr = { .op2 = adds }};
    ir_instruction instruction4 = { .type = IR_INST_DP, .repr = { .dp = dp4 }};
    *cinst++ = instruction4;

    // generated/orr/orr0   "orr x29, x15, x21"
    ir_dp_operand_shift shift5 = { .amount = 0, .type = IR_SHIFT_LSL };
    ir_dp_operand operand5 = { .is_register = true, .shift = shift5, .value = { .rm = 21 }};
    ir_dp_op2 orr = { .opcode = IR_DP_OPC_ORR, .rd = 29, .rn = 15, .operand = operand5 };

    ir_dp dp5 = { .type = IR_DP_OP2, .repr = { .op2 = orr }};
    ir_instruction instruction5 = { .type = IR_INST_DP, .repr = { .dp = dp5 }};
    *cinst++ = instruction5;

    // generated/ands/ands0 "ands x17, x28, x11, lsr #21"
    ir_dp_operand_shift shift6 = { .amount = 21, .type = IR_SHIFT_LSR };
    ir_dp_operand operand6 = { .is_register = true, .shift = shift6, .value = { .rm = 11 }};
    ir_dp_op2 ands = { .opcode = IR_DP_OPC_ANDS, .rd = 17, .rn = 28, .operand = operand6 };

    ir_dp dp6 = { .type = IR_DP_OP2, .repr = { .op2 = ands }};
    ir_instruction instruction6 = { .type = IR_INST_DP, .repr = { .dp = dp6 }};
    *cinst++ = instruction6;

    // generated/movn/movn0 "movn x14, #0x1f87, lsl #48"
    ir_dp_operand_shift shift7 = { .amount = 48, .type = IR_SHIFT_LSL };
    ir_dp_operand operand7 = { .is_register = false, .shift = shift7, .value = { .imm = 0x1f87 }};
    ir_dp_op1 movn = { .opcode = IR_DP_OPC_MOVN, .rd = 14, .operand = operand7 };

    ir_dp dp7 = { .type = IR_DP_OP1, .repr = { .op1 = movn }};
    ir_instruction instruction7 = { .type = IR_INST_DP, .repr = { .dp = dp7 }};
    *cinst++ = instruction7;

    // generated/movz/movz0 "movz w18, #0x8beb, lsl #0"
    ir_dp_operand_shift shift8 = { .amount = 0, .type = IR_SHIFT_LSL };
    ir_dp_operand operand8 = { .is_register = false, .shift = shift8, .value = { .imm = 0x8beb }};
    ir_dp_op1 movz = { .opcode = IR_DP_OPC_MOVZ, .rd = 18, .operand = operand8 };

    ir_dp dp8 = { .type = IR_DP_OP1, .repr = { .op1 = movz }};
    ir_instruction instruction8 = { .type = IR_INST_DP, .repr = { .dp = dp8 }, .is_32 = true};
    *cinst++ = instruction8;
    
    // generated/movk/movk0 "movk w10, #3839, lsl #16"
    ir_dp_operand_shift shift9 = { .amount = 16, .type = IR_SHIFT_LSL };
    ir_dp_operand operand9 = { .is_register = false, .shift = shift9, .value = { .imm = 3839 }};
    ir_dp_op1 movk = { .opcode = IR_DP_OPC_MOVK, .rd = 10, .operand = operand9 };

    ir_dp dp9 = { .type = IR_DP_OP1, .repr = { .op1 = movk }};
    ir_instruction instruction9 = { .type = IR_INST_DP, .repr = { .dp = dp9 }, .is_32 = true};
    *cinst++ = instruction9;

    general_test("test_mcw_sdt.out", expected, instructions, N_DP_TESTS);
}

int main(void) {
    test_branch();
    test_sdt();
    test_dp();
    return 0;
}
