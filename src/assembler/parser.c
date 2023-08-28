#include "parser.h"

static ir_instruction parse_dir(char *line);
static ir_instruction parse_label(char *line, int line_no, hm *st);
static ir_instruction parse_inst(char *line, int line_no, hm *st);

static ir_instruction parse_inst_dp(char *line, hm *st);
static ir_instruction parse_inst_brc(char *line, hm *st, int line_no);
static ir_instruction parse_inst_sdt(char *line, int line_no, hm *st);

static int parse_value(char *addr, bool skipfirst);
static address_u parse_literal(char *literal, hm *st, int line_no);

static int select_inst_dp_type(char *opc);
static bool elem(char **arr, char *s, int size);
static bool is_32reg(char *register_name) {
    return strstr(register_name,"w") != NULL;//W-register is 32 bit register
}

// substr(char *origin, char *result, int start, int end)
static char *substr(char *o, char *r, int s, int e) {
    return memcpy(r, o + s, e - s + 1);
}

static char *skipws(char *str) {
    while (isspace(*str)) str++;
    return str;
}

// DEBUG: printing functions
static void print_operand(ir_dp_operand operand) {
    if (operand.is_register) {
        printf("OPERAND/reg, rm = %d", operand.value.rm);
    } else {
        printf("OPERAND/imm, imm = %d", operand.value.imm);
    }
}

// static char *stolower(char *name) {
//     char *buffer = malloc(sizeof(char) * (strlen(name) + 1));
//     char *ptr = buffer;
//     while ((*ptr++ = tolower(*name++)) != '\0');
//     return buffer;
// }

static register_no_u get_register_no(hm *st, char *name) {
    address_u *lno = (address_u *) hm_get(st, name);
    return (register_no_u) *lno;
}

static ir_instruction parse_nop(char *line) {
    ir_instruction inst;
    // suspicious
    inst.is_32 = true;
    inst.type = IR_INST_NOP;
    return inst;
}

extern ir_instruction parse(char *line, int line_no, hm *st) {
    if (strcmp(line, "nop") == 0) {
        printf("(debug) PARSE: nop\n");
        return parse_nop(line);
    }
    if (line[0] == '.') {
        printf("(debug) PARSE: directive\n");
        return parse_dir(line);
    }

    if (strchr(line, ':') != NULL) {
        printf("(debug) PARSE: label\n");
        return parse_label(line, line_no, st);
    }

    printf("(debug) PARSE: instruction\n");
    return parse_inst(line, line_no, st);
}

static ir_instruction parse_dir(char *line) {
    // directive can only be in the form '.int x'
    // where x is a int, which is the second element in
    // ll if we tokenize the line
    ir_instruction inst;
    ll *tokens = (ll *) malloc(sizeof(ll));
    tokens = tokenize(line);
    // inst.repr.directive = parse_value(ll_get(tokens, 1)->value);

    // no need to parse value, we know its format already
    char *value = ll_get(tokens, 1)->value;
    if (value[1] == 'x') {
        inst.repr.directive = strtol(value, NULL, HEX);
    } else {
        inst.repr.directive = atoi(value);
    }
    

    inst.is_32 = true;
    inst.type = IR_INST_DIRC;
    return inst;
}

static ir_instruction parse_label(char *line, int line_no, hm *st) {
    // the assumption is: the line is in the form
    // 'label: inst....'
    // for labels, we only need to remove the labels and
    // parse the inst after it
    ll *tokens = (ll *) malloc(sizeof(ll));
    tokens = tokenize(line);
    char *next_line = (char *) malloc(MAX_LINE_SIZE * sizeof(char));

    for (int i = 1; i < ll_length(tokens); i++) {
        strcat(next_line, " ");
        strcat(next_line, ll_get(tokens, i)->value);
    }

    char *nws_next_line = skipws(next_line);

    // return parse(nws_next_line, line_no + 1, st);
    // do *not* increment line_no here! The point of line_no is not to accurately
    // track which line we are on, but rather what the address would be,
    // so doing this would defeat the point of consolidating label and line in the first place.
    return parse(nws_next_line, line_no, st);
}

