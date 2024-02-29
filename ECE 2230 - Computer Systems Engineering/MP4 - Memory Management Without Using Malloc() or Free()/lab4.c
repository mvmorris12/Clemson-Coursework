 /* lab4.c
 * Lab4: Dynamic Memory Allocation
 * Version: 1
 * Michael Morris
 * Mvmorri
 * ECE 2230, Fall 2023
 *
 * This file contains drivers to test the mem.c package for dynamic memory
 * allocation.
 * 
 * The following parameters can be set on the command line.  If not set,
 * default values are used.  
 *
 * If different options are implemented for the memory package, this provides a
 * simple mechanism to change the options.  
 *
 * -f best|first  search policy to find memory block (first by default)
 * -c             turn on coalescing (off by default)
 *
 * General options for all test drivers
 * -s 10262023 random number generator seed
 * -v         verbose
 *
 * The Unit test drivers.  Two example drivers are provided.  You MUST
 *           add one or more additional unit drivers for more detailed testing
 * 
 * -u 0      Tests one allocation for 16 bytes
 * -u 1      Tests 4 allocations including a new page
 *           The student must update this driver to match the details of
 *           his or her design.
 *
 * -u 2      Tests allocations of just below, at, and just above full page. 
 *           Also allocates 8 and 9 pages to test large requests.
 * -u 3      Tests that coalescing works correctly
 * -u 4      Tests that first and best-fit methods work correctly
 *
 * The equilibrium test driver.  See comments with equilibriumDriver below for
 * additional options.
 * -e        run equilibrium test driver
 *
 * To test the equilibrium driver with the system malloc/free isntead
 * of your implementation of a heap use the option
 * -d        Use system malloc/free to verify equilibrium dirver and list ADT
 *           work as expected
 *
 * Revisions: Consider changing equilibrium driver to check out smaller than
 *            average block sizes during warmup to create clutter in free list
 *            without coalescing.  And, scale memory block sizes up the longer
 *            the driver runs so that early allocations create more small
 *            blocks.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include "twl_list.h"
#include "mem.h"

// structure for equilibrium driver parameters 
typedef struct {
    int SearchPolicy;
    int Coalescing;
    int Seed;
    int Verbose;
    int EquilibriumTest;
    int WarmUp;
    int Trials;
    int AvgNumInts;
    int RangeInts;
    int SysMalloc;
    int UnitDriver;
} driver_params;

// prototypes for functions in this file only 
void getCommandLine(int argc, char **argv, driver_params *ep);
void equilibriumDriver(driver_params *ep);

int main(int argc, char **argv)
{
    driver_params dprms;
    getCommandLine(argc, argv, &dprms);
    printf("Seed: %d\n", dprms.Seed);
    srand48(dprms.Seed);

    // The major choices: search policy and coalescing option 
    if (dprms.SearchPolicy == BEST_FIT) printf("Best-fit search policy");
    else if (dprms.SearchPolicy == FIRST_FIT) printf("First-fit search policy");
    else {
        fprintf(stderr, "Error with undefined search policy\n");
        exit(1);
    }
    if (dprms.Coalescing == TRUE) printf(" using coalescing\n");
    else if (dprms.Coalescing == FALSE) printf(" without coalescing\n");
    else {
        fprintf(stderr, "Error specify coalescing policy\n");
        exit(1);
    }
    Mem_configure(dprms.SearchPolicy, dprms.Coalescing);

    if (dprms.UnitDriver == 0)
    {
        // unit driver 0: basic test with one allocation and free
        printf("\n----- Begin unit driver 0 -----\n");
        char *string;
        const char msg[] = "hello world 15c";
        int len = strlen(msg);
        // add one for null character at end of string 
        string = (char *) Mem_alloc((len+1) * sizeof(char));
        strcpy(string, msg);
        printf("string length=%d\n:%s:\n", len, string);
        printf("\nFree list after first allocation\n");
        Mem_stats();
        Mem_print();
        Mem_free(string);
        printf("\nFree list after first free\n");
        printf("unit driver 0 has returned all memory to free list\n");
        Mem_print();
        Mem_stats();
        string = NULL;
        printf("\n----- End unit driver 0 -----\n");
    }
    else if (dprms.UnitDriver == 1)
    {
        printf("\n----- Begin unit driver 1 -----\n");

        int unit_size = sizeof(mem_chunk_t);
        int units_in_first_page = PAGESIZE/unit_size - 1;  // minus 1 due to dummy block
        assert((units_in_first_page+1) * unit_size == PAGESIZE);
        printf("There are %d units in first page, and one unit is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2, *p3, *p4;
        int num_bytes_1, num_bytes_2, num_bytes_3, num_bytes_4;

        // allocate 1st pointer to 1/8 of a page
        num_bytes_1 = ((units_in_first_page+1)/8 - 1)*unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();

        // allocate for 2nd pointer to 1/2 of a page
        num_bytes_2 = ((units_in_first_page+1)/2 - 1)*unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("second: %d bytes (%d units) at p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print();

        // allocate remaining memory in free list
        num_bytes_3 = units_in_first_page - num_bytes_1/unit_size 
            - num_bytes_2/unit_size - 3;
        num_bytes_3 *= unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("third: %d bytes (%d units) at p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print();
        printf("unit driver 1: above Mem_print shows empty free list\n");

        // allocate for 4th pointer to 1/4 a page when free list is empty
        num_bytes_4 = (units_in_first_page/4 - 1)*unit_size;
        p4 = (int *) Mem_alloc(num_bytes_4);
        printf("fourth: %d bytes (%d units) at p=%p \n", 
                num_bytes_4, num_bytes_4/unit_size, p4);
        Mem_print();

        // next put the memory back into the free list:

        printf("first free of 1/8 a page p=%p \n", p1);
        Mem_free(p1);
        Mem_print();

        printf("second free of 3/8 a page p=%p \n", p3);
        Mem_free(p3);
        Mem_print();

        printf("third free of 1/2 a page p=%p \n", p2);
        Mem_free(p2);
        Mem_print();
        
        printf("fourth free of 1/4 a page p=%p\n", p4);
        Mem_free(p4);
        printf("unit driver 1 has returned all memory to free list\n");
        Mem_print();
        Mem_stats();
        printf("\n----- End unit test driver 1 -----\n");
    } else if (dprms.UnitDriver == 2){
        //   -- request the number of bytes that matches a whole page, and a 
        //      size that is one unit smaller and larger than a page

        int *p1, *p2, *p3, *p4, *p5, *p6, *p7;
        int num_bytes_1, num_bytes_2, num_bytes_3, num_bytes_4, num_bytes_5, num_bytes_6, num_bytes_7;
        int num_units_1, num_units_2, num_units_3, num_units_4, num_units_5, num_units_6, num_units_7;

        num_bytes_1 = PAGESIZE-sizeof(mem_chunk_t);
        num_bytes_2 = PAGESIZE;
        num_bytes_3 = PAGESIZE+sizeof(mem_chunk_t);
        num_bytes_4 = PAGESIZE*2-(5*sizeof(int));
        num_bytes_5 = PAGESIZE*2+(5*sizeof(int));
        num_bytes_6 = PAGESIZE*8-sizeof(mem_chunk_t);
        num_bytes_7 = PAGESIZE*8+(5*sizeof(int));
        
        num_units_1 = (num_bytes_1/sizeof(mem_chunk_t))+((num_bytes_1%sizeof(mem_chunk_t))>0);
        num_units_2 = (num_bytes_2/sizeof(mem_chunk_t))+((num_bytes_2%sizeof(mem_chunk_t))>0);
        num_units_3 = (num_bytes_3/sizeof(mem_chunk_t))+((num_bytes_3%sizeof(mem_chunk_t))>0);
        num_units_4 = (num_bytes_4/sizeof(mem_chunk_t))+((num_bytes_4%sizeof(mem_chunk_t))>0);
        num_units_5 = (num_bytes_5/sizeof(mem_chunk_t))+((num_bytes_5%sizeof(mem_chunk_t))>0);
        num_units_6 = (num_bytes_6/sizeof(mem_chunk_t))+((num_bytes_6%sizeof(mem_chunk_t))>0);
        num_units_7 = (num_bytes_7/sizeof(mem_chunk_t))+((num_bytes_7%sizeof(mem_chunk_t))>0);

        printf("\nAlloc p1 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_1, num_units_1);
        p1 = (int*)Mem_alloc(num_bytes_1);
        Mem_print();
        printf("\nFreeing p1 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_1, num_units_1);
        Mem_free(p1);
        Mem_print();
        // Mem_stats();
        printf("\nAlloc p2 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_2, num_units_2);
        p2 = (int*)Mem_alloc(num_bytes_2);
        Mem_print();
        printf("\nFreeing p2 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_2, num_units_2);
        Mem_free(p2);
        Mem_print();
        // Mem_stats();
        printf("\nAlloc p3 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_3, num_units_3);
        p3 = (int*)Mem_alloc(num_bytes_3);
        Mem_print();
        printf("\nFreeing p3 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_3, num_units_3);
        Mem_free(p3);
        Mem_print();
        // Mem_stats();
    
        //   -- request more bytes than in one page
        printf("\nAlloc p4 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_4, num_units_4);
        p4 = (int*)Mem_alloc(num_bytes_4);
        Mem_print();
        printf("\nFreeing p4 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_4, num_units_4);
        Mem_free(p4);
        Mem_print();
        // Mem_stats();
        printf("\nAlloc p5 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_5, num_units_5);
        p5 = (int*)Mem_alloc(num_bytes_5);
        Mem_print();
        printf("\nFreeing p5 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_5, num_units_5);
        Mem_free(p5);
        Mem_print();
        // Mem_stats();
        printf("\nAlloc p6 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_6, num_units_6);
        p6 = (int*)Mem_alloc(num_bytes_6);
        Mem_print();
        printf("\nFreeing p6 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_6, num_units_6);
        Mem_free(p6);
        Mem_print();
        // Mem_stats();
        printf("\nAlloc p7 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_7, num_units_7);
        p7= (int*)Mem_alloc(num_bytes_7);
        Mem_print();
        printf("\nFreeing p7 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_7, num_units_7);
        Mem_free(p7);
        Mem_print();
        Mem_stats();
        printf("end unit test 2\n");
    } else if (dprms.UnitDriver == 3){
        //   -- combinations of requests and frees such that the free list is empty

        int *p1, *p2, *p3, *p4, *p5, *p6, *p7;
        int num_bytes_1, num_bytes_2, num_bytes_3, num_bytes_4, num_bytes_5, num_bytes_6, num_bytes_7;
        int num_units_1, num_units_2, num_units_3, num_units_4, num_units_5, num_units_6, num_units_7;

        num_bytes_1 = 14*sizeof(mem_chunk_t);
        num_bytes_2 = 29*sizeof(mem_chunk_t);
        num_bytes_3 = 34*sizeof(mem_chunk_t);
        num_bytes_4 = 49*sizeof(mem_chunk_t);
        num_bytes_5 = 54*sizeof(mem_chunk_t);
        num_bytes_6 = 69*sizeof(mem_chunk_t);
        num_bytes_7 = 255*sizeof(mem_chunk_t);
        
        num_units_1 = (num_bytes_1/sizeof(mem_chunk_t))+((num_bytes_1%sizeof(mem_chunk_t))>0);
        num_units_2 = (num_bytes_2/sizeof(mem_chunk_t))+((num_bytes_2%sizeof(mem_chunk_t))>0);
        num_units_3 = (num_bytes_3/sizeof(mem_chunk_t))+((num_bytes_3%sizeof(mem_chunk_t))>0);
        num_units_4 = (num_bytes_4/sizeof(mem_chunk_t))+((num_bytes_4%sizeof(mem_chunk_t))>0);
        num_units_5 = (num_bytes_5/sizeof(mem_chunk_t))+((num_bytes_5%sizeof(mem_chunk_t))>0);
        num_units_6 = (num_bytes_6/sizeof(mem_chunk_t))+((num_bytes_6%sizeof(mem_chunk_t))>0);
        num_units_7 = (num_bytes_7/sizeof(mem_chunk_t))+((num_bytes_7%sizeof(mem_chunk_t))>0);
        
        printf("\nAlloc p1 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_1, num_units_1);
        p1 = (int*)Mem_alloc(num_bytes_1);
        Mem_print();
        printf("\nAlloc p2 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_2, num_units_2);
        p2 = (int*)Mem_alloc(num_bytes_2);
        Mem_print();
        printf("\nAlloc p3 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_3, num_units_3);
        p3 = (int*)Mem_alloc(num_bytes_3);
        Mem_print();
        printf("\nAlloc p4 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_4, num_units_4);
        p4 = (int*)Mem_alloc(num_bytes_4);
        Mem_print();
        printf("\nAlloc p5 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_5, num_units_5);
        p5 = (int*)Mem_alloc(num_bytes_5);
        Mem_print();
        printf("\nAlloc p6 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_6, num_units_6);
        p6 = (int*)Mem_alloc(num_bytes_6);
        Mem_print();
        printf("\nAlloc p7 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_7, num_units_7);
        p7 = (int*)Mem_alloc(num_bytes_7);
        Mem_print();


        printf("\nFreeing p1 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_1, num_units_1);
        Mem_free(p1);
        Mem_print();
        printf("\nFreeing p3 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_3, num_units_3);
        Mem_free(p3);
        Mem_print();
        printf("\nFreeing p5 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_5, num_units_5);
        Mem_free(p5);
        Mem_print();
        printf("\nFreeing p2 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_2, num_units_2);
        Mem_free(p2);
        Mem_print();
        printf("\nFreeing p4 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_4, num_units_4);
        Mem_free(p4);
        Mem_print();
        printf("\nFreeing p6 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_6, num_units_6);
        Mem_free(p6);
        Mem_print();
        Mem_stats();
        printf("\nFreeing p7 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_7, num_units_7);
        Mem_free(p7);
        Mem_print();
        Mem_stats();
        printf("end unit test 3\n");
    } else if (dprms.UnitDriver == 4){
        //   -- combinations of requests and frees such that the free list is empty
        //   -- demonstrate all patterns in coalescing
        //   -- show that rover spreads allocatins in list and does not cluster
        //      fragments at head of the free list

        int *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10, *p11, *p12, *p13, *p14, *p15, *p16, *p17;
        int num_bytes_1, num_bytes_2, num_bytes_3, num_bytes_4, num_bytes_5, num_bytes_6, num_bytes_7, num_bytes_8, num_bytes_9;
        int num_bytes_10, num_bytes_11, num_bytes_12, num_bytes_13, num_bytes_14, num_bytes_15, num_bytes_16, num_bytes_17;
        int num_units_1, num_units_2, num_units_3, num_units_4, num_units_5, num_units_6, num_units_7, num_units_8, num_units_9;
        int num_units_10, num_units_11, num_units_12, num_units_13, num_units_14, num_units_15, num_units_16, num_units_17;

        num_bytes_1 = 6*sizeof(mem_chunk_t);
        num_bytes_2 = 7*sizeof(mem_chunk_t);
        num_bytes_3 = 8*sizeof(mem_chunk_t);
        num_bytes_4 = 9*sizeof(mem_chunk_t);
        num_bytes_5 = 10*sizeof(mem_chunk_t);
        num_bytes_6 = 11*sizeof(mem_chunk_t);
        num_bytes_7 = 12*sizeof(mem_chunk_t);
        num_bytes_8 = 13*sizeof(mem_chunk_t);
        num_bytes_9 = 14*sizeof(mem_chunk_t);
        num_bytes_10 = 15*sizeof(mem_chunk_t);
        num_bytes_11 = 16*sizeof(mem_chunk_t);
        num_bytes_12 = 17*sizeof(mem_chunk_t);
        num_bytes_13 = 18*sizeof(mem_chunk_t);
        num_bytes_14 = 19*sizeof(mem_chunk_t);
        num_bytes_15 = 20*sizeof(mem_chunk_t);
        num_bytes_16 = 21*sizeof(mem_chunk_t);
        num_bytes_17 = 22*sizeof(mem_chunk_t);
        // num_bytes_18 = num_bytes_8;
        
        num_units_1 = (num_bytes_1/sizeof(mem_chunk_t))+((num_bytes_1%sizeof(mem_chunk_t))>0);
        num_units_2 = (num_bytes_2/sizeof(mem_chunk_t))+((num_bytes_2%sizeof(mem_chunk_t))>0);
        num_units_3 = (num_bytes_3/sizeof(mem_chunk_t))+((num_bytes_3%sizeof(mem_chunk_t))>0);
        num_units_4 = (num_bytes_4/sizeof(mem_chunk_t))+((num_bytes_4%sizeof(mem_chunk_t))>0);
        num_units_5 = (num_bytes_5/sizeof(mem_chunk_t))+((num_bytes_5%sizeof(mem_chunk_t))>0);
        num_units_6 = (num_bytes_6/sizeof(mem_chunk_t))+((num_bytes_6%sizeof(mem_chunk_t))>0);
        num_units_7 = (num_bytes_7/sizeof(mem_chunk_t))+((num_bytes_7%sizeof(mem_chunk_t))>0);
        num_units_8 = (num_bytes_8/sizeof(mem_chunk_t))+((num_bytes_8%sizeof(mem_chunk_t))>0);
        num_units_9 = (num_bytes_9/sizeof(mem_chunk_t))+((num_bytes_9%sizeof(mem_chunk_t))>0);
        num_units_10 = (num_bytes_10/sizeof(mem_chunk_t))+((num_bytes_10%sizeof(mem_chunk_t))>0);
        num_units_11 = (num_bytes_11/sizeof(mem_chunk_t))+((num_bytes_11%sizeof(mem_chunk_t))>0);
        num_units_12 = (num_bytes_12/sizeof(mem_chunk_t))+((num_bytes_12%sizeof(mem_chunk_t))>0);
        num_units_13 = (num_bytes_13/sizeof(mem_chunk_t))+((num_bytes_13%sizeof(mem_chunk_t))>0);
        num_units_14 = (num_bytes_14/sizeof(mem_chunk_t))+((num_bytes_14%sizeof(mem_chunk_t))>0);
        num_units_15 = (num_bytes_15/sizeof(mem_chunk_t))+((num_bytes_15%sizeof(mem_chunk_t))>0);
        num_units_16 = (num_bytes_16/sizeof(mem_chunk_t))+((num_bytes_16%sizeof(mem_chunk_t))>0);
        num_units_17 = (num_bytes_17/sizeof(mem_chunk_t))+((num_bytes_17%sizeof(mem_chunk_t))>0);
        // num_units_18 = (num_bytes_18/sizeof(mem_chunk_t))+((num_bytes_18%sizeof(mem_chunk_t))>0);
        
        printf("Alloc p1 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_1, num_units_1);
        p1 = (int*)Mem_alloc(num_bytes_1);
        // Mem_print();
        printf("Alloc p2 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_2, num_units_2);
        p2 = (int*)Mem_alloc(num_bytes_2);
        // Mem_print();
        printf("Alloc p3 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_3, num_units_3);
        p3 = (int*)Mem_alloc(num_bytes_3);
        // Mem_print();
        printf("Alloc p4 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_4, num_units_4);
        p4 = (int*)Mem_alloc(num_bytes_4);
        // Mem_print();
        printf("Alloc p5 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_5, num_units_5);
        p5 = (int*)Mem_alloc(num_bytes_5);
        // Mem_print();
        printf("Alloc p6 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_6, num_units_6);
        p6 = (int*)Mem_alloc(num_bytes_6);
        // Mem_print();
        printf("Alloc p7 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_7, num_units_7);
        p7 = (int*)Mem_alloc(num_bytes_7);
        // Mem_print();
        printf("Alloc p8 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_8, num_units_8);
        p8 = (int*)Mem_alloc(num_bytes_8);
        // Mem_print();
        printf("Alloc p9 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_9, num_units_9);
        p9 = (int*)Mem_alloc(num_bytes_9);
        // Mem_print();
        printf("Alloc p10 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_10, num_units_10);
        p10 = (int*)Mem_alloc(num_bytes_10);
        // Mem_print();
        printf("Alloc p11 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_11, num_units_11);
        p11 = (int*)Mem_alloc(num_bytes_11);
        // Mem_print();
        printf("Alloc p12 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_12, num_units_12);
        p12 = (int*)Mem_alloc(num_bytes_12);
        // Mem_print();
        printf("Alloc p13 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_13, num_units_13);
        p13 = (int*)Mem_alloc(num_bytes_13);
        // Mem_print();
        printf("Alloc p14 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_14, num_units_14);
        p14 = (int*)Mem_alloc(num_bytes_14);
        // Mem_print();
        printf("Alloc p15 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_15, num_units_15);
        p15 = (int*)Mem_alloc(num_bytes_15);
        // Mem_print();
        printf("Alloc p16 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_16, num_units_16);
        p16 = (int*)Mem_alloc(num_bytes_16);
        // Mem_print();
        printf("Alloc p17 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_17, num_units_17);
        p17 = (int*)Mem_alloc(num_bytes_17);
        // Mem_print();

        printf("\n\nFreeing all blocks in pseudo-random order, ");
        if (dprms.Coalescing){
            printf("resulting in a single block\n");
        } else {
            printf("creating multiple small blocks\n");
        }

        Mem_free(p1);
        Mem_free(p7);
        Mem_free(p14);
        Mem_free(p2);
        Mem_free(p8);
        Mem_free(p15);
        Mem_free(p3);
        Mem_free(p9);
        Mem_free(p16);
        Mem_free(p4);
        Mem_free(p10);
        Mem_free(p17);
        Mem_free(p5);
        Mem_free(p11);
        Mem_free(p6);
        Mem_free(p12);
        Mem_free(p13);
        Mem_print();

        printf("\nRepeating allocations, freeing along the way\n");


        printf("\nAlloc p1 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_1, num_units_1);
        p1 = (int*)Mem_alloc(num_bytes_1);
        Mem_print();
        printf("\nAlloc p6 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_6, num_units_6);
        p6 = (int*)Mem_alloc(num_bytes_6);
        Mem_print();
        printf("\nFree p1 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_1, num_units_1);
        Mem_free(p1);
        Mem_print();
        printf("\nAlloc p12 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_12, num_units_12);
        p12 = (int*)Mem_alloc(num_bytes_12);
        Mem_print();
        printf("\nFree p6 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_6, num_units_6);
        Mem_free(p6);
        Mem_print();
        printf("\nAlloc p2 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_2, num_units_2);
        p2 = (int*)Mem_alloc(num_bytes_2);
        Mem_print();
        printf("Free p12 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_12, num_units_12);
        Mem_free(p12);
        Mem_print();
        printf("\nAlloc p17 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_17, num_units_17);
        printf("Free p2 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_2, num_units_2);
        p17 = (int*)Mem_alloc(num_bytes_17);
        Mem_print();
        printf("Free p2 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_2, num_units_2);
        Mem_free(p2);
        Mem_print();
        printf("\nAlloc p9 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_9, num_units_9);
        p9 = (int*)Mem_alloc(num_bytes_9);
        Mem_print();
        printf("Free p17 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_17, num_units_17);
        Mem_free(p17);
        Mem_print();
        printf("\nAlloc p15 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_15, num_units_15);
        p15 = (int*)Mem_alloc(num_bytes_15);
        Mem_print();
        printf("Free p9 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_9, num_units_9);
        Mem_free(p9);
        Mem_print();
        printf("\nAlloc p8 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_8, num_units_8);
        p8 = (int*)Mem_alloc(num_bytes_8);
        Mem_print();
        printf("Free p15 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_15, num_units_15);
        Mem_free(p15);
        Mem_print();
        printf("\nAlloc p10 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_10, num_units_10);
        p10 = (int*)Mem_alloc(num_bytes_10);
        Mem_print();
        printf("Free p8 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_8, num_units_8);
        Mem_free(p8);
        Mem_print();
        printf("\nAlloc p3 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_3, num_units_3);
        p3 = (int*)Mem_alloc(num_bytes_3);
        Mem_print();
        printf("Free p10 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_10, num_units_10);
        Mem_free(p10);
        Mem_print(); 
        printf("\nAlloc p14 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_14, num_units_14);
        p14 = (int*)Mem_alloc(num_bytes_14);
        Mem_print();
        printf("Free p3 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_3, num_units_3);
        Mem_free(p3);
        Mem_print();
        printf("\nAlloc p16 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_16, num_units_16);
        p16 = (int*)Mem_alloc(num_bytes_16);
        Mem_print();
        printf("Free p14 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_14, num_units_14);
        Mem_free(p14);
        Mem_print();
        printf("\nAlloc p13 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_13, num_units_13);
        p13 = (int*)Mem_alloc(num_bytes_13);
        Mem_print();
        printf("Free p16 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_16, num_units_16);
        Mem_free(p16);
        Mem_print();
        printf("\nAlloc p4 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_4, num_units_4);
        p4 = (int*)Mem_alloc(num_bytes_4);
        Mem_print();
        printf("Free p13 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_13, num_units_13);
        Mem_free(p13);
        Mem_print();
        printf("\nAlloc p7 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_7, num_units_7);
        p7 = (int*)Mem_alloc(num_bytes_7);
        Mem_print();
        printf("Free p4 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_4, num_units_4);
        Mem_free(p4);
        Mem_print();//
        printf("\nAlloc p5 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_5, num_units_5);
        p5 = (int*)Mem_alloc(num_bytes_5);
        Mem_print();
        printf("Free p7 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_7, num_units_7);
        Mem_free(p7);
        Mem_print();
        printf("\nAlloc p11 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_11, num_units_11);
        p11 = (int*)Mem_alloc(num_bytes_11);
        Mem_print();
        printf("Free p5 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_5, num_units_5);
        Mem_free(p5);
        Mem_print();
        printf("\nFree p11 (%d bytes, %d units (+1 w/ mem_chunk_t header))\n", num_bytes_11, num_units_11);
        Mem_free(p11);
        Mem_print();
        Mem_stats();
        printf("end unit test 4\n");
    }
    // test for performance in equilibrium 
    if (dprms.EquilibriumTest)
        equilibriumDriver(&dprms);

    exit(0);
}

/* ----- equilibriumDriver -----
 *
 * This is a driver to test the performance of the dynamic memory allocation
 * and free calls in equilibrium.  This code assumes that the functions are
 * defined in mem.h 
 *
 * The driver allocates dynamic memory for variable sized arrays of integers.
 * The size of an array is uniformly distributed in the range [avg-range,
 * avg+range].  The average size and range can be specified on the command
 * line.
 *
 * During a warmup phase, calls are made to allocate the integer arrays and
 * the arrays are stored in an unsorted list using the twl_list.c module.  
 *
 * During the equilibrium phase, the code randomly chooses to either allocate a
 * new array, or return one of the arrays stored in the list.  The events are
 * equally likely.  If an array is removed from the list and freed, one of the
 * list items is choosen with an equal probability over all items in the list.
 *
 * Finally, the last phase frees all arrays stored in the list.
 *
 * At the end of each phase, Mem_stats is called to print information about
 * the size of the free list.  In verbose mode, Mem_print is called to print
 * the address and size of each item in the free list.  Only enable verbose
 * mode when testing with small warmup and trial phases.
 *
 * The following parameters can be set on the command line.  If not set,
 * default values are used.
 *
 * -w 1000   number of warmup allocations
 * -t 100000 number of trials in equilibrium
 * -a 128    average size of interger array
 * -r 127    range for average size of interger array
 * -d        use system malloc/free instead of MP4 versions
 */
