#include "dpr.h"

static void de_dpr_ab(machine *machine, int sf, int opc, int m, int opr, int rm,
                      int operand, int rn, int rd); // arithmetic + bit-logic
static void de_dpr_mt(machine *machine, int sf, int opc, int m, int opr, int rm,
                      int operand, int rn, int rd); // multiply
static void set_state_N_Z(machine *machine, long long result, int sf);

const long long max_32 = 0xffffffff;
const long long min_32 = (-1) * max_32;
const long long max_64 = 0xffffffffffffffff;
const long long min_64 = (-1) * (max_64);

static void set_state_N_Z(machine *m1, long long result, int sf) {
    bool sign_bit;
    if (sf) {
        sign_bit = (result >> 63) & 0b1; // 64 bit case.
    }
    else {
        sign_bit = (result >> 31) & 0b1;
    }

    set_pstateN(m1, sign_bit);
    set_pstateZ(m1, result == 0);
}

// decode and execute data processing instructions - register.
extern void de_dpr(machine *ms, instruction ins) {

    int sf, opc, opr, rm, operand, rn, rd, m;
    int n1, n2; // useless digit
    int spec[] = {5, 5, 6, 5, 4, 1, 2, 1, 2, 1};
    int *dest[] = {&rd, &rn, &operand, &rm, &opr, &n2, &n1, &m, &opc, &sf};
    decode_segments(ins, dest, spec, 10);
    if (m == 1) { // multiply
        de_dpr_mt(ms, sf, opc, m, opr, rm, operand, rn, rd);
    } else {
        // bit +arithmetic
        de_dpr_ab(ms, sf, opc, m, opr, rm, operand, rn, rd);
    }
}

static int ars_int(int x, int n) {
    if (x < 0 && n > 0) {
        return x >> n | ~(~0U >> n);
    } else {
        return x >> n;
    }
}

static long ars_long(long long x, long long n) {
    if (x < 0 && n > 0) {
        long long shiftmask = ~(~((unsigned long long)0u) >> n);
        return x >> n | shiftmask;
    } else {
        return x >> n;
    }
}

static unsigned long rotr_long(unsigned long value, int shift) {
    if ((shift &= 63) == 0) {
        return value;
    }
    return (value >> shift) | (value << (64 - shift));
}

static unsigned int rotr_int(unsigned int value, int shift) {
    if ((shift &= 31) == 0) {
        return value;
    }
    return (value >> shift) | (value << (32 - shift));
}

// static int lrs_int(int x, int n) {
//     int mask = ~(-1 << n) << (32 - n);
//     return ~mask & ((x >> n) | mask);
// }

// static long long lrs_long(long long x, long long n) {
//     int mask = ~(-1 << n) << (64 - n);
//     return ~mask & ((x >> n) | mask);
// }

static void update_condition_flags(machine *machine, long long op1, long long op2, int sf, bool add) {
    long long res;
    int op1_sign, op2_sign, res_sign;
    bool s_of, us_of;

    unsigned long us_op1 = (unsigned long)op1;
    unsigned long us_op2 = (unsigned long)op2;

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
        s_of = (op1 > 0 && op2 < 0 && res < 0) || (op1 < 0 && op2 > 0 && res > 0);

        set_state_N_Z(machine, res, sf);
        set_pstateC(machine, us_of);
        set_pstateV(machine, s_of);
    }
}

static void de_dpr_ab(machine *m, int sf, int opc, int m1, int opr, int rm,
                      int operand, int rn, int rd) {
    long long vn, vm;
    long long result;
    bool is_32 = false;
    if (sf) {
        // 64 bit
        vm = get_register64(m, rm);
        vn = get_register64(m, rn);
    } else {
        // 32bit
        vm = get_register32(m, rm);
        vn = get_register32(m, rn);
        is_32 = true;
    }

    int n = 1 & opr;
    int shift = ((((1 << 2) & opr)) + ((1 << 1) & opr)) >> 1;

    switch (shift) {
    case LLS_IDENTIFIER:
        vm = is_32 ? ((unsigned int)vm) << ((int)operand) : ((unsigned long)vm) << ((long)operand);
        break;
    case LRS_IDENTIFIER:
        vm = is_32 ? ((unsigned int)vm) >> ((int)operand) : ((unsigned long)vm) >> ((int)operand);
        break;
    case ARS_IDENTIFIER:
        vm = is_32 ? ars_int(vm, operand) : ars_long(vm, operand);
        break;
    case ROR_IDENTIFIER:
        vm = is_32 ? rotr_int((int)vm, operand) : rotr_long(vm, operand);
        break;
    default:
        printf("instruction overflow\n");
        break;
    }

    if (opr & (1 << 3)) {
        // arithmetic
        switch (opc) {
        case ADD_IDENTIFIER:
            result = vn + vm;
            break; // add
        case ADDS_IDENTIFIER:
            result = vn + vm;
            update_condition_flags(m, vn, vm, sf, true);
            break; // adds
        case SUB_IDENTIFIER:
            result = vn - vm;
            break; // sub
        case SUBS_IDENTIFIER:
            result = vn - vm;
            update_condition_flags(m, vn, vm, sf, false);
            break; // subs
        default:
            printf("instruction overflow\n");
        }
    } else {
        vm = n == 1 ? (~vm) : vm;
        switch (opc) {
        case AND_IDENTIFIER:
            result = vn & vm;
            break;
        case OR_IDENTIFIER:
            result = vn | vm;
            break;
        case XOR_IDENTIFIER:
            result = vn ^ (vm);
            break;
        case ANDS_IDENTIFIER:
            result = vn & vm;
            set_state_N_Z(m, result, sf);
            set_pstateC(m, false);
            set_pstateV(m, false);
            break;
        default:
            printf("instruction over flow\n");
        }
    }
    if (sf) { // 64 bit
        // printf("result:%lld\n", result);
        set_register64(m, rd, result);
    } else {
        set_register32(m, rd, (int)result);
    }
} // arithmetic + bit-logic

// de_dpr_mt(ms, sf, opc, m, opr, rm, operand, rn, rd);
static void de_dpr_mt(machine *m, int sf, int opc, int m1, int opr, int rm,
                      int operand, int rn, int rd) {
    long long result;
    long long va, vn, vm;

    register_no ra = operand & 0b11111;

    if (sf) {
        // 64
        va = get_register64(m, ra);
        vn = get_register64(m, rn);
        vm = get_register64(m, rm);
    } else {
        va = get_register32(m, ra);
        vn = get_register32(m, rn);
        vm = get_register32(m, rm);
    }
    int x = operand >> 5;
    result = x == 0 ? va + vn * vm : va - vn * vm;
    if (sf) {
        set_register64(m, rd, result);
    } else {
        set_register32(m, rd, (int)result);
    }
} // multiply