static ir_instruction parse_inst(char *line, int line_no, hm *st) {
    char fc = line[0]; char sc = line[1];
    if (fc == 'b' && (sc == '.' || sc == 'r' || sc == ' ')) {
        // branching
        return parse_inst_brc(line, st, line_no);
    }
    if ((fc == 's' && sc == 't') || (fc == 'l' && sc == 'd')) {
        // load or store
        return parse_inst_sdt(line, line_no, st);
    }
    // data processing
    printf("(debug) \tPARSE_INST: data processing\n");
    return parse_inst_dp(line, st);

}

static ir_instruction parse_inst_brc(char *line, hm *st, int line_no) {
    ir_instruction inst;
    ir_brc brc;
    address_u addr;

    ll *tokens = tokenize(line);
    char *opc = ll_get(tokens, 0)->value;
    char *operand = ll_get(tokens, 1)->value;

    if (strcmp(opc, IR_OPC_BRC_LIT) == 0) {
        // literal branching (b label/immediate)
        // no need to divide by 4, machine code writer does it instead.
        addr = parse_literal(operand, st, line_no);
        inst.is_32 = is_32reg(operand);
        brc.repr.lit = addr;
        brc.type = IR_BRC_LIT;
        printf("(debug)\tPARSE_INST_BRC: operand = %s, line_no = %d, addr = %d\n",
            operand, line_no, addr);
    } else if (strcmp(opc, IR_OPC_BRC_REG) == 0) {
        addr = *((address_u *) hm_get(st, operand));
        inst.is_32 = is_32reg(operand);
        brc.repr.reg = addr;
        brc.type = IR_BRC_REG;
    } else {
        // the last case which is b.cond
        char *cond = (char *) malloc(sizeof(char) * MAX_LINE_SIZE);
        int offset = 2;
        ir_brc_cnd brc_cnd;
        strcpy(cond, opc + offset);

        if (strcmp(cond, "eq") == 0) brc_cnd.condition = IR_BRC_EQ;
        if (strcmp(cond, "ne") == 0) brc_cnd.condition = IR_BRC_NE;
        if (strcmp(cond, "ge") == 0) brc_cnd.condition = IR_BRC_GE;
        if (strcmp(cond, "lt") == 0) brc_cnd.condition = IR_BRC_LT;
        if (strcmp(cond, "gt") == 0) brc_cnd.condition = IR_BRC_GT;
        if (strcmp(cond, "le") == 0) brc_cnd.condition = IR_BRC_LE;
        if (strcmp(cond, "al") == 0) brc_cnd.condition = IR_BRC_AL;

        addr = parse_literal(operand, st, line_no);
        brc.type = IR_BRC_CND;
        brc_cnd.literal = addr;
        brc.repr.cnd =  brc_cnd;
        inst.is_32 = true;//I assume it is true when we are branching the .cond
    }

    inst.repr.brc = brc;
    inst.type = IR_INST_BRC;
    return inst;
}

