/* geninput.c
 * MP3
 * Harlan Russell
 * ECE 2230, Fall 2023
 *
 * Purpose: Generate an input file for MP3 
 * Assumptions: Input file is for testing sort.  Creates
 *              an unsorted list, uses APPENDQ to insert items, and
 *              calls SORT to sort the list
 *
 * Command line arguments:
 *    1st -- field for sorting
 *           eth: eth_address
 *           acc: access_point
 *    2nd -- number of records to create
 *    3rd -- type of list to create
 *           1: random addresses [0, 3/4 number records)
 *           2: assending and sequential
 *           3: descending and sequential
 *    4th -- type of sort
 *           1: Insertion
 *           2: Recursive Selection
 *           3: Iterative Selection
 *           4: Merge
 *
 * Pipe the output of this program into lab3. For example
 *     ./geninput eth 10000 1 1 | ./lab3 3
 *
 * For mc, there are no ties because eth_address are unique
 * For eth, there are no ties because a permutation
 *
 * See also mp3test.sh 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    int records = 0, list_type = -1, sort_type = -1, field_type = -1;
    int addr_range;
    int eth_addr;
    int seed = 10022023;

    if (argc != 5 && argc != 6) {
        printf("Usage: ./geninput field listsize listype sorttype [seed]\n");
        printf("\tfield   : eth or mc\n");
        printf("\tlistsize: number of records to create\n");
        printf("\tlisttype: {1, 2, 3} for random, ascending or descending\n");
        printf("\tsorttype: 1: Insertion\n");
        printf("\t          2: Recursive Selection\n");
        printf("\t          3: Iterative Selection\n");
        printf("\t          4: Merge\n");
        printf("\tseed: optional seed for random number generator\n");
        exit(1);
    }
    if (strcmp(argv[1], "eth") == 0) {
        field_type = 1;
    } else if (strcmp(argv[1], "mc") == 0) {
        field_type = 2;
    } else {
        printf("genniput has invalid field : %s\n", argv[1]);
        printf("\tfield   : eth or mc\n");
        exit(2);
    }
    records = atoi(argv[2]);
    if (records < 2) {
        printf("genniput has invalid number records: %d\n", records);
        exit(2);
    }
    list_type = atoi(argv[3]);
    sort_type = atoi(argv[4]);
    if (sort_type < 1 || sort_type > 4) {
        printf("genniput has invalid type of sort: %d\n", sort_type);
        exit(2);
    }
    if (argc == 6) {
        int temp_seed = -1;
        temp_seed = atoi(argv[5]);
        if (temp_seed > 0)
            seed = temp_seed;
    }
    if (records <= 20)
        printf("Seed %d\n", seed);
    addr_range = records * 0.5;
    srand48(seed);

    int i;
    if (list_type == 1) {
        if (field_type == 2) {
            // mc random addresses, some duplicates for ids
            for (i = 0; i < records; i++) {
                if ((records >= 8) && (i == records/4 || i == records/2 || i == 3*records/4)) {
                    eth_addr = records;
                } else if ((records >= 9) && (i == records/3 || i == 2*records/3)) {
                    eth_addr = 0;
                } else {
                    // 1 to addr_range
                    eth_addr = (int) (addr_range * drand48()) + 1;
                }
                printf("APPENDQ %d %d\n", i+1, eth_addr);
            }
        } else {
            // eth random permutation, no duplicates for ethernet addresses
            int * narray = (int *) malloc(records*sizeof(int));
            for (i = 0; i<records; i++)
                narray[i] = i;
            for (i = 0; i<records; i++) {
                int key = (int) (drand48() * (records - i)) + i;
                assert(i <= key && key < records);
                int temp = narray[i]; narray[i] = narray[key]; narray[key] = temp;
                printf("APPENDQ %d %d\n", narray[i]+1, i+1);
            }
            free(narray);
        }
    }
    else if (list_type == 2) {
        // ascending addresses
        for (i = 0; i < records; i++) {
            eth_addr = i+1;
            printf("APPENDQ %d %d\n", eth_addr, eth_addr);
        }
    }
    else if (list_type == 3) {
        // descending addresses
        for (i = 0; i < records; i++) {
            eth_addr = records - i;
            printf("APPENDQ %d %d\n", eth_addr, eth_addr);
        }
    }
    else {
        printf("geninput has invalid list type: %d\n", list_type);
        exit(3);
    }
    if (field_type == 1)
        printf("SORTETH %d\n", sort_type);
    else
        printf("SORTAP %d\n", sort_type);
    if (records <= 20)
        printf("PRINTQ\n");
    printf("QUIT\n");
    exit(0);
}

