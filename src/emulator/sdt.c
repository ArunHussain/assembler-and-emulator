#include "sdt.h"
#include <stdio.h>

extern void de_sdt(machine *machine, instruction instruction) {
    int ignore;

    int is_sdt, sf, u, todec_address, rt;
    int *dest[6] = {&rt, &todec_address, &u, &ignore, &sf, &is_sdt};
    int spec[6]  = {5,   19,             1,  5,       1,   1};
    decode_segments(instruction, dest, spec, 6);

    address load_addr = 0;

    if (is_sdt) {
        // single data transfer instruction  
        int l, offset, xn;
        int *addr_dest[4] = {&xn, &offset,         &l, &ignore};
        int addr_spec[4]  = {5,   SDT_OFFSET_SIZE, 1,  1};
        decode_segments(todec_address, addr_dest, addr_spec, 4);


        bool register_offset = offset >> (SDT_OFFSET_SIZE - 1);

        if (u) {
            // unsigned offset
            unsigned int unsigned_offset = (unsigned int) offset;
            int multiplier = (sf ? 8 : 4);
            load_addr = get_register64(machine, xn) + unsigned_offset * multiplier;
            // show_bits(stdout, instruction);
        } else if (register_offset) {
            // register offset
            register_no xm = (offset >> SDT_XM_SHIFT) & SDT_XM_MASK;
            load_addr = get_register64(machine, xn) + get_register64(machine, xm);
        } else {
            // pre/post-indexed
            bool i = (offset >> SDT_I_SHIFT) & SDT_I_MASK;
            longint simm9 = offset >> SDT_SIMM9_SHIFT;
            int64_t signed_simm9 = sign_ext(simm9, 9);
            address writeback = get_register64(machine, xn) + signed_simm9;
            if (i) load_addr = writeback;                   // pre-indexed
            else   load_addr = get_register64(machine, xn); // post-indexed
            set_register64(machine, xn, writeback);
        }

        if (!l) {
            // a store operation
            if (sf) set_memory_uint(machine, load_addr, 8, get_register64(machine, rt)); // 64-bit register width
            else    set_memory_uint(machine, load_addr, 4, get_register32(machine, rt)); // 32-bit register width
            return;
        }
    } else {
        // load literal instruction
        address pc     = get_register64(machine, PC_INDEX);
        address simm19 = sign_ext(todec_address, 19);
        load_addr = pc + simm19 * 4;
    }

    // a load operation
    if (sf) {
        // 64-bit register width
        byte dest[8];
        get_bytes(machine, load_addr, dest, 8);
        register64 value = join_bits(dest, 8);
        set_register64(machine, rt, value);
    } else {
        // 32-bit register width
        byte dest[4];
        get_bytes(machine, load_addr, dest, 4);
        register32 value = join_bits(dest, 4);
        set_register32(machine, rt, value);
    }
}