static ir_instruction parse_inst_sdt(char *line, int line_no, hm *st) {

    ir_instruction inst;
    ir_sdt inst_sdt;
    ir_sdt_addr addr;

    ll *tokens = tokenize(line);
    printf("(debug) PARSE_INST_SDT: TOKENIZATION COMPLETE\n");

    // opc is the first element in the tokens
    char *opc = ll_get(tokens, 0)->value;
    char *reg = ll_get(tokens, 1)->value;
    char *xn = ll_get(tokens, 2)->value;

    // don't need to assume - the xn, xm stuff are known to always be 64-bit,
    // since they address memory addresses, so is_32 is only for reg
    inst.is_32 = is_32reg(reg);//I assume the register are all have same type

    bool is_literal = (*xn == '#') || hm_in(st, xn);

    printf("(debug) LITERAL: is %s in symbol table? %d\n", xn, hm_in(st, xn));

    

    // Don't need a conversion function, put in hashmap
    // we want to do this, so shut up, compiler
    // hm_get(st, xn) might be suspicious though
    // register_no_u rt = ((register_no_u) *((address_u *) hm_get(st, xn)));
    // register_no_u rt = ((register_no_u) *((address_u *) hm_get(st, reg)));
    register_no_u rt = get_register_no(st, reg);

    inst_sdt.rt = rt;
    inst_sdt.is_load = !strcmp(opc, IR_OPC_SDT_LDR);

    bool is_pre_ind = false;
    if (ll_length(tokens) == 4) {
        is_pre_ind = strchr(ll_get(tokens, 3)->value, '!');
    }

    bool is_post_ind = (strstr(xn, "[") && strstr(xn, "]")) && ll_length(tokens) > 3;


    bool is_reg = false;
    if (ll_length(tokens) == 4) {
        char *xm = ((char *) ll_get(tokens, 3)->value);
        is_reg = isalpha(xn[1]) && isalpha(xm[0]);
    }

    printf("(debug)\tADDR DET: literal = %d, reg = %d, pre_ind = %d, post_ind = %d\n",
           is_literal, is_reg, is_pre_ind, is_post_ind);

    if (is_literal) { //literal type
        printf("(debug)\tPARSE_INST_SDT: literal addressing mode\n");
        assert(ll_length(tokens) == 3);

        ir_sdt_addr_lit addr_lit = parse_literal(xn, st, line_no)/4;
        printf("(debug)\t\tLITERAL: addr_lit*4 = %08x, addr_lit = %08x\n", addr_lit*4, addr_lit);

        addr.repr.lit = addr_lit;
        addr.type = IR_SDT_LIT;
    } else if (is_reg) {
        printf("(debug)\tPARSE_INST_SDT: register addressing mode\n");
        // is register addressing mode
        assert(ll_length(tokens) == 4);

        ir_sdt_addr_reg addr_reg;

        // char *fixed_xn = NULL;
        // char *fixed_xm = NULL;
        // fixed_xn = (char *) malloc(MAX_LINE_SIZE * sizeof(char));

        // 4 is max size, x30\0, for example
        char fixed_xn[4] = {0, 0, 0, 0};
        char fixed_xm[4] = {0, 0, 0, 0};

        char* xm = ll_get(tokens, 3)->value;
        substr(xm, fixed_xm, 0, strlen(xm) - 2);
        substr(xn, fixed_xn, 1, strlen(xn));

        printf("(debug)\t\tREGISTERS: fixed_xn = %s, fixed_xm = %s, IR_SDT_REG = %d\n",
               fixed_xn, fixed_xm, IR_SDT_REG);

        addr_reg.xn = get_register_no(st, fixed_xn);
        addr_reg.xm = get_register_no(st, fixed_xm);

        addr.repr.reg = addr_reg;
        addr.type = IR_SDT_REG;
    } else if (is_pre_ind || is_post_ind) {
        printf("(debug)\tPARSE_INST_SDT: pre/post index addressing mode\n");
        assert(ll_length(tokens) <= 4);

        addr.type = IR_SDT_PPI;
        ir_sdt_addr_ppi addr_ppi;

        char* simm = NULL;
        if (ll_length(tokens) == 4) {
            simm = ll_get(tokens, 3)->value;
        } else {
            simm = "0";
        }

        // char* fixed_xn = (char *) malloc(MAX_LINE_SIZE * sizeof(char));
        // char* fixed_simm = (char *) malloc(MAX_LINE_SIZE * sizeof(char));
        char fixed_xn[4] = {0, 0, 0, 0};
        char fixed_simm[MAX_LINE_SIZE];
        memset(fixed_simm, 0, MAX_LINE_SIZE * sizeof(char));

        if (is_pre_ind) {
            substr(xn, fixed_xn, 1, strlen(xn));
            substr(simm, fixed_simm, 0, strlen(simm) - 3);
        } else {
            substr(xn, fixed_xn, 1, strlen(xn) - 2);
            // no need to fix simm
            substr(simm, fixed_simm, 0, strlen(simm));
        }

        // addr_ppi.xn = *((address_u *) hm_get(st, fixed_xn));
        addr_ppi.xn = get_register_no(st, fixed_xn);
        addr_ppi.simm = parse_value(fixed_simm, true);
        addr_ppi.pre_indexed = is_pre_ind;

        printf("(debug)\t\tPPI: fixed_xn = %s, fixed_simm = %s\n", fixed_xn, fixed_simm);

        // wirte back, suspicious may be deleted later
        /* hm_put(st, fixed_xn, addr_ppi.xn + addr_ppi.simm); */

        addr.repr.ppi = addr_ppi;

    } else {
        ir_sdt_addr_uio addr_uio;

        // last case is unsigned immediate offset
        // char *fixed_xn = NULL;
        // char *fixed_imm = NULL;
        // fixed_xn = (char *) malloc(MAX_LINE_SIZE * sizeof(char));
        // fixed_imm  = (char *) malloc(MAX_LINE_SIZE * sizeof(char));

        char fixed_xn[4] = {0, 0, 0, 0};
        char fixed_imm[MAX_LINE_SIZE];
        memset(fixed_imm, 0, MAX_LINE_SIZE * sizeof(char));


        if (ll_length(tokens) == 4) {
            substr(xn, fixed_xn, 1, strlen(xn));
            char* imm = ll_get(tokens, 3)->value;
            substr(imm, fixed_imm, 0, strlen(imm) - 2);
        } else {
            substr(xn, fixed_xn, 1, strlen(xn) - 2);
            fixed_imm[0] = '0';
            fixed_imm[1] = '\0';
        }
        

        printf("(debug)\t\tUIO: fixed_xn = %s, fixed_imm = %s\n", fixed_xn, fixed_imm);

        // addr_uio.xn = *((register_no_u *) hm_get(st, fixed_xn));
        addr_uio.xn = get_register_no(st, fixed_xn);
        addr_uio.imm = parse_value(fixed_imm, true) / (inst.is_32 ? 4 : 8);

        addr.repr.uio = addr_uio;
        addr.type = IR_SDT_UIO;
    }

    inst_sdt.addr = addr;
    inst.type = IR_INST_SDT;
    inst.repr.sdt = inst_sdt;
    return inst;
}

