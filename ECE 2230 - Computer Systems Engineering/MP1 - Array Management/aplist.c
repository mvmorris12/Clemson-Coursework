/* aplist.c
 * Michael Morris
 * mvmorri
 * ECE 2230-001 Fall 2023
 * MP1
 *
 * Purpose: These functions serve as the driving force for lab1.c. The
 *      functions here will create and destroy a list used to hold data from 
 *      access points. These functions also are used to maintain the list's entries
 *      by finding entries based on ethernet address, updating mobile count, and
 *      sorting the list by decreasing mobile count and increasing ethernet address.
 *
 * Assumptions: The ONLY members can be accessed in an ap_info_t memory block are 
 *      eth_address and mobile_count. All other members in this structure cannot be used
 *      in this file. 
 *
 * Bugs: None other than mentioned in lab1.c
 *
 */

#include <stdlib.h>
#include "aplist.h"

// Prototypes for additional private functions in aplist.c 
static int aplist_get_index(struct ap_list_t *list_ptr, int id);
static void aplist_sort(struct ap_list_t *list_ptr);



/* Search for a record in the list that has an eth_address that matches
 * the input id.  If found, the array index of that ap_info_t memory 
 * block is returned (the item is not removed and the list is not changed).
 *
 * If a match is not found, the return is -1 as 0 is a viable index.
 *
 * This function does not change the list
 *
 * Inputs
 *     list_ptr: pointer to the list created by the construction function
 *     id: eth_address to search for
 *
 * Output:
 *     integer index value for the matching record found in the list.  
 *     -1 if a match is not found.
 */
static int aplist_get_index(struct ap_list_t *list_ptr, int id)
{
    // ensure the list and pointer array are populated with at least 1 element
    if (list_ptr->apl_entries){
        for (int i=0; i<list_ptr->apl_entries; i++){
            // check if the list entry has a matching eth_address
            if (list_ptr->apl_ptr[i]->eth_address == id){
                return i;
            }
        }
    }
    // return this if no match was found
    return -1;
}


/* Sort an array of ap_info_t pointers based on MP1 specifications:
 * Initially in order of decreasing mobile_count.
 * Then in order of increasing eth_address if mobile_count values
 * between 2 entries are equal.
 *
 * This function may change the order of the elements in the list, but 
 * will not change the list if its elements are already ordered correctly.
 *
 * Inputs
 *     list_ptr: pointer to the list created by the construction function.
 *
 * Output:
 *     None
 */
static void aplist_sort(struct ap_list_t *list_ptr)
{
    // determine if the list is able to be sorted, if 0 or 1 entries exist the list cannot be sorted
    if (list_ptr->apl_entries > 1){
        // compare each entry to the next in the list, works from bottom to top
        for (int i=list_ptr->apl_entries-1; i>0; i--) {

            // checks if the current element's mobile_count is greater than the next element
            // if it is not, then..
            // checks if the 2 elements have matching mobile_count then compares both eth_addresses
            if ((list_ptr->apl_ptr[i]->mobile_count > list_ptr->apl_ptr[i-1]->mobile_count) || 
            ((list_ptr->apl_ptr[i]->mobile_count == list_ptr->apl_ptr[i-1]->mobile_count) &&
            (list_ptr->apl_ptr[i]->eth_address < list_ptr->apl_ptr[i-1]->eth_address))){
                // set a temp variable to assist in switching the 2 pointers
                struct ap_info_t * temp_apl_ptr = list_ptr->apl_ptr[i];
                // set second pointer to the first pointer
                list_ptr->apl_ptr[i] = list_ptr->apl_ptr[i-1];
                // set set first pointer to the second (stored in temp variable)
                list_ptr->apl_ptr[i-1] = temp_apl_ptr;
                // Recursively call this function (aplist_sort) for instances where there are multiple
                // entries with the same mobile count. Without this, the sort function only moves the entry 
                // one position. This allows the function to move entries with multiple matching mobile counts
                // to the correct position. This is not efficient, but it works and is simple to implement.
                aplist_sort(list_ptr);
            }
        }
    }
}


/* constructs a data structure by mallocing a ap_list_t header and an array
 * of ap_info_t pointers.  The ap_info_t pointers must be initalized to null.
 *
 * size: The size of the array when constructed.  
 *
 * returns pointer to the ap_list_t memory block
 */
struct ap_list_t *aplist_construct(int size)
{
    struct ap_list_t *list_ptr = NULL;
    int i = 0;