void equilibriumDriver(driver_params *ep)
{
    int i, index;
    int range_num_ints;
    int min_num_ints;
    int *ptr;
    int size;
    int pos;
    twl_list_t *mem_list;
    clock_t start, end;

    // print parameters for this test run 
    printf("\nEquilibrium test driver using ");
    if (ep->SysMalloc)
        printf("system malloc and free\n");
    else
        printf("Mem_alloc and Mem_free from mem.c\n");
    printf("  Trials in equilibrium: %d\n", ep->Trials);
    printf("  Warmup allocations: %d\n", ep->WarmUp);
    printf("  Average array size: %d\n", ep->AvgNumInts);
    printf("  Range for average array size: %d\n", ep->RangeInts);

    mem_list = twl_list_construct(NULL);
    // the size of the integer array is uniformly distributed in the range
    // [avg-range, avg+range]

    range_num_ints = 2 * ep->RangeInts + 1;
    min_num_ints = ep->AvgNumInts - ep->RangeInts;
    if (min_num_ints < 1 || ep->AvgNumInts < 1 || ep->RangeInts < 0) {
        printf("The average array size must be positive and greater than the range\n");
        exit(1);
    }

    // warmup by allocating memory and storing in list 
    for (i = 0; i < ep->WarmUp; i++) {
        // random size of array 
        size = ((int) (drand48() * range_num_ints)) + min_num_ints;
        if (ep->SysMalloc)
            ptr = (int *) malloc(size * sizeof(int));
        else
            ptr = (int *) Mem_alloc(size * sizeof(int));
        assert(ptr != NULL);
        // first position is size of array.  fill rest with numbers 
        ptr[0] = -size;
        for (index = 1; index < size; index++)
            ptr[index] = -index;   // same as *(ptr+index)=index 
        twl_list_insert(mem_list, (mydata_t *) ptr, TWL_LIST_BACK);
        ptr = NULL;
    }
    printf("After warmup\n");
    if (!ep->SysMalloc) {
        Mem_stats();
        if (ep->Verbose) Mem_print();
    } 

    // in equilibrium make allocations and frees with equal probability 
    start = clock();
    for (i = 0; i < ep->Trials; i++) {
        if (drand48() < 0.5) {
            size = ((int) (drand48() * range_num_ints)) + min_num_ints;
            // if (ep->Verbose) {
            //     // uncomment following print for more detail
            //     // printf("\n\n  list before allocation of size %ld (%ld units)\n", size*sizeof(int), size*sizeof(int)/sizeof(mem_chunk_t)+((size*sizeof(int))%sizeof(mem_chunk_t)>0)+1); 
            //     // Mem_print(); // WARNING this breaks -d
            // }
            if (ep->SysMalloc)
                ptr = (int *) malloc(size * sizeof(int));
            else {
                ptr = (int *) Mem_alloc(size * sizeof(int));
            }
            assert(ptr != NULL);
            ptr[0] = -size;
            for (index = 1; index < size; index++)
                ptr[index] = -index;
            twl_list_insert(mem_list, (mydata_t *) ptr, TWL_LIST_BACK);
            ptr = NULL;
        } else if (twl_list_size(mem_list) > 0) {
            pos = (int) (drand48() * twl_list_size(mem_list));
            ptr = (int *) twl_list_remove(mem_list, pos);
            assert(ptr != NULL);
            size = -ptr[0];
            
            // if (ep->Verbose) {
            //     // uncomment following print for more detail
            //     // printf("\n\n\n  list before freeing block with size %ld (%ld units) from position %d\n", size*sizeof(int), (size*sizeof(int))/sizeof(mem_chunk_t)+((size*sizeof(int))%sizeof(mem_chunk_t)>0)+1, pos); 
            //     // Mem_print(); // WARNING this breaks -d
            // }
            assert(min_num_ints <= size && size <= ep->AvgNumInts+ep->RangeInts);
            for (index = 1; index < size; index++)
                assert(ptr[index] == -index);
            if (ep->SysMalloc)
                free(ptr);
            else {
                Mem_free(ptr);
            }
            ptr = NULL;
        }
    }
    end = clock();
    printf("After exercise, time=%g\n",
            1000*((double)(end-start))/CLOCKS_PER_SEC);
    if (!ep->SysMalloc) {
        Mem_stats();
        if (ep->Verbose) Mem_print();
    } 

    // remove and free all items from mem_list
    pos = twl_list_size(mem_list);
    for (i = 0; i < pos; i++) {
        
        ptr = (int *) twl_list_remove(mem_list, 0);
        assert(ptr != NULL);
        size = -ptr[0];
        assert(min_num_ints <= size && size <= ep->AvgNumInts+ep->RangeInts);
        for (index = 1; index < size; index++)
            assert(ptr[index] == -index);
        if (ep->SysMalloc)
            free(ptr);
        else
            Mem_free(ptr);
        ptr = NULL;
    }
    assert(twl_list_size(mem_list) == 0);
    twl_list_destruct(mem_list);

    printf("After cleanup\n");
    if (!ep->SysMalloc) {
        if (ep->Verbose) Mem_print();
        Mem_stats();
    } 
    printf("----- End of equilibrium test -----\n\n");
}


