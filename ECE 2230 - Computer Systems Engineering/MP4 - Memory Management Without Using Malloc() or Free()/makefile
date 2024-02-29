# makefile for MP4
# Version: 1
#
# -lm is used to link in the math library
#
# -Wall turns on all warning messages 
#
comp = gcc
comp_flags = -g -Wall 
comp_libs = -lm  

lab4 : twl_list.o mem.o lab4.o
	$(comp) $(comp_flags) twl_list.o mem.o lab4.o -o lab4 $(comp_libs)

twl_list.o : twl_list.c datatypes.h twl_list.h
	$(comp) $(comp_flags) -c twl_list.c

mem.o : mem.c mem.h
	$(comp) $(comp_flags) -c mem.c

lab4.o : lab4.c datatypes.h twl_list.h mem.h
	$(comp) $(comp_flags) -c lab4.c

clean :
	rm -f *.o lab4 core

