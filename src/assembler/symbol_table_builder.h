#include "ll.h"
#include "hm.h"

// Accepts lines read from a .s file and produces a hashmap associating
// labels with memory addresses, as the first pass of two-pass assembly.
extern hm *build_symbol_table(ll *lines);
