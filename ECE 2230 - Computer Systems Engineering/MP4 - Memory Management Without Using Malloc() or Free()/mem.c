/* mem.c A template
 * Michael Morris
 * Mvmorri
 * Lab4: Dynamic Memory Allocation
 * ECE 2230, Fall 2023
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>

#include "mem.h"

// Global variable required in mem.c only
static mem_header_t Mem = {0, 0, FIRST_FIT, FALSE, NULL};

// private function prototypes
void mem_validate(void);

/* function to request 1 or more pages from the operating system.
 *
 * new_bytes must be the number of bytes that are being requested from
 *           the OS with the sbrk command.  It must be an integer 
 *           multiple of the PAGESIZE
 *
 * returns a pointer to the new memory location.  If the request for
 * new memory fails this function simply returns NULL, and assumes some
 * calling function will handle the error condition.  Since the error
 * condition is catastrophic, nothing can be done but to terminate 
 * the program.
 *
 * You can update this function to match your design.  But the method
 * to test sbrk much not be changed.  
 */
mem_chunk_t *morecore(int new_bytes) 
{
    char *cp;
    mem_chunk_t *new_p;
    // preconditions that must be true for all designs
    assert(new_bytes % PAGESIZE == 0 && new_bytes > 0);
    assert(PAGESIZE % sizeof(mem_chunk_t) == 0);
    cp = sbrk(new_bytes);
    if (cp == (char *) -1)  /* no space available */
        return NULL;
    new_p = (mem_chunk_t *) cp;
    Mem.sbrk_calls++;
    Mem.pages_req += new_bytes/PAGESIZE;

    return new_p;
}

/* A function to change default operation of dynamic memory manager.
 * This function should be called before the first call to Mem_alloc.
 *
 * search_type: sets Search Policy to BEST_FIT (defults to FIRST_FIT)
 *
 * coalescing_state: sets Coalescing to TRUE (defaults to FALSE)
 *
 * Notice that Mem is a static global variable, so it not an input
 * to this (or any of the Mem_ functions).
 */
void Mem_configure(int search_type, int coalescing_state)
{
    assert(search_type == FIRST_FIT || search_type == BEST_FIT);
    assert(coalescing_state == TRUE || coalescing_state == FALSE);
    Mem.search_policy = search_type;
    Mem.coalescing = coalescing_state;
}

/* deallocates the space pointed to by return_ptr; it does nothing if
 * return_ptr is NULL.  
 *
 * This function assumes that the rover pointer has already been 
 * initialized and points to some memory block in the free list.
 */
void Mem_free(void *return_ptr)
{
    // precondition
    assert(Mem.rover != NULL && Mem.rover->next != NULL);

    return_ptr -= sizeof(mem_chunk_t);

    // two cases: if coalescing or not
    if (Mem.coalescing == FALSE){
        // insert after rover
        ((mem_chunk_t *)return_ptr)->next = Mem.rover->next;
        Mem.rover->next = return_ptr;
        Mem.rover = Mem.rover->next;
    } else {
        mem_chunk_t *temp_rover = Mem.rover;
        int block_inserted = FALSE;
        // look at addresses of free blocks and insert if return_ptr is between 2
        do{
            Mem.rover = Mem.rover->next;
            if ((Mem.rover->size_units) && ((int*)return_ptr > (int*)Mem.rover) && ((int*)return_ptr < (int*)Mem.rover->next)){
                ((mem_chunk_t *)return_ptr)->next = Mem.rover->next;
                Mem.rover->next = return_ptr;
                block_inserted = TRUE;
                break;
            }    
        } while (Mem.rover != temp_rover);
        // if previous do while loop didnt find a suitable space to put return_ptr
        if (!block_inserted){
            // go to front
            while (Mem.rover->next->size_units != 0){
                Mem.rover = Mem.rover->next;
            }
            // stick at back of list before dummy
            if ((int*)return_ptr > (int*)Mem.rover){
                ((mem_chunk_t*)return_ptr)->next = Mem.rover->next;
                Mem.rover->next = return_ptr;
            // stick at front of list after dummy
            } else {
                while (Mem.rover->size_units != 0){
                    Mem.rover = Mem.rover->next;
                }
                ((mem_chunk_t*)return_ptr)->next = Mem.rover->next;
                Mem.rover->next = return_ptr;
            }
        }

        // loop through list, nested do loops allow for corner cases
        // finds adjacent free blocks and combines
        temp_rover = Mem.rover;
        do {
            Mem.rover = Mem.rover->next;
            if ((Mem.rover->size_units) && (Mem.rover->next == (Mem.rover + Mem.rover->size_units))){
                do {
                    Mem.rover->size_units += Mem.rover->next->size_units;
                    Mem.rover->next = Mem.rover->next->next;
                } while ((Mem.rover->next == (Mem.rover + Mem.rover->size_units)));
            }
        } while (Mem.rover != temp_rover);
    }
}

