#include "arch.h"
#include <stdio.h>

extern void show_bits(FILE *fp, register64 value);

extern longint sign_ext(longint value, int bitcount);

// The finalised, pretty unreadable version.
// usage:
//  int rd, operand, opi, op0, opc, sf;
//  int *dest[6] = {&rd, &operand, &opi, &op0, &opc, &sf};
//  int spec[6]  = {5,   18,       3,    3,    2,    1};
//  decode_segs(undec_inst, dest, spec, 6);

// Splits a binary instruction into `n_segments` segments whose lengths are given by
// the list of integers `int *spec`, and stores the results in addresses given by
// the list of pointers `int **dest`.
extern void decode_segments(instruction undec_inst, int **dest, int *spec, int n_segments);

extern longint shift(int shift, longint value);

extern longint join_bits(byte *src, int nbytes);
