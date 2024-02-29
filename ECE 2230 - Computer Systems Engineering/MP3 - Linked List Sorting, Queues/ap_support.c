/* ap_support.c 
 * Michael Morris
 * Mvmorri
 * ECE 2230 Fall 2023
 * MP3
 *
 * Propose: This file serves as the connection between the data within
 *      each of the list members and the list itself. As all data is "private"
 *      and required to be hidden from lab2.c and twl_list.c, this file contains
 *      functions that can add / remove, access, increment / decrement, print, 
 *      and compare the data within each member. 
 *
 * Assumptions: No member data should be accessed outside of this file. Arguments 
 *      passed to the functions within this file will generally be checked for 
 *      out-of-bounds requests but should be check for accuracy prior to calling
 *      these functions. 
 *
 * Bugs: None found, though exhaustive testing may reveal some.
 *
 * You can modify the interface for any of the ap_ functions if you like
 * But you must clearly document your changes.
 *
 * (You CANNOT modify any of the details of the list.h interface, or use any 
 *  of the private variables outside of list.c.)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "twl_list.h"
#include "ap_support.h"

#include <time.h>

void time_sort(twl_list_t *list_ptr, int sort_type, int (*fcomp)(const mydata_t *, const mydata_t *));

/* ap_rank_aps is required by the linked list ADT for sorted lists. 
 *
 * This function returns 
 *     1 if rec_a should be closer to the front than rec_b,
 *    -1 if rec_b is to be considered closer to the front, and 
 *     0 if the records are equal.
 *
 * For the AP data we want to sort from largest mobile count down, so
 * closer to the front means a larger mobile count address.
 *
 * Also, if the mobile count is tied then the record with the smaller
 * eth_address is closer to the front.
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int ap_rank_aps(const ap_info_t *record_a, const ap_info_t *record_b)
{
    assert(record_a != NULL && record_b !=NULL);

    if (record_a->mobile_count > record_b->mobile_count)
        return 1;
    else if (record_a->mobile_count < record_b->mobile_count)
        return -1;
    else {
        // the mobile counts are equal
        if (record_a->eth_address < record_b->eth_address)
            return 1;
        else if (record_a->eth_address > record_b->eth_address)
            return -1;
        else
            return 0;
    }
}



/* ap_rank_eth is required by the linked list ADT for sorted lists. 
 *
 * This function returns 
 *     1 if rec_a should be closer to the front than rec_b,
 *    -1 if rec_b is to be considered closer to the front, and 
 *     0 if the records are equal.
 *
 * For the Eth data we want to sort from smallest ethernet up, so
 * closer to the front means a larger ethernet address.
 *
 * Also, if the ethernet is tied then the record with the larger
 * mobile_count is closer to the front.
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int ap_rank_eth(const ap_info_t *record_a, const ap_info_t *record_b)
{
    assert(record_a != NULL && record_b !=NULL);

    if (record_a->eth_address < record_b->eth_address)
        return 1;
    else if (record_a->eth_address > record_b->eth_address)
        return -1;
    else {
        // the mobile counts are equal
        if (record_a->mobile_count > record_b->mobile_count)
            return 1;
        else if (record_a->mobile_count < record_b->mobile_count)
            return -1;
        else
            return 0;
    }
}

/* ap_match_eth is used by the list element find for searching for an access point. 
 *
 * This function returns 
 *     0 if the records have the same AP id.
 *    -1 otherwise
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int ap_match_eth(const ap_info_t *record_a, const ap_info_t *record_b)
{
    assert(record_a != NULL && record_b !=NULL);
    if (record_a->eth_address == record_b->eth_address)
        return 0;
    return -1;
}

/* print the records in a list 
 *
 * This function provides an example on how to iterate through
 * the list using the access function.
 * 
 * Inputs:
 *     list_ptr: pointer to the list created by the construction function
 *     type_of_list: pointer to string of command entered by user
 */
void ap_print_list(twl_list_t *list_ptr, const char *type_of_list)
{
    assert(strcmp(type_of_list, "Leaderboard")==0 || strcmp(type_of_list, "Queue")==0);
    ap_info_t *rec_ptr;
    int num_in_list = twl_list_size(list_ptr); 
    int counter = 0;
    if (num_in_list == 0) { // list is empty
        printf("%s is empty\n", type_of_list);
    } else {
        printf("%s has %d records\n", type_of_list, num_in_list);
        rec_ptr = twl_list_access(list_ptr, counter);
        while (rec_ptr != NULL)
        {
            printf("%d: ", counter);
            ap_print_info(rec_ptr);
            counter++;
            rec_ptr = twl_list_access(list_ptr, counter);
        }
        assert(num_in_list == counter);
    }
    printf("\n");
}

