/* twl_list.c              <<-- A template for two-way linked list module 
 * Michael Morris
 * Mvmorri
 * ECE 2230 Fall 2023
 * MP2
 *
 * Propose: This file manages the sorted / unsorted lists for the main lab2.c file.
 *          It is unable to access the data within each list member and therefore 
 *          requires comparison functions within the lists themselves to properly
 *          maintain the lists. 
 *
 * Assumptions: The functions within this file should check for corner cases / out
 *          of bounds operations on the lists, but proper requests should be made
 *          by the user when possible. 
 *
 * Bugs: None found, though exhaustive testing may reveal some.
 */

#include <stdlib.h>
#include <assert.h>

#include "twl_list.h"   // defines public functions for two-way linked list ADT

#define TRUE  1
#define FALSE 0

// prototypes for private functions used in twl_list.c only
void list_debug_validate(twl_list_t *L);

/* ----- below are the functions  ----- */

/* Allocates a new, empty list 
 *
 * If the comparison function is NULL, then the list is unsorted.
 *
 * Otherwise, the list is initially assumed to be sorted.  Note that if 
 * twl_list_insert is used the list is changed to unsorted.  
 *
 * Use linked_destruct to remove and deallocate all elements on a list 
 * and the header block.
 *
 * (This function is written and no changes needed. It provides an example
 *  of how save the comparison function pointer.  See other examples in this
 *  file.)
 */
twl_list_t * twl_list_construct(int (*compare_function)(const mydata_t *, const mydata_t *))
{
    twl_list_t *L;

    L = (twl_list_t *) malloc(sizeof(twl_list_t));
    L->ll_front = NULL;
    L->ll_back = NULL;
    L->ll_count = 0;
    L->ll_comp_function = compare_function;
    if (compare_function == NULL)
        L->ll_is_sorted = FALSE;
    else
        L->ll_is_sorted = TRUE;

    /* the last line of this function must call validate */
    list_debug_validate(L);
    return L;
}

/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 *
 * Free all elements in the list and the header block.
 * 
 * list_ptr: pointer to list-of-interest. 
 */
void twl_list_destruct(twl_list_t *list_ptr)
{
    /* the first line must validate the list */
    list_debug_validate(list_ptr);

    ll_node_t *rover = NULL;
    while (list_ptr->ll_front != NULL){
        rover = list_ptr->ll_front->next;
        free(list_ptr->ll_front->data_ptr);
        list_ptr->ll_front->data_ptr = NULL;
        free(list_ptr->ll_front);
        list_ptr->ll_front = rover;
    }
    free(list_ptr);
    list_ptr = NULL;
}

/* Obtains a pointer to an element stored in the specified list, at the
 * specified list position
 * 
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from twl_list_construct.
 *
 * pos_index: position of the element to be accessed.  Index starts at 0 at
 *            front of the list, and incremented by one until the back is
 *            reached.  Can also specify TWL_LIST_FRONT and TWL_LIST_BACK
 *
 * return value: pointer to the mydata_t element accessed in the list at the
 * index position.  A value NULL is returned if the pos_index does not 
 * correspond to an element in the list.
 */
mydata_t * twl_list_access(twl_list_t *list_ptr, int pos_index)
{
    assert(list_ptr != NULL);
    /* debugging function to verify that the structure of the list is valid */
    list_debug_validate(list_ptr);

    /* handle special cases.
     *   1.  The list is empty
     *   2.  Asking for the front 
     *   3.  Asking for the back 
     *   4.  specifying a position that is out of range.  This is not defined
     *       to be an error in this function, but instead it is assumed the 
     *       calling function is looping to find an end of the list 
     */
    // the list is empty
    if (list_ptr->ll_count == 0) {
        return NULL;
    }
    // the position of interest is the front
    else if (pos_index == TWL_LIST_FRONT || pos_index == 0) {
        return list_ptr->ll_front->data_ptr;
    }
    // the position of interest is the back
    else if (pos_index == TWL_LIST_BACK || pos_index == list_ptr->ll_count - 1) {
        return list_ptr->ll_back->data_ptr;
    }
    // position of interest is outside the bounds of the list
    else if (pos_index < 0 || pos_index >= list_ptr->ll_count)
        return NULL;   // does not correspond to position in list

    ll_node_t *rover = NULL;
    rover = list_ptr->ll_front;

    int count = 0;
    while ((rover != NULL) && (count < pos_index)){
        rover = rover->next;
        count++;
    }

    return rover->data_ptr;   // you have to fix the return value
}

