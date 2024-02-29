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
    // twl_list_t *sorted_list;
    unsorted_list = twl_list_construct(NULL);  // because the argument is null the list is unsorted
    // sorted_list = twl_list_construct(&ap_rank_aps);  // because the argument is null the list is unsorted
    // sorted_list = twl_list_construct(&ap_rank_eth);  // because the argument is null the list is unsorted

    // create item to test twl_list_insert
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 1;
    datap->mobile_count = 444;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 6;
    datap->mobile_count = 333;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 2;
    datap->mobile_count = 22;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 7;
    datap->mobile_count = 111;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 3;
    datap->mobile_count = 9000;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 8;
    datap->mobile_count = 10;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 4;
    datap->mobile_count = 200;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 40;
    datap->mobile_count = 200;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 5;
    datap->mobile_count = 2000;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    datap->eth_address = 50;
    datap->mobile_count = 9000;
    twl_list_insert(unsorted_list, datap, TWL_LIST_BACK);
    datap = NULL;
    // driver_debug_print(unsorted_list);

    // datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    // datap->eth_address = 504;
    // datap->mobile_count = 444;
    // twl_list_insert(unsorted_list, datap, 1);
    // datap = NULL;
    // driver_debug_print(unsorted_list);

    // datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    // datap->eth_address = 506;
    // datap->mobile_count = 333;
    // twl_list_insert(unsorted_list, datap, 1);
    // datap = NULL;
    // driver_debug_print(unsorted_list);

    // datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    // datap->eth_address = 508;
    // datap->mobile_count = 222;
    // twl_list_insert(unsorted_list, datap, 1);
    // datap = NULL;
    // driver_debug_print(unsorted_list);

    // datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    // datap->eth_address = 502;
    // datap->mobile_count = 111;
    // twl_list_insert(unsorted_list, datap, 1);
    // datap = NULL;
    // driver_debug_print(unsorted_list);


    // printf("\nSecond test using access.  List should be {555, 500, 444}\n\n");
    // // find all three and print 
    // datap = twl_list_access(unsorted_list, 0);   // same as TWL_LIST_FRONT 
    // if (datap != NULL) {
    //     printf("Position 0 should find 555 and found (%d)\n", datap->eth_address);
    //     datap = twl_list_access(unsorted_list, 1);
    //     printf("Position 1 should find 500 found (%d)\n", datap->eth_address);
    //     datap = twl_list_access(unsorted_list, TWL_LIST_BACK);
    //     printf("Position 2 should find 444 and found (%d)\n", datap->eth_address);
    // } else {
    //     printf("ERROR: list access is broken?\n");
    // }

    // // Next try to use driver_debug_print 
    // printf("\nTest of list print\n\n");
    // driver_debug_print(unsorted_list);
    // printf("starting insertion sort\n");
    // unsorted_list->ll_comp_function = &ap_rank_eth;
    // unsorted_list->ll_comp_function = &ap_rank_aps;

    // twl_list_insertion_sort(unsorted_list, &ap_rank_aps);
    // driver_debug_print(unsorted_list);
    // ap_inc(unsorted_list, 500);
    // ap_inc(unsorted_list, 500);
    // ap_inc(unsorted_list, 500);
    // ap_inc(unsorted_list, 12345);
    // ap_inc(unsorted_list, 12345);
    // twl_list_insertion_sort(unsorted_list, &ap_rank_aps);
    // driver_debug_print(unsorted_list);



    // twl_list_find_max(unsorted_list, unsorted_list->ll_front, unsorted_list->ll_back);
    // twl_list_selection_sort_recursive(unsorted_list, unsorted_list->ll_front, unsorted_list->ll_back);
    // twl_list_selection_sort_iterative(unsorted_list, unsorted_list->ll_front, unsorted_list->ll_back);
   printf("starting merge sort\n");

//    sorted_list = twl_list_sort(unsorted_list, 4, &ap_rank_eth);
   twl_list_sort(unsorted_list, 4, &ap_rank_eth);