/* This creates a list for storing AP records that are maintained
 * in sorted order based on mobile_count and eth_address.  There are 
 * no records with duplicate ethernet addresses.  
 *
 * Notice that the function pointer for the comparison
 * function is passed in as an agrument.  The comparison function is used 
 * by the sorted insert (and validate) function. 
 *
 */
twl_list_t *ap_create_leaderboard(void)
{
    return twl_list_construct(ap_rank_aps);
}

/* This function adds the AP record to the leaderboard list.  The list is
 * kept in sorted order based on the mobile_acount and eth_address.
 *
 * The order of the list should be first account for the mobile_count in
 * decreasing order.  If there are records with the same mobile_count value, 
 * these should be sorted based on the eth_address in increasing order.  
 * Note that there cannot be any ties for eth_address, because a duplicate 
 * eth_address cannot be added.
 *
 * There are two reasons a record may not be added to the list.  Check in this
 * order:
 *    first: if the list is full
 *    second: if a record already exists with the same eth_address
 *
 * Inputs:
 *     list_ptr: pointer to the list created by the construction function
 *     ap_id: the ethernet address of the AP
 *     max_list_size: the limit of how many records can be stored in the
 *                    leaderboard
 *
 * print one of three outcomes for the add operation:
 *     0: added a record in sorted order.
 *     1: did not insert because list is full.
 *     2: did not insert because eth_address already found in list (and list
 *        is not full)
 */
void ap_add(twl_list_t *list_ptr, int ap_id, int max_list_size)
{
    int add_result = -2;

    int list_count = twl_list_size(list_ptr);

    ap_info_t *rec_ptr = ap_create_info(ap_id);

    if (list_count < max_list_size){

        if (twl_list_elem_find_data_ptr(list_ptr, rec_ptr, ap_match_eth) != NULL){
            add_result = 2;
            free(rec_ptr);
            rec_ptr = NULL;
        } else {    // dont free ap_info_t block here as it is now used in the list
            twl_list_insert_sorted(list_ptr, rec_ptr);
            add_result = 0;
        }
    } else {
        add_result = 1;
        free(rec_ptr);
        rec_ptr = NULL;
    }

    // if needed use ap_create_info to malloc and fill a new memory block for new AP
    // ap_info_t *rec_ptr = ap_create_info(ap_id);
 
    // print message after determine result of operation

    if (add_result == 0) {
        printf("Inserted %d\n", ap_id);
    } else if (add_result == 1) {
        printf("Rejected %d because list is full with %d entries\n", ap_id, max_list_size);
    } else if (add_result == 2) {
        printf("Rejected %d already in list\n", ap_id);
    } else {
        printf("Error with return value! Fix your code.\n");
    }
}

/* This function prints the record with the matching eth address in the
 * leaderboard list.  

 * Inputs:
 *     list_ptr: pointer to the list created by the construction function
 *     ap_id: the ethernet address of the AP
 */
void ap_find(twl_list_t *list_ptr, int ap_id)
{
    ap_info_t *rec_ptr = NULL;
    ap_info_t *rec_ptr_result = NULL;

    rec_ptr = (ap_info_t *) calloc(1, sizeof(ap_info_t));
    rec_ptr->eth_address = ap_id;

    rec_ptr_result = twl_list_elem_find_data_ptr(list_ptr, rec_ptr, ap_match_eth);
  
    if (rec_ptr_result == NULL) {
        printf("Did not find access point with id: %d\n", ap_id);
    } else {
        // after searching list for record, verify found correct one 
        printf("%d mobiles registered with AP %d\n", rec_ptr_result->mobile_count, ap_id); 
        assert(rec_ptr_result->eth_address == ap_id);
    }
    free(rec_ptr);
    rec_ptr = NULL;
}

/* This function removes the record from the leaderboard list with the matching
 * AP ethernet address
 *
 * Inputs
 *     list_ptr: pointer to the leaderboard list 
 *     ap_id: eth_address that should be removed from the list
 */
