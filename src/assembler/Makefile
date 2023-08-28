CC      = gcc
CFLAGS  ?= -g -Wall
TESTS   = test_adts test_mcw test_readfile test_stb test_tokenizer test_hm test_ll
LIB     = libassembler.a
LIBOBJS = ll.o hm.o machine_code_writer.o parser.o readfile.o symbol_table_builder.o tokenizer.o
BUILD   = $(TESTS) $(LIB)

all: $(BUILD)

clean:
	$(RM) -f $(BUILD) *.o core

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

ll.o: ll.h
hm.o: hm.h ll.o ll.h

machine_code_writer.o:  arch.h ir.h machine_code_writer.h
parser.o:               arch.h ir.h ll.o ll.h hm.o hm.h
readfile.o:             ll.o ll.h
symbol_table_builder.o: ll.o ll.h hm.o hm.h
tokenizer.o:            ll.o ll.h

test_adts:      ll.o hm.o test_adts.o
test_mcw:       machine_code_writer.o test_mcw.o
test_readfile:  ll.o readfile.o test_readfile.o
test_stb:       ll.o hm.o symbol_table_builder.o test_stb.o
test_tokenizer: ll.o tokenizer.o test_tokenizer.o
test_hm: 		ll.o hm.o
test_ll: 		ll.o

