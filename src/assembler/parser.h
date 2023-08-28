#include "ir.h"
#include "ll.h"
#include "hm.h"
#include "tokenizer.h"
#include <stdlib.h>
#include <assert.h> 
#include <ctype.h> 

// Parses tokens for a single line, produced by a tokenizer, accepting the symbol table
// produced by symbol_table_builder.

// takes in the original str o
// and store the substring
// form start index s to end index e
// in result str r
// #define substr(o, r, s, e) memcpy(r, o + s, e - s + 1)
#define memlen(m)          (sizeof(m) / sizeof(m[0]))

#define MAX_LINE_SIZE     128
#define HEX               16

#define IR_OPC_BRC_LIT    "b"
#define IR_OPC_BRC_REG    "br"

#define IR_OPC_SDT_LDR    "ldr"
#define IR_OPC_SDT_STR    "str"
  
#define IR_OPC_DP_MUL     0
#define IR_OPC_DP_OP2_D   1
#define IR_OPC_DP_OP1     2
#define IR_OPC_DP_OP2_ND  3


extern ir_instruction parse(char *line, int line_no, hm *st);

