# makefile for MP6
#
# -lm is used to link in the math library
# -Wall turns on all warning messages 
#
comp = gcc
comp_flags = -g -Wall 
comp_libs = -lm  

lab6 : hashes.o table.o lab6.o
	$(comp) $(comp_flags)  hashes.o table.o lab6.o -o lab6 $(comp_libs)

table.o : table.c table.h hashes.h
	$(comp) $(comp_flags) -c table.c

hashes.o : hashes.c table.h hashes.h
	$(comp) $(comp_flags) -c hashes.c

lab6.o : lab6.c table.h
	$(comp) $(comp_flags) -c lab6.c

clean :
	rm -f *.o lab6 core

