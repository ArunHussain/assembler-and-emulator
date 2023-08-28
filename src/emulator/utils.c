#include "utils.h"

void _show_bits(char *buffer, register64 value) {
    if (value >> 1 > 0) {
        _show_bits(buffer, value >> 1);
    }
    strcat(buffer, (value & 1) ? "1" : "0");
}

void show_bits(FILE *fp, register64 value) {
    char bits[65];
    memset(bits, 0, 65);
    _show_bits(bits, value);

    int n_bits = strlen(bits);
    int padsize = 64 - n_bits;

    char output[65];
    for (int i = 0; i < padsize; i++) {
        output[i] = '0';
    }
    if (padsize > 0) {
        output[padsize] = '\0';
        strcat(output, bits);
    } else {
        strcpy(output, bits);
    }

    fprintf(fp, "%s\n", output);
}

extern longint sign_ext(longint value, int bit_count) {
    bool sign = value >> (bit_count - 1);
    if (sign == 1) {
        value |= (0xFFFFFFFFFFFFFFFF << bit_count);
    }
    return value;
}

// The finalised, pretty unreadable version.
// usage:
//  int rd, operand, opi, op0, opc, sf;
//  int *dest[6] = {&sf, &opc, &op0, &opi, &operand, &rd};
//  int spec[6]  = {1,   2,    3,    3,    18,       5};
//  decode_segs(undec_inst, dest, spec, 6);

// Splits a binary instruction into `n_segments` segments whose lengths are given by
// the list of integers `int *spec`, and stores the results in addresses given by
// the list of pointers `int **dest`.
extern void decode_segments(instruction undec_inst, int *dest[], int *spec, int n_segments) {
    for (int i = 0; i < n_segments; i++) {
        *(*dest++) = undec_inst & (1 << *spec) - 1;
        undec_inst >>= (*spec++);
    }
}

extern longint join_bits(byte *src, int nbytes) {
    longint result = 0;
    for (int i = 0; i < nbytes; i++) {
        result += ((longint) *src++) << ((longint) 8*i);
    }
    return result;
}