/* Finds an element in a list and returns a pointer to the mydata_t memory
 * block.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: element against which other elements in the list are compared.
 *
 * compare_function: function to test if elem_ptr is a match to an item in the list.
 *        Returns 0 if they match and any value not equal to 0 if they do not match.
 *
 * The function returns a pointer to the mydata_t memory block for the ll_node_t 
 * that contains the first matching element if a match if found.  If a match is 
 * not found the return value is NULL.
 */
mydata_t * twl_list_elem_find_data_ptr(twl_list_t *list_ptr, mydata_t *elem_ptr,
        int (*compare_function)(const mydata_t *, const mydata_t *))
{
    list_debug_validate(list_ptr);

    ll_node_t *rover = NULL;
    rover = list_ptr->ll_front;
    int count = 0;
    // pass rover through the list and see if the compare function shows a match
    while(rover != NULL){
        if((rover->data_ptr != NULL) && (compare_function(rover->data_ptr, elem_ptr) == 0)){
            return rover->data_ptr;
        }
        count++;
        rover = rover->next;
    }

    return NULL;
}


/* Similar to twl_list_elem_find_data_ptr, this function finds an element in 
 * a list.  However, the return value is the integer position of the matching 
 * element with the lowest index.  If a matching element is not found, the 
 * position index that is returned should be -1. 
 *
 */
int twl_list_elem_find_position(twl_list_t *list_ptr, mydata_t *elem_ptr,
        int (*compare_function)(const mydata_t *, const mydata_t *))
{
    list_debug_validate(list_ptr);

    ll_node_t *rover = NULL;
    rover = list_ptr->ll_front;
    int count = 0;

    // pass rover through the list and see if the compare function shows a match
    while(rover != NULL){
        if((rover->data_ptr != NULL) && (compare_function(rover->data_ptr, elem_ptr) == 0)){
            return count;
        }
        count++;
        rover = rover->next;
    }

    return -1;
}

/* Inserts the data element into the specified list at the specified
 * position.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the memory block to be inserted into list.
 *
 * pos_index: numeric position index of the element to be inserted into the 
 *            list.  Index starts at 0 at front of the list, and incremented by 
 *            one until the back is reached.  The index can also be equal
 *            to TWL_LIST_FRONT or TWL_LIST_BACK (these are special negative 
 *            values use to provide a short cut for adding to the front 
 *            or back of the list).
 *
 * If pos_index is greater than the number of elements currently in the list, 
 * the element is simply appended to the end of the list (no additional elements
 * are inserted).
 *-
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the list_ptr->ll_is_sorted must be equal 
 * to FALSE.
 */