/* returns a pointer to space for an object of size nbytes, or NULL if the
 * request cannot be satisfied.  The memory is uninitialized.
 *
 * This function assumes that there is a Rover pointer that points to
 * some item in the free list.  
 * 
 * Input: 
 *      nbytes: the number of bytes to allocate in a block
 * Output: 
 *      pointer to the newly allocated block
 */
void *Mem_alloc(const int nbytes)
{   
    mem_chunk_t *p;
    // precondition
    assert(nbytes > 0);

    // the first call will find Mem.rover is NULL.  This means to 
    // initialize the dummy block.  After the first call, rover can
    // never be null again.
    if (Mem.rover == NULL) {
        mem_chunk_t *dummy;
        int dummy_size = 1;
        int units_per_page = PAGESIZE / sizeof(mem_chunk_t);
        int remaining_size = units_per_page - dummy_size;
        dummy = morecore(PAGESIZE);
        p = dummy + dummy_size;
        dummy->size_units = 0;  // make zero instead of 1 so it can never be checked out
        dummy->next = p;
        p->size_units = remaining_size;
        p->next = dummy;
        Mem.rover = dummy;
    }
    assert(Mem.rover != NULL && Mem.rover->next != NULL);

    // find appropriately sized memory block

    int units_needed = (nbytes/sizeof(mem_chunk_t))+((nbytes%sizeof(mem_chunk_t))>0)+1;
    p = Mem.rover;
    int block_found = FALSE;
    mem_chunk_t *q, *temp;
    if (Mem.search_policy == FIRST_FIT){
        // search for block that is bigger or equal to needed size
        do{
            if (units_needed <= Mem.rover->size_units){
                block_found = TRUE;
                break;
            }
            Mem.rover = Mem.rover->next;
        } while (Mem.rover != p);
    } else if (Mem.search_policy == BEST_FIT){
        int min_available_units = 0x7FFFFFFF;
        int exact_size_found = FALSE;
        // cycle through all blocks, noting smallest size
        do{
            Mem.rover = Mem.rover->next;
            if ((Mem.rover->size_units < min_available_units) && (Mem.rover->size_units >= units_needed)){
                min_available_units = Mem.rover->size_units;
                block_found = TRUE;
                // break loop if exact size is found
                if (Mem.rover->size_units == units_needed){
                    exact_size_found = TRUE;
                    break;
                }
            }
        } while (Mem.rover != p);
        // circle back to a block with the smallest adaquate size
        if (block_found){
            if (!exact_size_found){
                do{
                    Mem.rover = Mem.rover->next;
                } while (Mem.rover->size_units != min_available_units);
            }
        }
    }
    // adjust the blocks size then prepare carved out block for user
    if (block_found){
        q = Mem.rover + 1;
        int remaining_units = Mem.rover->size_units - units_needed;
        Mem.rover->size_units = units_needed;

        temp = Mem.rover;
        while (temp->next != Mem.rover){
            temp = temp->next;
        }
        // determine if free list is empty and handle pointers accordingly
        if (remaining_units){
            temp->next += Mem.rover->size_units;
            temp->next->size_units = remaining_units;
            temp->next->next = Mem.rover->next;
        } else {
            temp->next = Mem.rover->next;
        }
        p = Mem.rover;
        Mem.rover = Mem.rover->next;
        p->next = NULL;
    // no block was found, allocate a new block and recursively call this fx
    } else {
        int pages_needed = ((nbytes+sizeof(mem_chunk_t)) / PAGESIZE) + ((((nbytes+sizeof(mem_chunk_t))-PAGESIZE)%PAGESIZE)>0);
        if (!pages_needed){
            pages_needed = 1;
        }   
        mem_chunk_t *new_block;
        // calculate units needed based on page size
        int units_per_page = (PAGESIZE*pages_needed) / sizeof(mem_chunk_t);
        int remaining_size = units_per_page;
        // get new page(s)
        new_block = morecore(PAGESIZE*pages_needed);

        assert(new_block != NULL);

        // adjust sizes in new block after carving out block
        new_block->size_units = remaining_size; 
        new_block++;
        
        // send new block to free list
        Mem_free(new_block);

        // call mem_alloc again, new block with appropriate size will
        // be in free list so this will work
        return Mem_alloc(nbytes);
    }

    // here are possible post-conditions, depending on your design. 
    // Include these tests before returning from this function.
    //
    // one unit is equal to sizeof(mem_chunk_t) bytes
    //
    // assume p is a pointer to the starting address of the memory block 
    // to be given to the user. The address returned to the user is q.
    //
    assert(p + 1 == q);   // +1 means one unit or sizeof(mem_chunk_t)
    //
    // Next, test that the memory block is just big enough.
    // The minus one in the next two tests accounts for the header
    //
    assert((p->size_units-1)*sizeof(mem_chunk_t) >= nbytes);
    assert((p->size_units-1)*sizeof(mem_chunk_t) <= nbytes + sizeof(mem_chunk_t));
    assert(p->next == NULL);  // saftey first!

    return q;
}

