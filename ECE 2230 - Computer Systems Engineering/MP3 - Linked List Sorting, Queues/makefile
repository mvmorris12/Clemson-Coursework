# The makefile for MP3.
# Type:
#   make         -- to build program lab3
#   make design  -- check for simple design errors (incomplete)
#   make driver -- to compile testing program
#   make clean   -- to delete object files, executable, and core
#   make twl_list.o  -- to compile only twl_list.o (or: use lab3.o, ap_support.o)
#
# You should not need to change this file.  
#
# Format for each entry
#    target : dependency list of targets or files
#    <tab> command 1
#    <tab> command 2
#    ...
#    <tab> last command
#    <blank line>   -- the list of commands must end with a blank line

lab3 : twl_list.o ap_support.o lab3.o
	gcc -Wall -g twl_list.o ap_support.o lab3.o -o lab3

twl_list.o : twl_list.c datatypes.h twl_list.h
	gcc -Wall -g -c twl_list.c

ap_support.o : ap_support.c datatypes.h twl_list.h ap_support.h
	gcc -Wall -g -c ap_support.c

lab3.o : lab3.c datatypes.h twl_list.h ap_support.h
	gcc -Wall -g -c lab3.c

driver : driver.o twl_list.o ap_support.o
	gcc -Wall -g twl_list.o ap_support.o driver.o -o driver

driver.o : driver.c datatypes.h twl_list.h ap_support.h
	gcc -Wall -g -c driver.c

#  @ prefix suppresses output of shell command
#  - prefix ignore errors
#  @command || true to avoid Make's error
#  : is shorthand for true
design :
	@grep -e "-> *ll_front" lab3.c ap_support.c ||:
	@grep -e "-> *ll_back" lab3.c ap_support.c ||:
	@grep -e "-> *ll_count" lab3.c ap_support.c ||:
	@grep -e "-> *ll_is_sorted" lab3.c ap_support.c ||:
	@grep -e "-> *next_node" lab3.c ap_support.c ||:
	@grep -e "-> *prev_node" lab3.c ap_support.c ||:
	@grep -e "-> *data_ptr" lab3.c ap_support.c ||:
	@grep "ll_node_t" lab3.c ap_support.c ||:
	@grep "mobile_count" twl_list.c ||:
	@grep "eth_address" twl_list.c ||:
	@grep "ip_address" twl_list.c ||:
	@grep "time_received" twl_list.c ||:
	@grep "ap_" twl_list.c ||:

clean :
	rm -f *.o lab3 driver core a.out