static ir_dp_operand parse_operand_tail(hm* st, ll *tail) {
    ir_dp_operand operand;
    char* reg_imm= ((char *) ll_get(tail, 0)->value);
    printf("(debug)\t\t\tPARSE_OPERAND: reg_imm = %s\n", reg_imm);

    if (isalpha(reg_imm[0])){ // register
        // operand.value.rm = *((int*) hm_get(st,reg_imm));//register
        operand.value.rm = get_register_no(st, reg_imm);
        operand.is_register = true;
    } else { // immediate
        operand.value.imm = parse_value(reg_imm, true);
        operand.is_register = false;

        printf("(debug)\t\t\tPARSE_OPERAND: immediate, imm = %d\n", parse_value(reg_imm, true));
    }

    if (ll_length(tail) == 3) { // the operand is to be shifted
        printf("(dbeug)\t\t\tPARSE_OPERAND: has shift\n");
        char* shift_type = ((char *) ll_get(tail, 1)->value);
        char* shift_amount = ((char *) ll_get(tail, 2)->value);
        // IR_SHIFT_LSL, IR_SHIFT_LSR, IR_SHIFT_ASR, IR_SHIFT_ROR
        if(strcmp(shift_type,"lsl") == 0) operand.shift.type = IR_SHIFT_LSL;
        if(strcmp(shift_type,"lsr") == 0) operand.shift.type = IR_SHIFT_LSR;
        if(strcmp(shift_type,"asr") == 0) operand.shift.type = IR_SHIFT_ASR;
        if(strcmp(shift_type,"ror") == 0) operand.shift.type = IR_SHIFT_ROR;
        operand.shift.amount = parse_value(shift_amount, true);//get the shift value
        printf("(debug)\t\t\tPARSE_OPERAND: shift_type=%s, shift_amount=%d\n",
            shift_type, operand.shift.amount);
    } else {
        // there is no shift
        operand.shift.type = 0;
        operand.shift.amount = 0;
    }

    return operand;
}

// static ir_dp_operand parse_operand(hm* st, ll* tokens) {//pase in value
//     ir_dp_operand operand;
//     char* reg_imm= ((char *) ll_get(tokens, 2)->value);
//     printf("(debug)\t\t\tPARSE_OPERAND: reg_imm = %s\n", reg_imm);