/* This function returns the number of digits in a number, used for formatting
 * the right side of the Mem_stats printout. Not the most elegant but works well.
 *
 * Input:
 *      n: an integer
 * Output: 
 *      the number of digits in n
 */
int Get_digits (int n) {
    if (n < 0) return 0;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    return 10;
}


/* prints stats about the current free list
 *
 * -- number of items in the linked list including dummy item0
 * -- min, max, and average size of each item (in bytes)
 * -- total memory in list (in bytes)
 * -- number of calls to sbrk and number of pages requested
 *
 * A message is printed if all the memory is in the free list
 */
void Mem_stats(void)
{
    // printf("the student must implement mem stats\n");
    // One of the stats you must collect is the total number
    // of pages that have been requested using sbrk.
    // Say, you call this NumPages.  You also must count M,
    // the total number of bytes found in the free list 
    // (including all bytes used for headers).  
    mem_chunk_t *temp = Mem.rover;
    float avg_bytes = 0.0;
    int min_bytes = 0xFFFFFFFF;
    int max_bytes = 0;
    int number_of_blocks = 0;

    // set this equal to sizeof(mem_chunk_t) to include the dummy block as it says it has size 0 (units)
    // but really has size 1 (units)
    int total_bytes_returned = sizeof(mem_chunk_t);
    // cycle through all free list members
    do{
        total_bytes_returned += temp->size_units*sizeof(mem_chunk_t);
        // find minimum sized member
        if ((temp->size_units != 0) && (temp->size_units*sizeof(mem_chunk_t) < min_bytes)){
            min_bytes = temp->size_units*sizeof(mem_chunk_t);
        }
        // find maximum sized member
        if (temp->size_units*sizeof(mem_chunk_t) > max_bytes){
            max_bytes = temp->size_units*sizeof(mem_chunk_t);
        }
        number_of_blocks++;
        temp = temp->next;
    } while (temp != Mem.rover);
    if (number_of_blocks > 1){
        avg_bytes = (float)(total_bytes_returned-sizeof(mem_chunk_t))/(number_of_blocks-1);
    } else {
        // if no members in list
        avg_bytes = 0.0;
        min_bytes = 0;
    }
    // print stats, need to find how many spaces from last character to right side of box
    int sp1, sp2, sp3, sp4, sp5, sp6, sp7;
    sp1 = 41-Get_digits(Mem.sbrk_calls);
    sp2 = 19-Get_digits(Mem.pages_req)-Get_digits(Mem.pages_req*PAGESIZE)-Get_digits(Mem.pages_req*PAGESIZE/sizeof(mem_chunk_t));
    sp3 = 41-Get_digits(number_of_blocks);
    sp4 = 27-Get_digits(total_bytes_returned)-Get_digits(total_bytes_returned/sizeof(mem_chunk_t));
    sp5 = 27-Get_digits(min_bytes)-Get_digits(min_bytes/sizeof(mem_chunk_t));
    sp6 = 27-Get_digits(max_bytes)-Get_digits(max_bytes/sizeof(mem_chunk_t));
    sp7 = 32-Get_digits((int)(avg_bytes));
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~ Free list stats ~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf(" Including dummy block\n");
    printf("┌────────────────────────────────────────────────────────────────┐\n");
    printf("│           Sbrk calls: %d%*s│\n", Mem.sbrk_calls, sp1, "");
    printf("│      Pages allocated: %d pages, %d bytes, %ld units%*s│\n", Mem.pages_req, Mem.pages_req*PAGESIZE, Mem.pages_req*PAGESIZE/sizeof(mem_chunk_t),sp2,"");
    printf("│       Blocks in list: %d%*s│\n", number_of_blocks, sp3, "");
    printf("│ Total memory in list: %d bytes, %ld units%*s│\n", total_bytes_returned, total_bytes_returned/sizeof(mem_chunk_t), sp4, "");
    printf("└────────────────────────────────────────────────────────────────┘\n\n");
    printf(" Not including dummy block\n");
    printf("┌────────────────────────────────────────────────────────────────┐\n");
    printf("│   Minimum block size: %d bytes, %ld units%*s│\n", min_bytes, min_bytes/sizeof(mem_chunk_t), sp5, "");
    printf("│   Maximum block size: %d bytes, %ld units%*s│\n", max_bytes, max_bytes/sizeof(mem_chunk_t), sp6, "");
    printf("│   Average block size: %0.2f bytes%*s│\n", avg_bytes, sp7, "");
    printf("└────────────────────────────────────────────────────────────────┘\n");
    
    
    if (total_bytes_returned == Mem.pages_req*PAGESIZE){
        printf("\n");
        printf("╔════════════════════════════════════════════════════════╗\n");
        printf("║   All memory is in the heap -- no leaks are possible   ║\n");
        printf("╚════════════════════════════════════════════════════════╝\n\n");
    } else {
        printf("\nMissing %d bytes (%ld units)...\n\n", Mem.pages_req*PAGESIZE-total_bytes_returned, (Mem.pages_req*PAGESIZE-total_bytes_returned)/sizeof(mem_chunk_t));
    }
    // mem_validate();
    // if (M == NumPages * PAGESIZE) then print
    // printf("all memory is in the heap -- no leaks are possible\n");
}