/* read in command line arguments.  
 *
 * -u 0  is for the unit drivers, starting with driver 0
 *
 * Other command line arguments are for the equilibrium driver parameters.
 */
void getCommandLine(int argc, char **argv, driver_params *ep)
{
    /* The geopt function creates three global variables:
     *    optopt--if an unknown option character is found
     *    optind--index of next element in argv 
     *    optarg--argument for option that requires argument 
     *
     * The third argument to getopt() specifies the possible argument flags
     *   If an argument flag is followed by a colon, then an argument is 
     *   expected. E.g., "x:y" means -x must have an argument but not -y
     */
    int c;
    int index;


    ep->SearchPolicy = FIRST_FIT;
    ep->Coalescing = FALSE;
    ep->Seed = 10262023;
    ep->Verbose = FALSE;
    ep->EquilibriumTest = FALSE;
    ep->WarmUp = 1000;
    ep->Trials = 100000;
    ep->AvgNumInts = 128;
    ep->RangeInts = 127;
    ep->SysMalloc = FALSE;
    ep->UnitDriver = -1;

    while ((c = getopt(argc, argv, "w:t:s:a:r:f:u:cdve")) != -1) {
        switch(c) {
            case 'u': ep->UnitDriver = atoi(optarg);   break;
            case 'w': ep->WarmUp = atoi(optarg);       break;
            case 't': ep->Trials = atoi(optarg);       break;
            case 's': ep->Seed = atoi(optarg);         break;
            case 'a': ep->AvgNumInts = atoi(optarg);   break;
            case 'r': ep->RangeInts = atoi(optarg);    break;
            case 'd': ep->SysMalloc = TRUE;            
            printf("new malloc on\n"); break;
            case 'v': ep->Verbose = TRUE;              break;
            case 'e': ep->EquilibriumTest = TRUE;      break;
            case 'c': ep->Coalescing = TRUE;               break;
            case 'f':
                  if (strcmp(optarg, "best") == 0)
                      ep->SearchPolicy = BEST_FIT;
                  else if (strcmp(optarg, "first") == 0)
                      ep->SearchPolicy = FIRST_FIT;
                  else {
                      fprintf(stderr, "invalid search policy: %s\n", optarg);
                      exit(1);
                  }
                  break;
            case '?':
                  if (isprint(optopt))
                      fprintf(stderr, "Unknown option %c.\n", optopt);
                  else
                      fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            default:
                  printf("Lab4 command line options\n");
                  printf("General options ---------\n");
                  printf("  -v        turn on verbose prints (default off)\n");
                  printf("  -s 54321  seed for random number generator\n");
                  printf("  -c        turn on coalescing (default off)\n");
                  printf("  -f        best|first\n");
                  printf("            search policy to find memory block (first by default)\n");
                  printf("  -u 0      run unit test driver\n");
                  printf("  -e        run equilibrium test driver\n");
                  printf("\nOptions for equilibrium test driver ---------\n");
                  printf("  -w 1000   number of warmup allocations\n");
                  printf("  -t 100000 number of trials in equilibrium\n");
                  printf("  -a 128    average size of interger array\n");
                  printf("  -r 127    range for average size of array\n");
                  printf("  -d        use system malloc/free instead of MP4 versions\n");
                  exit(1);
        }
    }
    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
}

/* vi:set ts=8 sts=4 sw=4 et: */
