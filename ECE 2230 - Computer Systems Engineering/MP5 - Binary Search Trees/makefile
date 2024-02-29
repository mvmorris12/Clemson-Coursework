# makefile for MP5
# Version: 1
#
# -lm is used to link in the math library
#
# -Wall turns on all warning messages 
#
comp = gcc
comp_flags = -g -Wall 
comp_libs = -lm  

lab5 : tree.o lab5.o
	$(comp) $(comp_flags) tree.o lab5.o -o lab5 $(comp_libs)

tree.o : tree.c tree.h
	$(comp) $(comp_flags) -c tree.c

lab5.o : lab5.c tree.h
	$(comp) $(comp_flags) -c lab5.c

clean :
	rm -f *.o lab5 tree core