void ap_remove(twl_list_t *list_ptr, int ap_id)
{
    ap_info_t *rec_ptr = NULL;
    
    rec_ptr = (mydata_t *) calloc(1, sizeof(mydata_t));
    rec_ptr->eth_address = ap_id;
    
    int rec_pos = twl_list_elem_find_position(list_ptr, rec_ptr, ap_match_eth);

    // free my_data_t block, only needed it to find the index position
    free(rec_ptr);
    rec_ptr = NULL;

    rec_ptr = twl_list_remove(list_ptr, rec_pos);

    if (rec_ptr == NULL) {
        printf("Remove did not find: %d\n", ap_id);
    } else {
        printf("Removed: %d\n", ap_id);
        ap_print_info(rec_ptr);
        assert(rec_ptr->eth_address == ap_id);
        free(rec_ptr);
        rec_ptr = NULL;
    }
}

/* Increment the mobile_count field for the access point record with
 * the matching eth_address.  If the record is found, print the new
 * value of mobile_count.  
 *
 * The record that is changed may need to have its position in the
 * list updated (to a lower index position or nearer the front of the list) so
 * the list is alwasy in sorted order.  The sorting order is the same as
 * defined in the ap_add function.
 *
 * Inputs
 *     list_ptr: pointer to the leaderboard 
 *     ap_id: eth_address that should have its mobile count incremented 
 */
void ap_inc(twl_list_t *list_ptr, int ap_id)
{
    int inc_result = -2;   // get the new mobile_count if found in list

    ap_info_t *rec_ptr = NULL;
    ap_info_t *rec_ptr_result = NULL;
    rec_ptr = (mydata_t *) calloc(1, sizeof(mydata_t));
    rec_ptr->eth_address = ap_id;

    rec_ptr_result = twl_list_elem_find_data_ptr(list_ptr, rec_ptr, ap_match_eth);

    if (rec_ptr_result != NULL){
        /*
            get index position of the list element, remove the ap_info_t pointer but keep
            the mydata_t pointer to all the data, increment the mobile count then add the
            ap_info_t pointer back to the list. This way we dont need a redundant sorting
            function and can just let twl_list_insert_sorted handle the sorting.
        */
        int position = twl_list_elem_find_position(list_ptr, rec_ptr_result, ap_match_eth);
        twl_list_remove(list_ptr, position);
        inc_result = ++rec_ptr_result->mobile_count;
        twl_list_insert_sorted(list_ptr, rec_ptr_result);
    }
    free(rec_ptr);
    rec_ptr = NULL;
    
    if (inc_result == -2) {
        printf("Increment failed for AP %d because not found\n", ap_id);
    } else if (inc_result > 0) {
        printf("AP %d incremented to %d\n", ap_id, inc_result);
    } else {
        printf("Increment return value %d invalid for AP %d.  Fix your code.\n", inc_result, ap_id);
    }
}

/* Decrement the mobile_count field for the access point record with
 * the matching eth_address.  If the record is found, print the new
 * value of mobile_count.  
 *
 * If the count is already zero it cannot be decremented.
 *
 * The record that is changed may need to have its position in the
 * list updated (to a larger index position or nearer the back of the list) so
 * the list is always in sorted order.  The sorting order is the same as
 * defined in the ap_add function.
 *
 * Inputs
 *     list_ptr: pointer to the leaderboard 
 *     ap_id: eth_address that should have its mobile count decremented 
 *
 */
void ap_dec(twl_list_t *list_ptr, int ap_id)
{
    int dec_result = -3;   // get the new mobile_count if found in list

    ap_info_t *rec_ptr = NULL;
    ap_info_t *rec_ptr_result = NULL;
    rec_ptr = (mydata_t *) calloc(1, sizeof(mydata_t));
    rec_ptr->eth_address = ap_id;
    
    rec_ptr_result = twl_list_elem_find_data_ptr(list_ptr, rec_ptr, ap_match_eth);
    if (rec_ptr_result != NULL){
        if (rec_ptr_result->mobile_count > 0){
            /*
                similar to the ap_inc function, we get index position of the list element, 
                remove the ap_info_t pointer but keep the mydata_t pointer to all the data, 
                decrement the mobile count then add the ap_info_t pointer back to the list.
            */
            int position = twl_list_elem_find_position(list_ptr, rec_ptr_result, ap_match_eth);
            twl_list_remove(list_ptr, position);
            dec_result = --rec_ptr_result->mobile_count;
            twl_list_insert_sorted(list_ptr, rec_ptr_result);
        } else if (rec_ptr->mobile_count == 0){ // mobile count is 0, cant be decremented
            dec_result = -1;
        }
    } else {
        dec_result = -2;
    }
    free(rec_ptr);
    rec_ptr = NULL;
    if (dec_result == -2) {
        printf("Decrement for AP %d failed because not found\n", ap_id);
    } else if (dec_result == -1) {
        printf("Decrement for AP %d failed.  Count is already zero\n", ap_id);
    } else if (dec_result >= 0) {
        printf("AP %d decremented to %d\n", ap_id, dec_result);
    } else {
        printf("Decrement return value %d invalid for AP %d.  Fix your code.\n", dec_result, ap_id);
    }
}