//    twl_list_sort(unsorted_list, 4, &ap_rank_aps);

    // driver_debug_print(unsorted_list);
   
   
   
    // you should repeat the above tests with differents insertion orders
 
    // next you may want to work on twl_insert_sorted, 
    //*********************************************************************
    // printf("\nTest of sorted inserts\n");
    // twl_list_t *sorted_list = twl_list_construct(ap_rank_aps);

    // datap = (mydata_t *) calloc(1,sizeof(mydata_t));
    // datap->eth_address = 31;
    // twl_list_insert_sorted(sorted_list, datap);

    // // driver_debug_print(sorted_list);
    // // add a second item
    // datap = (mydata_t *) calloc(1,sizeof(mydata_t));
    // datap->eth_address = 7;
    // twl_list_insert_sorted(sorted_list, datap);

    // // driver_debug_print(sorted_list);
    // // add a third item
    // datap = (mydata_t *) calloc(1,sizeof(mydata_t));
    // datap->eth_address = 15;
    // twl_list_insert_sorted(sorted_list, datap);

    // // driver_debug_print(sorted_list);
    // // add a fourth item
    // datap = (mydata_t *) calloc(1,sizeof(mydata_t));
    // datap->eth_address = 63;
    // twl_list_insert_sorted(sorted_list, datap);

    // driver_debug_print(sorted_list);

    // printf("\nHere is the sorted list should be {7, 15, 31, 63}\n\n");
    // driver_debug_print(sorted_list);

    // // datap = 0;
    // datap = twl_list_remove(sorted_list, 2);
    // printf("removed: %d\n", datap->eth_address);
    // free(datap);
    // driver_debug_print(sorted_list);
    
    // twl_list_destruct(sorted_list);
    // //*****************************************/
    // // after the insertion functions work, you can try the find command

    // printf("\nTest of list elem find position\n");
    // mydata_t *foundp;
    // datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    // datap->eth_address = 444;
    // int my_pos = twl_list_elem_find_position(unsorted_list, datap, ap_match_eth);
    // if (my_pos != -1) {
    //     printf("Record %d found in position %d\n", datap->eth_address, my_pos);
    //     foundp = twl_list_access(unsorted_list, my_pos);
    //     if (foundp != NULL) {
    //         printf("looked for %d and found %d\n",
    //                 datap->eth_address, foundp->eth_address);
    //         if (datap->eth_address != foundp->eth_address) {
    //             printf("ERROR: returned pointer to wrong record\n");
    //         }
    //     } else {
    //         printf("ERROR: access did not get position %d\n", my_pos);
    //     }
    // } else {
    //     printf("looked for %d and did not find \n", datap->eth_address);
    // }
    // free(datap);  // but not foundp
    // datap = foundp = NULL;

    // printf("\nTest of list elem find data ptr\n");
    // datap = (mydata_t *) calloc(1, sizeof(mydata_t));
    // datap->eth_address = 500;
    // foundp = twl_list_elem_find_data_ptr(unsorted_list, datap, ap_match_eth);
    // if (foundp != NULL) {
    //     printf("looked for %d and found %d\n",
    //             datap->eth_address, foundp->eth_address);
    //     if (datap->eth_address != foundp->eth_address) {
    //         printf("ERROR: returned pointer to wrong record\n");
    //     }
    // } else {
    //     printf("ERROR: elem find for data ptr did not find %d\n", datap->eth_address);
    // }
    free(datap);  // but not foundp
    datap = NULL;
       // *************************************************************/

    twl_list_destruct(unsorted_list);
    printf("fin\n");
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
        printf("Pos: %d contains IP: %d\t- %d\n", count, rover->data_ptr->eth_address, rover->data_ptr->mobile_count);
        rover = rover->next;
        count++;
    }
    if (count != list_ptr->ll_count)
        printf("\nERROR\n  Hey!! the list size is wrong! count is %d\n", count);
}