/* print table of memory in free list 
 *
 * The print should include the dummy item in the list 
 *
 * A unit is the size of one mem_chunk_t structure
 */
void Mem_print(void)
{
    printf("_________Current_free_list_________\n");
    // note position of Rover is not changed by this function
    assert(Mem.rover != NULL && Mem.rover->next != NULL);
    mem_chunk_t *p = Mem.rover;
    mem_chunk_t *start = p;
    printf("Total SBRK calls: %d\n", Mem.sbrk_calls);
    printf("Total pages requested: %d\n", Mem.pages_req);
    do {
        printf("p=%p\tsize=%d (units)\t%send=%p\tnext=%p %s\n", 
                p, p->size_units, p->size_units > 99 ? "": "\t",
                p + p->size_units, p->next, p->size_units!=0?"":"<-- dummy");
        p = p->next;
    } while (p != start);
    // mem_validate();
}

/* This is an experimental function to attempt to validate the free
 * list when coalescing is used.  It is not clear that these tests
 * will be appropriate for all designs.  If your design utilizes a different
 * approach, that is fine.  You do not need to use this function and you
 * are not required to write your own validate function.
 */
void mem_validate(void)
{
    // note position of Rover is not changed by this function
    assert(Mem.rover != NULL && Mem.rover->next != NULL);
    assert(Mem.rover->size_units >= 0);
    int wrapped = FALSE;
    int found_dummy = FALSE;
    mem_chunk_t *p, *largest, *smallest;

    p = Mem.rover;
    do {
        if (p->size_units == 0) {
            assert(found_dummy == FALSE);
            found_dummy = TRUE;
        } else {
            assert(p->size_units > 0);
        }
        
    } while (p != Mem.rover);
    assert(found_dummy == TRUE);

    if (Mem.coalescing) {
        do {
            if (p >= p->next) {
                // this is not good unless at the one wrap around
                if (wrapped == TRUE) {
                    printf("validate: List is out of order, already found wrap\n");
                    printf("first largest %p, smallest %p\n", largest, smallest);
                    printf("second largest %p, smallest %p\n", p, p->next);
                    assert(wrapped == FALSE);   // stop and use gdb
                } else {
                    wrapped = TRUE;
                    largest = p;
                    smallest = p->next;
                }
            } else {
                assert(p + p->size_units < p->next);
            }
            p = p->next;
        } while (p != Mem.rover);
        assert(wrapped == TRUE);
    }
}
/* vi:set ts=8 sts=4 sw=4 et: */

