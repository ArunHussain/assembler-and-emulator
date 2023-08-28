CC      = gcc
CFLAGS  = -std=c17 -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -Wall -Iemulator -Iassembler
LDLIBS  = -Lemulator -lemulator -Lassembler -lassembler
BUILD   = emulator assembler emulate assemble

.PHONY: all clean emulator assembler

all: $(BUILD)

clean:
	$(RM) -f emulate assemble *.o core;
	cd emulator; make clean
	cd assembler; make clean

emulator:
	cd emulator;  make

assembler:
	cd assembler; make

emulate: emulate.o
assemble: assemble.o