//     if(isalpha(reg_imm[0]) == 1){//register
//         operand.value.rm = *((int*) hm_get(st,reg_imm));//register
//         operand.is_register = true;
//     } else {
//         operand.value.rm = parse_value(reg_imm, true);
//         operand.is_register = false;
//     }
//     if(ll_length(tokens) == 5) {//there is shift in the
//         char* shift_type = ((char *) ll_get(tokens, 3)->value);
//         char* shift_amount = ((char *) ll_get(tokens, 4)->value);
//         // IR_SHIFT_LSL, IR_SHIFT_LSR, IR_SHIFT_ASR, IR_SHIFT_ROR
//         if(strcmp(shift_type,"lsl") == 0) operand.shift.type = IR_SHIFT_LSL;
//         if(strcmp(shift_type,"lsr") == 0) operand.shift.type = IR_SHIFT_LSR;
//         if(strcmp(shift_type,"asr") == 0) operand.shift.type = IR_SHIFT_ASR;
//         if(strcmp(shift_type,"ror") == 0) operand.shift.type = IR_SHIFT_ROR;
//         operand.shift.amount = parse_value(shift_amount, true);//get the shift value
//     }
//     return operand;
// }

static ir_instruction parse_inst_dp(char *line, hm *st) {
    ir_instruction inst;
    ir_dp inst_dp;
    // ll *tokens = (ll *) malloc(sizeof(ll));
    ll *tokens;

    tokens = tokenize(line);
    printf("(debug)\t\tTOKENIZATION COMPLETED\n");

    inst.is_32 = is_32reg((char*)(ll_get(tokens, 1)->value));
    //the first token must be an register
    char *opc = ll_get(tokens, 0)->value;
    printf("(debug)\t\tPARSE_INST_DP: opc = %s\n", opc);
    int type = select_inst_dp_type(opc);

    printf("(debug)\t\tPARSE_INST_DP: type = %d\n", type);

    if (type == IR_OPC_DP_MUL) {
//        assert(ll_length(tokens) == 5);


        ir_dp_mult inst_dp_mult;
        inst_dp.type = IR_DP_MULT;

        register_no_u rd, rn, rm, ra;

        // also intentional, see rt = hm_get
        // rd = (register_no_u) *((address_u *) hm_get(st, ll_get(tokens, 1)->value));
        // rn = (register_no_u) *((address_u *) hm_get(st, ll_get(tokens, 2)->value));
        // rm = (register_no_u) *((address_u *) hm_get(st, ll_get(tokens, 3)->value));

        rd = get_register_no(st, ll_get(tokens, 1)->value);
        rn = get_register_no(st, ll_get(tokens, 2)->value);
        rm = get_register_no(st, ll_get(tokens, 3)->value);

        // deal with the mul and mneg aliases, which sets ra to the zero register.
        if (strcmp(opc, "mul") == 0 || strcmp(opc, "mneg") == 0) {
            // ra = (register_no_u) *((address_u *) hm_get(st, "xzr"));
            ra = get_register_no(st, "xzr");
        } else {
            // ra = (register_no_u) *((address_u *) hm_get(st, ll_get(tokens, 4)->value));
            ra = get_register_no(st,ll_get(tokens, 4)->value);
        }

        inst_dp_mult.ra = ra;
        inst_dp_mult.rn = rn;
        inst_dp_mult.rd = rd;
        inst_dp_mult.rm = rm;

        if (strcmp(opc, "madd") == 0) inst_dp_mult.opcode = IR_DP_OPC_MADD;
        if (strcmp(opc, "msub") == 0) inst_dp_mult.opcode = IR_DP_OPC_MSUB;
        if (strcmp(opc, "mul")  == 0) inst_dp_mult.opcode = IR_DP_OPC_MADD;
        if (strcmp(opc, "mneg") == 0) inst_dp_mult.opcode = IR_DP_OPC_MSUB;

        inst_dp.repr.mult = inst_dp_mult;
    }

    if (type == IR_OPC_DP_OP1) {
//        assert(ll_length(tokens) == 3);


        ir_dp_op1 inst_dp_op1;
        inst_dp.type = IR_DP_OP1;

        // register_no_u rd = parse_value(*((int *) hm_get(st, ll_get(tokens, 1)->value)));
        // register_no_u rd = (register_no_u) *((address_u *) hm_get(st, ll_get(tokens, 1)->value));
        register_no_u rd = get_register_no(st, ll_get(tokens, 1)->value);

        inst_dp_op1.rd = rd;


        // if (strcmp(opc, "mov") == 0) inst_dp_op1.opcode = IR_DP_OPC_ORR;
        // if (strcmp(opc, "mul") == 0) inst_dp_op1.opcode = IR_DP_OPC_MADD;
        // if (strcmp(opc, "mneg") == 0) inst_dp_op1.opcode = IR_DP_OPC_MSUB;
        if (strcmp(opc, "neg") == 0) inst_dp_op1.opcode = IR_DP_OPC_SUB;
        if (strcmp(opc, "negs") == 0) inst_dp_op1.opcode = IR_DP_OPC_SUBS;
        if (strcmp(opc, "mvn") == 0) inst_dp_op1.opcode = IR_DP_OPC_ORN;
        if (strcmp(opc, "movz") == 0) inst_dp_op1.opcode = IR_DP_OPC_MOVZ;
        if (strcmp(opc, "movn") == 0) inst_dp_op1.opcode = IR_DP_OPC_MOVN;
        if (strcmp(opc, "movk") == 0) inst_dp_op1.opcode = IR_DP_OPC_MOVK;
        // ir_dp_operand operand = parse_operand(st, tokens);
        ir_dp_operand operand = parse_operand_tail(st, ll_tail(tokens, 2));
        inst_dp_op1.operand = operand;
        inst_dp.repr.op1 = inst_dp_op1;



    }

    if (type == IR_OPC_DP_OP2_D) {
//        assert(ll_length(tokens) == 4);
        printf("(debug)\t\tPARSE_INST_DP (IR_OPC_DP_OP2_D):\n");
        printf("(debug)\t\t\tll_get(tokens, 1)->value = %s\n", ((char *) ll_get(tokens, 1)->value));
        printf("(debug)\t\t\tll_get(tokens, 2)->value = %s\n", ((char *) ll_get(tokens, 2)->value));

        ir_dp_op2 inst_dp_op2;
        inst_dp.type = IR_DP_OP2;

        register_no_u rd, rn;
        // rd = (register_no_u) *((address_u *) hm_get(st, ll_get(tokens, 1)->value));
        rd = get_register_no(st, ll_get(tokens, 1)->value);
        

        if (ll_length(tokens) >= 4) {
            // rn = (register_no_u) *((address_u *) hm_get(st, ll_get(tokens, 2)->value));
            rn = get_register_no(st, ll_get(tokens, 2)->value);
        } else {
            // rn = (register_no_u) *((address_u *) hm_get(st, "xzr"));
            rn = get_register_no(st, "xzr");
        }

        printf("(debug)\t\t\trd: %d, rn: %d\n", rd, rn);

        inst_dp_op2.rd = rd;
        inst_dp_op2.rn = rn;

        if (strcmp(opc, "mov") == 0) inst_dp_op2.opcode = IR_DP_OPC_ORR;
        if (strcmp(opc, "add") == 0) inst_dp_op2.opcode = IR_DP_OPC_ADD;
        if (strcmp(opc, "adds") == 0) inst_dp_op2.opcode = IR_DP_OPC_ADDS;
        if (strcmp(opc, "sub") == 0) inst_dp_op2.opcode = IR_DP_OPC_SUB;
        if (strcmp(opc, "subs") == 0) inst_dp_op2.opcode = IR_DP_OPC_SUBS;
        if (strcmp(opc, "and") == 0) inst_dp_op2.opcode = IR_DP_OPC_AND;
        if (strcmp(opc, "ands") == 0) inst_dp_op2.opcode = IR_DP_OPC_ANDS;
        if (strcmp(opc, "bic") == 0) inst_dp_op2.opcode = IR_DP_OPC_BIC;
        if (strcmp(opc, "bics") == 0) inst_dp_op2.opcode = IR_DP_OPC_BICS;
        if (strcmp(opc, "eor") == 0) inst_dp_op2.opcode = IR_DP_OPC_EOR;
        if (strcmp(opc, "orr") == 0) inst_dp_op2.opcode = IR_DP_OPC_ORR;
        if (strcmp(opc, "eon") == 0) inst_dp_op2.opcode = IR_DP_OPC_EON;
        if (strcmp(opc, "orn") == 0) inst_dp_op2.opcode = IR_DP_OPC_ORN;

        
        ir_dp_operand operand; 
        // = { .is_register = false, .shift = dummy_shift, 
            // .value = {.imm = 0} };

        if (ll_length(tokens) >= 4) {
            operand = parse_operand_tail(st, ll_tail(tokens, 3));
            printf("(debug)\t\t\t"); print_operand(operand); printf("\n");
        } else {
            ir_dp_operand_shift dummy_shift = { .amount = 0, .type = IR_SHIFT_LSL };
            operand.is_register = true;
            operand.shift = dummy_shift;
            // register_no_u rm = (register_no_u) *((address_u *) hm_get(st, ll_get(tokens, 2)->value));
            register_no_u rm = get_register_no(st, ll_get(tokens, 2)->value);
            operand.value.rm = rm;
        }

        inst_dp_op2.operand = operand;
        inst_dp.repr.op2 = inst_dp_op2;
    }

    if (type == IR_OPC_DP_OP2_ND) {
//        assert(ll_length(tokens) == 3);

        ir_dp_op2 inst_dp_op2;
        inst_dp.type = IR_DP_OP2;

        register_no_u rn;
        // rn = (register_no_u) *((address_u *) hm_get(st, ll_get(tokens, 1)->value));
        rn = get_register_no(st, ll_get(tokens, 1)->value);

        register_no_u rd;
        // rd = (register_no_u) *((address_u *) hm_get(st, "xzr"));
        rd = get_register_no(st, "xzr");

        inst_dp_op2.rn = rn;
        inst_dp_op2.rd = rd;

        if (strcmp(opc, "cmp") == 0) inst_dp_op2.opcode = IR_DP_OPC_SUBS;
        if (strcmp(opc, "cmn") == 0) inst_dp_op2.opcode = IR_DP_OPC_ADDS;
        if (strcmp(opc, "tst") == 0) inst_dp_op2.opcode = IR_DP_OPC_ANDS;

        // ir_dp_operand operand = parse_operand(st ,tokens);
        ir_dp_operand operand = parse_operand_tail(st, ll_tail(tokens, 2));
        inst_dp_op2.operand = operand;
        inst_dp.repr.op2 = inst_dp_op2;
    }

    inst.type = IR_INST_DP;
    inst.repr.dp = inst_dp;
    return inst;

}