/* This function prints the size of both the Leaderboard and Queue lists
 *
 * Inputs
 *     list_ptr: pointer to the leaderboard list 
 *     queue: pointer to the queue list 
 *
 */
void ap_stats(twl_list_t *leaderboard, twl_list_t *queue)
{
    // get the number in list and size of the list
    int num_leaderboard = twl_list_size(leaderboard);
    int num_queue = twl_list_size(queue);
    printf("Leaderboard list records:  %d, ", num_leaderboard);
    printf("Queue list records: %d\n", num_queue);
}


/* This function removes all entries from the provided list
 *
 * Inputs
 *     list_ptr: pointer to the list we want to empty
 */
void ap_removeall(twl_list_t *leaderboard)
{
    mydata_t * data_ptr = NULL;
    int found = 0;  // how many records are removed 
    for (int i=twl_list_size(leaderboard)-1; i>=0; i--){    // iterate through the number of list entries
            data_ptr = twl_list_remove(leaderboard, 0);     // deletes the first entry each iteration
            if (data_ptr != NULL){
                free(data_ptr);
                data_ptr = NULL;
                found++;
            }
    }
    if (found == 0) {
        printf("No stations found\n");
    } else {
        printf("Removed %d station%s\n", found, found==1?"":"s");
    }
}

/* This function moves the AP record at the front of the queue and attempts
 * to insert into the leaderboard.
 *
 * Use the identical rules as for ap_add.  Don't insert if the leaderboard 
 * is full or if AP is already in the leaderboard.  If the move is rejected
 * the record is discarded (and not left in the queue). 
 *
 *  Inputs
 *     queue: pointer to the first-in first-out queue 
 *     leaderboard: pointer to the leaderboard list
 *     max_list_size: the limit of how many records can be stored in the
 *                    leaderboard
 *
 * print one of these outcomes for the move operation:
 *     0: there is no AP in queue
 *     1: moved a record from queue to leaderboard in sorted order.
 *     2: did not move because leaderboard is full.
 *     3: did not move because eth_address already found in leaderboard (and
 *        leaderboard is not full)
 *
 */
void ap_dequeue(twl_list_t *queue, twl_list_t *leaderboard, int max_list_size)
{
    int move_result = -2;
    ap_info_t *rec_ptr = NULL;

    if (twl_list_size(queue) != 0){
        rec_ptr = twl_list_access(queue, 0);   // get record from queue
    }

    if (rec_ptr == NULL){
        move_result = 0;
    } else {
        if (twl_list_size(leaderboard) >= max_list_size){   // list is full
            move_result = 2;
        } else if (twl_list_elem_find_data_ptr(leaderboard, rec_ptr, ap_match_eth)){    // data already in list
            move_result = 3;
        } else {
            twl_list_insert_sorted(leaderboard, rec_ptr);
            move_result = 1;
        }
        twl_list_remove(queue, 0);
    }
    // determine if record can be inserted into leaderboard

    if (move_result == 0) {
        printf("Queue is empty, no AP moved\n");
    } else if (move_result == 1) {
        printf("Moved %d\n", rec_ptr->eth_address);
    } else if (move_result == 2) {
        printf("Move rejected %d because leaderboard is full with %d entries\n", rec_ptr->eth_address, max_list_size);
        free(rec_ptr);
    } else if (move_result == 3) {
        printf("Move rejected %d already in leaderboard\n", rec_ptr->eth_address);
        free(rec_ptr);
    } else {
        printf("Error with return value for move! Fix your code.\n");
    }
}

/* This function adds a new AP record to the back of the queue.
 *
 * This function does NOT check for a duplicate entry and it does NOT
 * limit the size of the queue.
 *
 * Simply append to the back of the queue.
 *
 */
void ap_enqueue(twl_list_t *queue, int ap_id)
{
    // use ap_create_info to malloc and fill a new memory block for new AP
    ap_info_t *rec_ptr = ap_create_info(ap_id);
    twl_list_insert(queue, rec_ptr, TWL_LIST_BACK);
 
    // print message after determine result of operation

    printf("Appended to back of queue %d\n", ap_id);
}

/* this function frees the memory for either a sorted or unsorted list.
 */
