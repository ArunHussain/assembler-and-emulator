CC      = gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall

LIB     = libemulator.a
LIBOBJS = obj.o machine.o fetch.o dpi.o dpr.o sdt.o brc.o decode_execute.o utils.o
BUILD   = $(LIB)

all: $(BUILD)

clean:
	$(RM) -f $(BUILD) *.o core

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)


utils.o:          arch.h utils.h
machine.o:        arch.h utils.o machine.h
obj.o:            arch.h utils.o obj.h
fetch.o:          arch.h utils.o fetch.h
decode_execute.o: dpi.o dpr.o sdt.o brc.o decode_execute.h

dpi.o: arch.h utils.o machine.o dpi.h
dpr.o: arch.h utils.o machine.o dpr.h
sdt.o: arch.h utils.o machine.o sdt.h
brc.o: arch.h utils.o machine.o brc.h	