// input a value(hex or dec) returns the decimal rep
static int parse_value(char *val, bool skipfirst) {
    if (val[1 + skipfirst] == 'x') {
        // the address is in hex form, skipping the intial '#'
        // printf("val: %s, conv: %d \n", val, strtol(val + skipfirst, NULL, HEX));
        return strtol(val + skipfirst, NULL, HEX);
    } else {
        // the address is in decimal form, skipping the initial '#'
        return atoi(val + skipfirst);
    }
}

// input a literal and returns an address
static address_u parse_literal(char *literal, hm *st, int line_no) {
    address_u parsed_addr;
    if (isalpha(literal[0])) {
        // this means it is a label
        parsed_addr = *((address_u *)hm_get(st, literal)) - line_no * PC_INCREMENT;
    } else {
        // this means that it is an address
        parsed_addr = parse_value(literal, false) - line_no * PC_INCREMENT;
    }
    return parsed_addr;
}


// select which dp type we are parsing
static int select_inst_dp_type(char *opc) {
    const char *mul[4] = {"madd", "msub", "mul", "mneg"};
    const char *op2_d[14] = {"add", "adds", "sub", "subs", "and", "ands", "bic", "bics",
                             "eor", "orr", "ern", "orn", "eon", "mov"};
    const char *op1[6] = {"neg", "negs", "mvn", "movz", "movn", "movk"};
    const char *op2_nd[3] = {"cmp", "cmn", "tst"};

    if (elem(((char **) mul), opc, 4)) return IR_OPC_DP_MUL;
    if (elem(((char **) op2_d), opc, 14)) return IR_OPC_DP_OP2_D;
    if (elem(((char **) op1), opc, 6)) return IR_OPC_DP_OP1;
    if (elem(((char **) op2_nd), opc, 3)) return IR_OPC_DP_OP2_ND;

    return 0;
}

static bool elem(char **arr, char *s, int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(arr[i], s) == 0) return true;
    }
    return false;
}