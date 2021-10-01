CC		= gcc
CFLAGS	= -Wall -Wextra -std=c11 -O2

all: similar_lines

word_type.o: word_type.c word_type.h structs_and_const.h

line_process.o: line_process.c line_process.h  word_type.h

sets_sort.o: sets_sort.c sets_sort.h structs_and_const.h

new_set_add.o: new_set_add.c new_set_add.h structs_and_const.h

allocations_and_frees.o: allocations_and_frees.c allocations_and_frees.h structs_and_const.h

output.o: output.c output.h

similar_lines.o:	similar_lines.c line_process.h sets_sort.h new_set_add.h allocations_and_frees.h output.h

similar_lines:	similar_lines.o word_type.o line_process.o sets_sort.o new_set_add.o allocations_and_frees.o output.o

clean:
	rm -f *.o similar_lines