    // allocate space for the ap_list_t data inside list_ptr
    list_ptr = (struct ap_list_t *) malloc(sizeof(struct ap_list_t));
    list_ptr->apl_array_size = size;
    list_ptr->apl_entries = 0;
    list_ptr->apl_ptr = NULL;

    // allocate space for pointers (to various list elements)
    list_ptr->apl_ptr = (struct ap_info_t **) malloc(size * sizeof(struct ap_info_t *));

    // initialize all list pointers to NULL
    for (i=0; i < size; i++){
	    list_ptr->apl_ptr[i] = NULL;
    }

    return list_ptr;



    // return NULL;
}

/* frees all ap_info_t records, then frees the array of ap_info_t pointers,
 * then frees the ap_list_t header memory block.  This ensures that all 
 * memory that is currently stored in the list is returned to the dynamic
 * memory manager.
 *
 * ptr: pointer to a list that was created by the construct function
 */
void aplist_destruct(struct ap_list_t *ptr)
{
    int i;
    if (ptr->apl_array_size){
        for (i=0; i < ptr->apl_entries-1; i++){
            // clear all individual list records
            free(ptr->apl_ptr[i]);
            ptr->apl_ptr[i] = NULL;
        }
    }
    // clear pointer that points to array of pointers
    free(ptr->apl_ptr);
    ptr->apl_ptr = NULL;
    // clear list structure variable
    free(ptr);
    ptr = NULL;
    
}

/* inserts a ap_info_t memory block into the list.  The insertion into the
 * list maintains the list in sorted order as defined below.  The list must
 * be sequential with no empty gaps between entries in the list.  The record
 * with the largest mobile count should be found at index position 0, the next
 * largest in order at index position 1, etc.  Be careful to handle ties 
 * correctly.
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
 *     rec_ptr: pointer to an existing memory block containing the record for
 *              an access point.
 *
 * Return values:
 *     0: added a record in sorted order.
 *     1: did not insert because list is full.
 *     2: did not insert because eth_address already found in list (and list
 *        is not full)
 */
int aplist_add(struct ap_list_t *list_ptr, struct ap_info_t *rec_ptr)
{
    int i;
    i = list_ptr->apl_entries;
    // check if list is full
    if (i < list_ptr->apl_array_size){
        // check if eth_address already exists in list (list already determined to not be full)
        if (i && aplist_find(list_ptr, rec_ptr->eth_address)){            
            return 2;
        }
        // allocate space for new list entry somewhere in memory and assign that address to array
        list_ptr->apl_ptr[i] = (struct ap_info_t *) malloc(sizeof(struct ap_info_t));
        // copy new data into array-linked list
        *list_ptr->apl_ptr[i] = *rec_ptr;
        // increment the list entry count
        list_ptr->apl_entries++;  

        // sort list of pointers
        aplist_sort(list_ptr);
        return 0;      
    } else {
        // return this if list is full
        return 1;
    }
}

/* Search for a record in the list that has an eth_address that matches
 * the input id.  If found, an alias to the ap_info_t memory block is
 * returned (the item is not removed and the list is not changed).
 *
 * If a match is not found, the return is null.
 *
 * This function does not change the list
 *
 * Inputs
 *     list_ptr: pointer to the list created by the construction function
 *     id: eth_address to search for
 *
 * Output:
 *     alias pointer to the matching record found in the list.  Null if a
 *     match is not found.
 */
struct ap_info_t *aplist_find(struct ap_list_t *list_ptr, int id)
{
    for (int i=0; i<list_ptr->apl_entries; i++){
        if (list_ptr->apl_ptr[i]->eth_address == id){
            // return the address of the memory block if eth_address is found
            return list_ptr->apl_ptr[i];
        }
    }
    // return this if no similar eth_address in the list
    return NULL;
}

/* Allows a record at a specific index position to be accessed 
 *
 * This function does not change the list
 *
 * Inputs:
 *     list_ptr: pointer to the list created by the construction function
 *     index: array position that is requested
 *
 * Output:
 *     alias pointer to the matching record at the index
 *
 *     if the list does not exist, the index is out of bounds, or no 
 *     record is found at the index position, then null is returned.
 *
 */
struct ap_info_t *aplist_access(struct ap_list_t *list_ptr, int index)
{
    return list_ptr->apl_ptr[index];
}