void ap_cleanup(twl_list_t *list_ptr)
{
    twl_list_destruct(list_ptr);
}

/* Prompts user for AP record input starting with the Mobile's IP address.
 * The input is not checked for errors but will default to an acceptable value
 * if the input is incorrect or missing.
 *
 * This function creates the memory block for the record.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
ap_info_t *ap_create_info(int sta_id)
{
    char line[MAXLINE];
    char str[MAXLINE];
    char letter;
    ap_info_t *new = (ap_info_t *) calloc(1, sizeof(ap_info_t));
    assert(new != NULL);

    new->eth_address = sta_id;
    printf("AP IP address:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->ip_address);
    printf("AP location code:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->location_code);

    printf("Authenticated (T/F):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%s", str);
    if (strcmp(str, "T")==0 || strcmp(str, "t")==0)
        new->authenticated = 1;
    else
        new->authenticated = 0;

    printf("Privacy (none|WEP|WPA|WPA2):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%s", str);
    if (strcmp(str, "WEP")==0)
        new->privacy = 1;
    else if (strcmp(str, "WPA")==0)
        new->privacy = 2;
    else if (strcmp(str, "WPA2")==0)
        new->privacy = 3;
    else
        new->privacy = 0;


    printf("Standard letter (a b e g h n s):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%c", &letter);
    if (letter < 'a' || letter > 'z')
        letter = 'a';
    new->standard_letter = letter - 'a';

    printf("Band (2.4|5.0):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%f", &new->band);

    printf("Channel:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->channel);

    printf("Data rate:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%f", &new->data_rate);

    printf("Time received (int):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->time_received);
    printf("\n");

    return new;
}

/* print the information for a particular access point record 
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void ap_print_info(ap_info_t *rec)
{
    const char *pri_str[] = {"none", "WEP", "WPA", "WPA2"};
    assert(rec != NULL);
    printf("mc: %d,", rec->mobile_count);
    printf("eth: %d,", rec->eth_address);
    printf(" IP: %d, Loc: %d,", rec->ip_address, rec->location_code);
    printf(" Auth: %s,", rec->authenticated ? "T" : "F"); 
    printf(" Pri: %s, L: %c, B: %g, C: %d,", pri_str[rec->privacy], 
            (char) (rec->standard_letter + 'a'), rec->band, rec->channel);
    printf(" R: %g", rec->data_rate);
    printf(" Time: %d\n", rec->time_received);
}



/* Sorts the provided list based on mobile count of each member. 
 *
 * list_ptr: pointer to list to sort. 
 * sort_type: integer representing which sorting method to use.
 *
 */
void ap_sortap(twl_list_t *list_ptr, int sort_type){
    time_sort(list_ptr, sort_type, &ap_rank_aps);
}


/* Sorts the provided list based on ethernet address of each member. 
 *
 * list_ptr: pointer to list to sort. 
 * sort_type: integer representing which sorting method to use.
 *
 */
void ap_sorteth(twl_list_t *list_ptr, int sort_type){
    time_sort(list_ptr, sort_type, &ap_rank_eth);
}


/* Adds a member to the back of the provided list and does not check for repeating values between members. 
 *
 * list_ptr: pointer to list to append items to.
 * eth_id: ethernet address of item to append.
 * movile_cnt: mobile count of item to append
 *
 */
void ap_appendq(twl_list_t *list_ptr, int eth_id, int mobile_cnt){
    ap_info_t *new = (ap_info_t *) calloc(1, sizeof(ap_info_t));
    new->eth_address = eth_id;
    new->mobile_count = mobile_cnt;
    twl_list_insert(list_ptr, new, TWL_LIST_BACK);
}


/* Sorts a list then prints how long the sorting process took. 
 *
 * list_ptr: pointer to list to sort. 
 * sort_type: integer representing which sorting method to use.
 * fcomp: pointer to function that will be used to compare members in the sorting process
 *
 */
void time_sort(twl_list_t *list_ptr, int sort_type, int (*fcomp)(const mydata_t *, const mydata_t *)){
    clock_t start, end;
    double elapse_time; /* time in milliseconds */
    int initialcount = twl_list_size(list_ptr);
    start = clock();
    twl_list_sort(list_ptr, sort_type, fcomp);
    end = clock();
    elapse_time = 1000.0 * ((double) (end - start)) / CLOCKS_PER_SEC;
    assert(twl_list_size (list_ptr) == initialcount);
    printf("%d\t%f\t%d\n", initialcount, elapse_time, sort_type);
}


/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