void twl_list_insert(twl_list_t *list_ptr, mydata_t *elem_ptr, int pos_index)
{
    assert(list_ptr != NULL);

    ll_node_t *new_node = NULL;
    new_node = (ll_node_t *) malloc(sizeof(ll_node_t));
    new_node->data_ptr = elem_ptr;
    // adjust list front / back pointers if the list is empty
    if (list_ptr->ll_count == 0){
        list_ptr->ll_front = new_node;   
        list_ptr->ll_front->prev = NULL;
        list_ptr->ll_back = new_node;
        list_ptr->ll_back->next = NULL;
        list_ptr->ll_count++;
    } else {
        // adjust list front / back pointers if the new entry is inserted at the front
        if (pos_index == 0 || pos_index == TWL_LIST_FRONT){
            new_node->next = list_ptr->ll_front;
            list_ptr->ll_front->prev = new_node;
            new_node->prev = NULL;
            list_ptr->ll_front = new_node;
            list_ptr->ll_count++;
        } else if ((pos_index == TWL_LIST_BACK) || (pos_index > list_ptr->ll_count)){
            // adjust list front / back pointers if the new entry is inserted at the back
            list_ptr->ll_back->next = new_node;
            new_node->prev = list_ptr->ll_back;
            list_ptr->ll_back = new_node;
            new_node->next = NULL;
            list_ptr->ll_count++;
        } else {
            new_node->next = list_ptr->ll_front;
            // iterate through the list until the desired position is reached
            for (int i = 0; i<pos_index; i++){
                new_node->prev = new_node->next;
                new_node->next = new_node->next->next;                
            }
            new_node->prev->next = new_node;
            new_node->next->prev = new_node;
            list_ptr->ll_count++;
        }
    }

    /* the last two lines of this function must be the following */
    if (list_ptr->ll_is_sorted == TRUE) 
	    list_ptr->ll_is_sorted = FALSE;
    list_debug_validate(list_ptr);
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the ll_comp_function.  This function is defined in the header:
 *     list_ptr->ll_comp_function(A, B)
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type mydata_t.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the front of the list than B
 *   -1: B should be closer to the front of the list than A
 *    0: A and B are equal in rank
 *
 * If the element to be inserted is equal in rank to an element already
 * in the list, the newly inserted element will be placed _after all_ the
 * elements of equal rank that are already in the list.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use twl_list_insert_sorted, the list preserves its sorted nature.
 *
 * If you use twl_list_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call twl_list_insert_sorted, this subroutine
 * must generate a system error and the program should immediately stop.
 *
 */
void twl_list_insert_sorted(twl_list_t *list_ptr, mydata_t *elem_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->ll_is_sorted == TRUE);

    // step 1: find the index pointer location!
    // step 2: insert in front of index (or at back if no position found).

    ll_node_t *new_node = NULL;
    new_node = (ll_node_t *) malloc(sizeof(ll_node_t));
    new_node->data_ptr = elem_ptr;
    // if list is empty..
    if (list_ptr->ll_count == 0){
        list_ptr->ll_front = new_node;   
        list_ptr->ll_front->prev = NULL;
        list_ptr->ll_back = new_node;
        list_ptr->ll_back->next = NULL;
        list_ptr->ll_count++;
    } else {
        ll_node_t *rover = NULL;
        rover = list_ptr->ll_front;
        int count = 0;
        int ret_val = 0;
        // pass rover through the list
        while (rover != NULL){
            ret_val = list_ptr->ll_comp_function(rover->data_ptr, elem_ptr);
            // item being compared to the list needs to be inserted towards the front of the list, relatively
            if (ret_val == -1){
                // item needs to be inserted at front of list
                if(count == 0){
                    new_node->next = rover;
                    rover->prev = new_node;
                    new_node->prev = NULL;
                    list_ptr->ll_front = new_node;
                // item needs to be inserted somewhere in the middle of the list
                } else if (count < (list_ptr->ll_count)){
                    new_node->next = rover;
                    new_node->prev = rover->prev;
                    rover->prev->next = new_node;
                    rover->prev = new_node;
                // item needs to be inserted at the end of the list
                } else {
                    new_node->next = NULL;
                    new_node->prev = rover;
                    rover->next = new_node;
                    list_ptr->ll_back = new_node;                    
                }
                list_ptr->ll_count++;
                break;
            // item should be after the current member of the list, need to keep going to find the correct spot
            } else if (ret_val == 1){
                // pass rover until the item in question should go before current member of the list
                if (count == list_ptr->ll_count-1){
                    new_node->next = NULL;
                    new_node->prev = list_ptr->ll_back;
                    list_ptr->ll_back->next = new_node;
                    list_ptr->ll_back = new_node;
                    list_ptr->ll_count++;
                    break;
                // pass rover through the list
                } else {
                    rover = rover->next;
                }
            // item has equal element of current list member
            } else if (ret_val == 0){
                // pass rover through the list
                while((rover != NULL) && (list_ptr->ll_comp_function(rover->data_ptr, elem_ptr) == 0)){
                    rover = rover->next;
                }
                // when ^ is done, insert the new node
                new_node->next = rover;
                new_node->prev = rover->prev;
                rover->prev->next = new_node;
                if (rover->next != NULL){
                    rover->next->prev = new_node;
                }
                
            } else {

            }
            count++;
        }
    }     

    /* the last line of this function must be the following */
    list_debug_validate(list_ptr);
}

/* Removes the element from the specified list that is found at the 
 * specified list position.  A pointer to the data element is returned.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * pos_index: position of the element to be removed.  Index starts at 0 at
 *            front of the list, and incremented by one until the back is
 *            reached.  Can also specify TWL_LIST_FRONT and TWL_LIST_BACK
 *
 * Attempting to remove an element at a position index that is not contained in
 * the list will result in no element being removed, and a NULL pointer will be
 * returned.
 */
