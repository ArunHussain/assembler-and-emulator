#include "arch.h"
#include <stdbool.h>

#ifndef IR_DEFINED
#define IR_DEFINED

// internal representation for data processing
typedef enum {
    IR_DP_MULT, IR_DP_OP2, IR_DP_OP1
} ir_dp_type;

typedef enum {
    IR_SHIFT_LSL, IR_SHIFT_LSR, IR_SHIFT_ASR, IR_SHIFT_ROR
} ir_shift_type;

typedef struct {
    ir_shift_type type; // lsl, etc.
    int amount;
} ir_dp_operand_shift;

typedef struct {
    union {
        int           imm;
        register_no_u rm;
    } value;
    bool is_register;
    ir_dp_operand_shift shift;
} ir_dp_operand;

typedef enum {
    IR_DP_OPC_ADD,
    IR_DP_OPC_ADDS,
    IR_DP_OPC_SUB,
    IR_DP_OPC_SUBS,
    
    IR_DP_OPC_AND,
    IR_DP_OPC_BIC,
    IR_DP_OPC_ORR,
    IR_DP_OPC_ORN,
    IR_DP_OPC_EOR,
    IR_DP_OPC_EON,
    IR_DP_OPC_ANDS,
    IR_DP_OPC_BICS,

    IR_DP_OPC_MOVN,
    IR_DP_OPC_MOVZ,
    IR_DP_OPC_MOVK,

    IR_DP_OPC_MADD,
    IR_DP_OPC_MSUB
} ir_dp_opcode;

typedef struct {
    ir_dp_opcode  opcode;
    register_no_u rd;
    register_no_u rn;
    register_no_u rm;
    register_no_u ra;
} ir_dp_mult;

typedef struct {
    ir_dp_opcode  opcode;
    register_no_u rd;
    register_no_u rn;
    ir_dp_operand operand;
} ir_dp_op2;

typedef struct {
    ir_dp_opcode  opcode;
    register_no_u rd;
    ir_dp_operand operand;
} ir_dp_op1;

typedef struct {
    union {
        ir_dp_mult mult;
        ir_dp_op2  op2;
        ir_dp_op1  op1;
    } repr;
    ir_dp_type type;
} ir_dp;

// internal representation for single data transfer
typedef enum {
    IR_SDT_UIO, IR_SDT_PPI, IR_SDT_REG, IR_SDT_LIT
} ir_sdt_addr_type;

typedef struct {
    register_no_u xn;
    int           imm;
} ir_sdt_addr_uio; // unsigned immediate offset

typedef struct {
    register_no_u xn;
    int           simm;
    bool          pre_indexed;
} ir_sdt_addr_ppi; // pre-indexed

typedef struct {
    register_no_u xn;
    register_no_u xm;
} ir_sdt_addr_reg; // register

typedef int ir_sdt_addr_lit; // literal (load only)

typedef struct {
    union {
        ir_sdt_addr_uio uio;
        ir_sdt_addr_ppi ppi;
        ir_sdt_addr_reg reg;
        ir_sdt_addr_lit lit;
    } repr;
    ir_sdt_addr_type type; // encodes different addressing modes.
} ir_sdt_addr;

typedef struct {
    bool          is_load;
    register_no_u rt;
    ir_sdt_addr   addr;
} ir_sdt;

// internal representation for branch instructions
typedef enum {
    IR_BRC_LIT, IR_BRC_REG, IR_BRC_CND
} ir_brc_type;

typedef enum {
    IR_BRC_EQ,
    IR_BRC_NE,
    IR_BRC_GE,
    IR_BRC_LT,
    IR_BRC_GT,
    IR_BRC_LE,
    IR_BRC_AL
} ir_brc_cond;

typedef address_u ir_brc_lit; // unconditional literal

typedef register_no_u ir_brc_reg; // unconditional register

typedef struct {
    ir_brc_cond condition;
    address_u   literal;
} ir_brc_cnd; // conditional

typedef struct {
    union {
        ir_brc_lit lit;
        ir_brc_cnd cnd;
        ir_brc_reg reg;
    } repr;
    ir_brc_type type;
} ir_brc;

// directives
typedef uint32_t ir_int_dir_u;  // DEBUG: UNSIGNED

// ir stands for internal representation

typedef enum {
    IR_INST_DP,
    IR_INST_SDT,
    IR_INST_BRC,
    IR_INST_NOP,
    IR_INST_DIRC,
} ir_type;

typedef struct {
    union {
        ir_dp dp;
        ir_sdt sdt;
        ir_brc brc;
        ir_int_dir_u directive;
    } repr;
    // should include NOP and is_directive.
    ir_type type; // making it enum?
    bool is_32;
} ir_instruction;

#endif
