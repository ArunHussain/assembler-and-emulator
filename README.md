An assembler and emulator built for a group coursework.

The assembler was designed to translate an ARM assembly source file into a binary file.
The emulator simulates the execution of a binary file on a Raspberry Pi.

The files I worked on and coded are:
* src\assembler\ll.c - a linked list implementation used in other files to implement a hashmap and tokenize assembly source code
* src\assembler\ll.h - header file for the above
* src\assembler\hm.c - a hashmap implementation using ll.c which is used elsewhere to build a symbol table for the assembly source code
* src\assembler\hm.h - header file for the above
* src\assembler\readfile.c - contains the code for reading in the assembly source code file
* src\assembler\readfile.h - header file for the above
* src\emulator\brc.c - code for executing binary branch instructions
* src\emulator\brc.h - header file for the above
* src\emulator\sdt.c - code for executing binary single data transfer instructions
* src\emulator\sdt.h - header file for the above