/* look for and remove a record in the list with an eth_address that matches
 * the given id.  After removing the record, the resulting list is still
 * sequential with no gaps between entries in the list.  If no matching
 * record is found, null is returned.
 *
 * Inputs
 *     list_ptr: pointer to the list created by the construction function
 *     id: eth_address that should be removed from the list. If this is 
 *          -1, remove all the entries.
 *
 * Output
 *     pointer to the record that is removed from the list.  null if no 
 *     matching record is found, or the list does not exist. 
 */
struct ap_info_t *aplist_remove(struct ap_list_t *list_ptr, int id)
{
    // determine if the eth_address in question is actually in the list
    if (aplist_find(list_ptr, id)){
        // determine where in the array the match exists at
        int idx = aplist_get_index(list_ptr, id);
        if (idx != -1){
            // copy alias pointer to return to whatever called aplist_remove
            struct ap_info_t * ret_apl_ptr = list_ptr->apl_ptr[idx];
            // move all entries after the removed entry up (closer to the 0th position) by one position
            for (int j=idx; j<list_ptr->apl_entries-1; j++){
                list_ptr->apl_ptr[j] = list_ptr->apl_ptr[j+1];
            }

            // decrement the count of entries in the list
            list_ptr->apl_entries--;
            return ret_apl_ptr;
        }
    // if id == -1, remove all entries in list
    } else if (id == -1){
        int i = list_ptr->apl_entries;
        if (i){
            for (int j=0; j<i; j++){
                // free memory occupied by each element then clear dangling pointer
                free(list_ptr->apl_ptr[j]);
                list_ptr->apl_ptr[j] = NULL;
            }
            // set number of active entries to 0
            list_ptr->apl_entries = 0;
        }
    }
    return NULL;
}

/* increment the mobile_count field for the access point record with
 * the matching eth_address.  If the record is found, return the new
 * value of mobile_count.  If the record is not found, then return -2.
 *
 * The record that is changed may need to have its position in the
 * list updated (to a lower index position or nearer the front of the list) so
 * the list is alwasy in sorted order.  The sorting order is the same as
 * defined in the aplist_add function.
 *
 * Inputs
 *     list_ptr: pointer to the list created by the construction function
 *     id: eth_address that should have its mobile count incremented 
 *
 * Output
 *     The new mobile count value or -2 if the eth_address is not found.
 */
int aplist_inc(struct ap_list_t *list_ptr, int id)
{
    // determine if the eth_address exists in the list
    struct ap_info_t *search_ptr_ret = aplist_find(list_ptr, id);
    if (search_ptr_ret){
        // increment the mobile_count of that list element
        int updated_mobile_count = ++(search_ptr_ret->mobile_count);
        // sort the list as it has changed and may need to be resorted
        aplist_sort(list_ptr);
        return updated_mobile_count;
    }
    // return this if element with searched eth_address is not found
    return -2;
}

/* decrement the mobile_count field for the access point record with
 * the matching eth_address.  If the record is found, return the new
 * value of mobile_count.  If the record is not found, then return -2.
 * if the count is already zero it cannot be decremented so return -1.
 *
 * The record that is changed may need to have its position in the
 * list updated (to a larger index position or nearer the back of the list) so
 * the list is alwasy in sorted order.  The sorting order is the same as
 * defined in the aplist_add function.
 *
 * Inputs
 *     list_ptr: pointer to the list created by the construction function
 *     id: eth_address that should have its mobile count decremented 
 *
 * Output
 *     The new mobile count value, or 
 *     -1 if the value is already zero and cannot be decremented, or
 *     -2 if the eth_address is not found.
 */
int aplist_dec(struct ap_list_t *list_ptr, int id)
{
    // determine if the eth_address exists in the list
    struct ap_info_t *search_ptr_ret = aplist_find(list_ptr, id);
    if (search_ptr_ret){
        // determine if the current mobile_count is greater than 0
        if (search_ptr_ret->mobile_count){
            // decrement the mobile count
            int updated_mobile_count = --search_ptr_ret->mobile_count;
            // sort the list as it has changed and may need to be resorted
            aplist_sort(list_ptr);
            return updated_mobile_count; 
        } else {
            // return this if the mobile_count is 0 (no need to decrement)
            return -1;
        }
    }
    // return this if element with searched eth_address is not found
    return -2;
}

/* returns the number of records currently stored in the array
 *
 * Input:
 *     list_ptr: pointer to the list created by the construction function
 *
 * Output:
 *     Integer of the number of entries in the list
 */
int aplist_number_entries(struct ap_list_t *list_ptr)
{
    return list_ptr->apl_entries;
}