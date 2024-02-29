/*  driver.c

   Writing and debugging MP2 is challenging, and it is particularly hard if you
   try and write all the code before you do any testing. It is much easier to
   break the problem into small pieces and test each piece. 

   For MP2, one approach is to write twl_list_insert first, since it might be the
   easiest algorithm and it depends only on twl_list_construct.

   For testing, we use a simple main function in this file.  This file is for 
   testing only, and you will NOT submit it.  It is for your personal
   testing during development.

   Below are a few simple examples that allocates a few structures and inserts 
   them into a list.

   With this file you can compile and run list.c on its own using 
       gcc -Wall -g driver.c list.c ap_support.c -o driver

   The compling command is also found in the provided makefile.  Do
       make driver

   Do this testing before you make ANY changes to ap_support.c  The only 
   reason we need to compile ap_support.c is for the ap_compare function.
   The linker needs to find the comp_proc function and it is found in the 
   template ap_support.c file (with no changes) 

*/

#include <stdlib.h>
#include <stdio.h>
#include "twl_list.h"
#include "ap_support.h"

/* prototype for list debug print */
void driver_debug_print(twl_list_t *list_ptr);

int main(void)
{
    mydata_t *datap;
    twl_list_t *unsorted_list;
    unsorted_list = twl_list_construct(NULL);  // because the argument is null the list is unsorted

    // create item to test twl_list_insert
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 515;
    driver_debug_print(unsorted_list);
    printf("xxxxxxxxxxx\n");
    twl_list_insert(unsorted_list, datap, TWL_LIST_FRONT);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 12345;
    driver_debug_print(unsorted_list);
    printf("xxxxxxxxxx1\n");
    twl_list_insert(unsorted_list, datap, TWL_LIST_FRONT);
    driver_debug_print(unsorted_list);
    printf("xxxxxxxxxx2\n");

    // test list_access with one item in list
    datap = twl_list_access(unsorted_list, TWL_LIST_FRONT);
    if (datap != NULL) {
        printf("first test should find 555 and found (%d)\n\n", datap->eth_address);
    } else {
        printf("ERROR: list access is broken?\n");
    }
    datap = NULL;

    // add a second item to tail of the list
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 444;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    driver_debug_print(unsorted_list);

    // insert a new item in the middle 
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 500;
    twl_list_insert(unsorted_list, datap, 1);
    datap = NULL;
    driver_debug_print(unsorted_list);


    printf("\nSecond test using access.  List should be {555, 500, 444}\n\n");
    // find all three and print 
    datap = twl_list_access(unsorted_list, 0);   // same as TWL_LIST_FRONT 
    if (datap != NULL) {
        printf("Position 0 should find 555 and found (%d)\n", datap->eth_address);
        datap = twl_list_access(unsorted_list, 1);
        printf("Position 1 should find 500 found (%d)\n", datap->eth_address);
        datap = twl_list_access(unsorted_list, TWL_LIST_BACK);
        printf("Position 2 should find 444 and found (%d)\n", datap->eth_address);
    } else {
        printf("ERROR: list access is broken?\n");
    }

    // Next try to use driver_debug_print 
    printf("\nTest of list print\n\n");
    driver_debug_print(unsorted_list);

    // you should repeat the above tests with differents insertion orders
 
    // next you may want to work on twl_insert_sorted, 
    //*********************************************************************
    printf("\nTest of sorted inserts\n");
    twl_list_t *sorted_list = twl_list_construct(ap_rank_aps);

    datap = (mydata_t *) calloc(1,sizeof(mydata_t));
    datap->eth_address = 31;
    twl_list_insert_sorted(sorted_list, datap);

    // driver_debug_print(sorted_list);
    // add a second item
    datap = (mydata_t *) calloc(1,sizeof(mydata_t));
    datap->eth_address = 7;
    twl_list_insert_sorted(sorted_list, datap);

    // driver_debug_print(sorted_list);
    // add a third item
    datap = (mydata_t *) calloc(1,sizeof(mydata_t));
    datap->eth_address = 15;
    twl_list_insert_sorted(sorted_list, datap);

    // driver_debug_print(sorted_list);
    // add a fourth item
    datap = (mydata_t *) calloc(1,sizeof(mydata_t));
    datap->eth_address = 63;
    twl_list_insert_sorted(sorted_list, datap);

    driver_debug_print(sorted_list);

    printf("\nHere is the sorted list should be {7, 15, 31, 63}\n\n");
    driver_debug_print(sorted_list);

    // datap = 0;
    datap = twl_list_remove(sorted_list, 2);
    printf("removed: %d\n", datap->eth_address);
    free(datap);
    driver_debug_print(sorted_list);
    
    twl_list_destruct(sorted_list);
    //*****************************************/
    // after the insertion functions work, you can try the find command

    printf("\nTest of list elem find position\n");
    mydata_t *foundp;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 444;
    int my_pos = twl_list_elem_find_position(unsorted_list, datap, ap_match_eth);
    if (my_pos != -1) {
        printf("Record %d found in position %d\n", datap->eth_address, my_pos);
        foundp = twl_list_access(unsorted_list, my_pos);
        if (foundp != NULL) {
            printf("looked for %d and found %d\n",
                    datap->eth_address, foundp->eth_address);
            if (datap->eth_address != foundp->eth_address) {
                printf("ERROR: returned pointer to wrong record\n");
            }
        } else {
            printf("ERROR: access did not get position %d\n", my_pos);
        }
    } else {
        printf("looked for %d and did not find \n", datap->eth_address);
    }
    free(datap);  // but not foundp
    datap = foundp = NULL;

    printf("\nTest of list elem find data ptr\n");
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 500;
    foundp = twl_list_elem_find_data_ptr(unsorted_list, datap, ap_match_eth);
    if (foundp != NULL) {
        printf("looked for %d and found %d\n",
                datap->eth_address, foundp->eth_address);
        if (datap->eth_address != foundp->eth_address) {
            printf("ERROR: returned pointer to wrong record\n");
        }
    } else {
        printf("ERROR: elem find for data ptr did not find %d\n", datap->eth_address);
    }
    free(datap);  // but not foundp
    datap = foundp = NULL;
       // *************************************************************/

    twl_list_destruct(unsorted_list);
 
    return 0;
}

/*

   Next you will want to write your own driver_debug_print function to print a
   list. Then you can do "before and after" testing. That is, print the list
   before a change, and print the list after the change to verify that the
   change worked.

   Something like

*/

void driver_debug_print(twl_list_t *list_ptr) {
    ll_node_t *rover;
    int count;

    printf("List size: %d\n", list_ptr->ll_count);
    rover = list_ptr->ll_front;
    count = 0;
    while(rover != NULL) {
        printf("Pos: %d contains IP: %d\n", count, rover->data_ptr->eth_address);
        rover = rover->next;
        count++;
    }
    if (count != list_ptr->ll_count)
        printf("\nERROR\n  Hey!! the list size is wrong! count is %d\n", count);
}