mydata_t * twl_list_remove(twl_list_t *list_ptr, int pos_index)
{
    assert(list_ptr != NULL);

    // check that the pos_index is a valid index value
    if (((pos_index < 0) && (pos_index != TWL_LIST_BACK) && (pos_index != TWL_LIST_FRONT)) || 
        (pos_index > twl_list_size(list_ptr)-1) || twl_list_size(list_ptr) == 0){
        return NULL;
    } 

    ll_node_t *rover = NULL;
    rover = list_ptr->ll_front;

    mydata_t *ret_ptr = NULL;

    // check if first list member is selected to be removed
    if ((pos_index == 0) || (pos_index == TWL_LIST_FRONT)){
        if (twl_list_size(list_ptr) > 1){
            list_ptr->ll_front = rover->next;
            rover->next->prev = NULL;
        } else {
            list_ptr->ll_front = NULL;
            list_ptr->ll_back = NULL;
        }
        ret_ptr = rover->data_ptr;
    // check if last list member is selected to be removed
    } else if ((pos_index == twl_list_size(list_ptr)-1) || (pos_index == TWL_LIST_BACK)){
        if (twl_list_size(list_ptr) > 1){
            rover = list_ptr->ll_back;
            list_ptr->ll_back = rover->prev;
            rover->prev->next = NULL;
        } else {
            list_ptr->ll_front = NULL;
            list_ptr->ll_back = NULL;
        }
    // check if a middle (i.e. not first or last) member is selected to be removed
    } else {
        for (int i=0; i<pos_index; i++){
            rover = rover->next;
        }
        rover->prev->next = rover->next;
        rover->next->prev = rover->prev;
    }
    ret_ptr = rover->data_ptr;
    free(rover);
    rover = NULL;
    list_ptr->ll_count--;

    /* the last line should verify the list is valid after the remove */
    list_debug_validate(list_ptr);
    return ret_ptr;
}

/* Obtains the length of the specified list, that is, the number of elements
 * that the list contains.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns an integer equal to the number of elements stored in the list.  An
 * empty list has a size of zero.
 *
 * (This function is already written, so no changes necessary.)
 */
int twl_list_size(twl_list_t *list_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->ll_count >= 0);
    return list_ptr->ll_count;
}


/* This function verifies that the pointers for the two-way linked list are
 * valid, and that the list size matches the number of items in the list.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 * The function produces no output if the two-way linked list is correct.  It
 * causes the program to terminate and print a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 */
void list_debug_validate(twl_list_t *L)
{
    ll_node_t *N;
    int count = 0;
    assert(NULL != L); 
    if (NULL == L->ll_back) 
	assert(NULL == L->ll_front && 0 == L->ll_count);
    else
	assert(NULL == L->ll_back->next);
    if (L->ll_front != NULL) 
	assert(L->ll_front->prev == NULL);
    else
	assert(L->ll_back == NULL && L->ll_count == 0);
    if (0 == L->ll_count) assert(NULL == L->ll_front && NULL == L->ll_back);
    if (L->ll_front == L->ll_back && L->ll_back != NULL) assert(L->ll_count == 1);
    if (1 == L->ll_count) {
        assert(L->ll_front == L->ll_back && L->ll_back != NULL);
        assert(NULL == L->ll_back->next && NULL == L->ll_back->prev);
        assert(NULL != L->ll_back->data_ptr);
    }
    assert(L->ll_is_sorted == TRUE || L->ll_is_sorted == FALSE);
    if (1 < L->ll_count) {
        assert(L->ll_front != L->ll_back && NULL != L->ll_front && NULL != L->ll_back);
        N = L->ll_back;
        while (N != NULL) {
            assert(NULL != N->data_ptr);
            if (NULL != N->prev) assert(N->prev->next == N);
            else assert(N == L->ll_front);
            count++;
            N = N->prev;
        }
        assert(count == L->ll_count);
    }
    if (L->ll_is_sorted && NULL != L->ll_front) {
        N = L->ll_front;
        while (N->next != NULL) {
            assert(L->ll_comp_function(N->data_ptr, N->next->data_ptr) != -1);
            N = N->next;
        }
    }
}
/* commands for vim. ts: tabstop, sts: softtabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */

