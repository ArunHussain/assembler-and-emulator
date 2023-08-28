#include "arch.h"

#include "machine.h"
#include "obj.h"
#include "fetch.h"
#include "decode_execute.h"
#include "assert.h"

void write_output_file(FILE *fp, machine *machine)
{
    // FILE *fp = fopen(path, "w");
    fprintf(fp, "Registers: \n");
    for (register_no i = 0; i < N_REGISTERS; i++)
    {
        register64 value = get_register64(machine, i);
        if (i < 10) {
            fprintf(fp, "X0%d    = %016llx\n", i, value);
        }
        else {
            fprintf(fp, "X%d    = %016llx\n", i, value);
        }
    }
    fprintf(fp, "PC      = %016llx\n", get_register64(machine, PC_INDEX));
    fprintf(fp, "PSTATE: ");
    if (machine->pstateN) {
        fprintf(fp, "N");
    } else {
        fprintf(fp, "-");
    }

    if (machine->pstateZ) {
        fprintf(fp, "Z");
    } else {
        fprintf(fp, "-");
    }

    if (machine->pstateC) {
        fprintf(fp, "C");
    } else {
        fprintf(fp, "-");
    }

    if (machine->pstateV) {
        fprintf(fp, "V");
    } else {
        fprintf(fp, "-");
    }
    fprintf(fp, "\n");

    fprintf(fp, "Non-zero memory:\n");
    byte dest[4];
    for (address addr = 0; addr < MEMORY_SIZE; addr += 4) {
        get_bytes(machine, addr, dest, 4);
        uint32_t value = join_bits(dest, 4);
        if (value != 0) {
            fprintf(fp, "0x%08x: %08x\n", addr, value);
        } 
    }
    // fclose(fp);
       
}

int main(int argc, const char *args[]) {
    machine global_machine;
    machine *m = &global_machine;

    init_machine(m);
    load_object_file(m, args[1]);

    printf("Init finished.\n");

    int errcode = 0;
    bool halt = false;

    for (int cycle = 1; !halt; cycle++) {
        instruction undec_inst = fetch(m);
        // decode execute will also increment the program counter,
        // since some branch instructions set the PC to a specified value
        halt = decode_execute(m, undec_inst, &errcode);
        if (halt) {
            break;
        }
        printf("Cycle %d\n", cycle);
    }

    if (argc > 2) {
        FILE *fp = fopen(args[2], "w");
        write_output_file(fp, m);
        fclose(fp);
    } else {
        write_output_file(stdout, m);
    }

    return EXIT_SUCCESS;
}
