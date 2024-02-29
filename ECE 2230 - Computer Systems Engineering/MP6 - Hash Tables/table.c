/* table.c 
 * Michael Morris
 * mvmorri
 * Lab6: Hash Tables 
 * ECE 2230-1, Fall 2023
 *
 * 
 * This file contains driving functions for lab6.c. All functionality is requested by 
 * the various functions within lab6.c
 * 
 * Bugs: verbose mode causes some strange memory errors within Valgrind
 *          that are caused by printf when attempting to use large tables.  
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "table.h"
#include "hashes.h"
#include <assert.h>


/* Simple function to return table address based on hash function.
 *
 * Pick a hash funcion X_hash() from the hashes.c file.  All hash functions
 * return an unsigned int.  Then mod with table size.  Return signed result.  
 * See MP6.pdf for example hash functions.  For example:
 *
 *     return djb_hash(K) % tablesize;
 */
int table_hash(hashkey_t K, int tablesize){
    return djb_hash(K) % tablesize;
    // return djb_hash_xor(K) % tablesize;
    // return sax_hash(K) % tablesize;
    // return fnv_hash(K) % tablesize;
    // return oat_hash(K) % tablesize;
    // return jsw_hash(K) % tablesize;
    // return elf_hash(K) % tablesize;
    // return jen_hash(K) % tablesize;
}

/* Function to calculate the probe decrement for double hashing only.  You
 * can assume this function is only called when the probe type is double.
 *
 * Pick a different hash function X_hash() than used in table_hash.  After taking
 * the mod to get a decrement value, make sure the decrement is at least 1.
 * For example:
 *
 *      int prob_dec = djb_hash_xor(K) % tablesize;
 *      if (prob_dec < 1) prob_dec = 1;
 *      return prob_dec;
 */
int table_probe_dec(hashkey_t K, int tablesize){
    // int prob_dec =  djb_hash(K) % tablesize;
    int prob_dec =  djb_hash_xor(K) % tablesize;
    // int prob_dec =  sax_hash(K) % tablesize;
    // int prob_dec =  fnv_hash(K) % tablesize;
    // int prob_dec =  oat_hash(K) % tablesize;
    // int prob_dec =  jsw_hash(K) % tablesize;
    // int prob_dec =  elf_hash(K) % tablesize;
    // int prob_dec =  jen_hash(K) % tablesize;
    if (prob_dec < 1) prob_dec = 1;
    return prob_dec;
}

/*  The empty table is created.  The table must be dynamically allocated and
 *  have a total size of table_size.  The maximum number of (K, I) entries
 *  that can be stored in the table is table_size-1.  For open addressing, 
 *  the table is filled with a special empty key distinct from all other 
 *  nonempty keys (e.g., NULL).  
 *
 *  the probe_type must be one of {LINEAR, DOUBLE, CHAIN}
 *
 *  Do not "correct" the table_size or probe decrement if there is a chance
 *  that the combinaion of table size or probe decrement will not cover
 *  all entries in the table.  Instead we will experiment to determine under
 *  what conditions an incorrect choice of table size and probe decrement
 *  results in poor performance.
 */
table_t *table_construct(int table_size, int probe_type){
    table_t* new_table = (table_t*)malloc(sizeof(table_t));
    new_table->type_of_probing_used_for_this_table = probe_type;
    new_table->table_size_M = table_size;
    new_table->num_probes_for_most_recent_call = 0;
    new_table->num_keys_stored_in_table = 0;
    if (table_size>0){
        if (probe_type == LINEAR || probe_type == DOUBLE){
            table_entry_t* new_entry = (table_entry_t*)malloc(sizeof(table_entry_t)*table_size);
            new_table->oa = new_entry;
            new_table->sc = NULL;
            for (int i=0; i<table_size; i++){
                new_table->oa[i].data_ptr = NULL;
                new_table->oa[i].deleted = 0;
                new_table->oa[i].key = NULL;
            }
        } else if (probe_type == CHAIN){
            sep_chain_t** new_entry_p = (sep_chain_t**)malloc(sizeof(sep_chain_t*)*table_size);
            for (int i=0; i<table_size; i++){
                new_entry_p[i] = NULL;
            }
            new_table->sc = new_entry_p;
            new_table->oa = NULL;
        }
    }
    return new_table;
}

/* Sequentially remove each table entry (K, I) and insert into a new
 * empty table with size new_table_size.  Free the memory for the old table
 * and return the pointer to the new table.  The probe type should remain
 * the same.
 *
 * Do not rehash the table during an insert or delete function call.  Instead
 * use drivers to verify under what conditions rehashing is required, and
 * call the rehash function in the driver to show how the performance
 * can be improved.
 */
table_t *table_rehash(table_t *T, int new_table_size){
    table_t *new_table = table_construct(new_table_size, T->type_of_probing_used_for_this_table);
    if (T->type_of_probing_used_for_this_table != CHAIN){
        for (int i=0; i<T->table_size_M; i++){
            if (T->oa[i].deleted == 0 && T->oa[i].key != NULL){
                table_insert(new_table, strdup(T->oa[i].key), T->oa[i].data_ptr);
            }
        }
        for (int i=0; i<T->table_size_M; i++){
            if (T->oa[i].key != NULL){
                free(T->oa[i].key);
            }
        }
        free(T->oa);
        free(T);

    } else {
        sep_chain_t* rover = NULL;
        sep_chain_t* rover_next = NULL;
        // sep_chain_t* rover_next = NULL;
        for (int i=0; i<T->table_size_M; i++){
            rover = T->sc[i];
            while (rover != NULL && rover->key != NULL){
                table_insert(new_table, strdup(rover->key), rover->data_ptr);
                if (rover != NULL){
                    rover_next = rover->next;
                    free(rover->key);
                    free(rover);
                    rover = rover_next;
                }
            }
        }
        free(T->sc);
        free(T);
    }
    return new_table;    
}

/* returns number of entries in the table */
int table_entries(table_t *T){
    if (T != NULL){
        return T->num_keys_stored_in_table;
    } else {
        return 0;
    }
}

/* returns 1 if table is full and 0 if not full. 
 * For separate chaining the table is never full
 */
int table_full(table_t *T){
    if (T->num_keys_stored_in_table == T->table_size_M-1){
        return 1;
    } else {
        return 0;
    }
}

/* returns the number of table entries marked as deleted.  
 *
 * For separate chaining the number of delete keys is always zero.
 */
int table_deletekeys(table_t *T){
    int del_count = 0;
    if (T->type_of_probing_used_for_this_table != CHAIN){
        for (int i=0; i<T->table_size_M; i++){
            del_count += T->oa[i].deleted;
        }
        return del_count;
    } else {
        return 0;
    }
}
   
/* Insert a new table entry (K, I) into the table provided the table is not
 * already full.  
 * Return:
 *      0 if (K, I) is inserted, 
 *      1 if an older (K, I) is already in the table (update with the new I), or 
 *     -1 if the (K, I) pair cannot be inserted.
 *
 * Note that both K and I are pointers to memory blocks created by malloc()
 */
int table_insert(table_t *T, hashkey_t K, data_t I){
    int i, i_initial, i_firstdeleted;
    T->num_probes_for_most_recent_call = 0;
    i = table_hash(K,T->table_size_M);
    i_initial = i;
    i_firstdeleted = -1;
    if (T->type_of_probing_used_for_this_table != CHAIN){
        int ProbeDecrement;
        if (T->type_of_probing_used_for_this_table == DOUBLE){
            ProbeDecrement = table_probe_dec(K, T->table_size_M);
        } else {
            ProbeDecrement = 1;
        }
        while (T->oa[i].key != NULL || T->oa[i].deleted == 1) {
            if (T->oa[i].deleted == 1 && i_firstdeleted == -1){
                i_firstdeleted = i;
            }
            if (T->oa[i].deleted == 0 && strcmp(T->oa[i].key, K)==0){
                free(T->oa[i].data_ptr);
                T->oa[i].data_ptr = I;
                free(K);
                return 1;
            }
            i -= ProbeDecrement; /* compute next probe location */
            if (i < 0) {
                i += T->table_size_M; /* wrap around if needed */
            }
            T->num_probes_for_most_recent_call++;
            if (i == i_initial){
                if (i_firstdeleted != -1){
                    break;
                }
                
                printf("repeated insert loop detected, exiting..\n");
                free(K);
                free(I);
                return -1;
            }
        }

        if (T->num_keys_stored_in_table == T->table_size_M-1){
            return -1;
        }
        if (i_firstdeleted != -1){
            i = i_firstdeleted;
            free(T->oa[i].key);
        }
        T->oa[i].key = K; /* insert new key K in table T, and then */
        T->oa[i].data_ptr = I; 
        T->oa[i].deleted = 0;
        T->num_keys_stored_in_table++;
        return 0;
    } else {;
        sep_chain_t* rover = T->sc[i];
        if (rover == NULL){
            sep_chain_t* new_entry = (sep_chain_t*)malloc(sizeof(sep_chain_t));
            new_entry->key = K;
            new_entry->data_ptr = I;
            new_entry->next = NULL;
            T->sc[i] = new_entry;
            T->num_keys_stored_in_table++;  
        } else {
                while (rover->next != NULL){
                    T->num_probes_for_most_recent_call++;
                    if (strcmp(K, rover->key)==0){
                        free(rover->data_ptr);
                        rover->data_ptr = I;
                        free(K);
                        return 1;
                    }
                    rover = rover->next;
                }
                if (rover != NULL){
                    T->num_probes_for_most_recent_call++;
                    if (strcmp(K, rover->key)==0){
                        free(rover->data_ptr);
                        rover->data_ptr = I;
                        free(K);
                        return 1;
                    }
                }
                sep_chain_t* new_entry = (sep_chain_t*)malloc(sizeof(sep_chain_t));
                new_entry->key = K;
                new_entry->data_ptr = I;
                new_entry->next = NULL;
                rover->next = new_entry;
                T->num_keys_stored_in_table++;     
                return 0;      
            // }
        }
    }
    return 0;
}

/* Delete the table entry (K, I) from the table.  Free the key in the table.
 * Return:
 *     pointer to I, or
 *     null if (K, I) is not found in the table.  
 *
 * Be sure to free(K)
 *
 * See the note on page 490 in Standish's book about marking table entries for
 * deletions when using open addressing.  Review the -d driver for examples
 * of troublesome cases.
 */
data_t table_delete(table_t *T, hashkey_t K){
    int i, i_initial;
    int ProbeDecrement;
    T->num_probes_for_most_recent_call = 0;
    i = table_hash(K,T->table_size_M);
    i_initial = i;
    if (T->type_of_probing_used_for_this_table != CHAIN){
        if (T->type_of_probing_used_for_this_table == DOUBLE){
            ProbeDecrement = table_probe_dec(K, T->table_size_M);
        } else {
            ProbeDecrement = 1;
        }
        while (T->oa[i].key != NULL || T->oa[i].deleted == 1) {
            T->num_probes_for_most_recent_call++;
            if (T->oa[i].deleted == 0){
                if (strcmp(T->oa[i].key, K)==0){
                    T->oa[i].deleted = 1;
                    T->num_keys_stored_in_table--;
                    return T->oa[i].data_ptr;
                }
            }
            i -= ProbeDecrement; /* compute next probe location */
            if (i < 0) {
                i += T->table_size_M; /* wrap around if needed */
            }
            if (i == i_initial){
                return NULL;
            }
        }
    } else {
        sep_chain_t* rover = T->sc[i];
        sep_chain_t* rover_prev = T->sc[i];
        data_t retval = NULL;

        if (rover == NULL){
            return NULL;
        }
        while (rover != NULL){
            T->num_probes_for_most_recent_call++;
            if (strcmp(rover->key,K)==0){
                if (rover == T->sc[i]){
                    free(rover->key);
                    retval = rover->data_ptr;
                    T->sc[i] = rover->next;
                    T->num_keys_stored_in_table--;
                    free(rover);
                    return retval;
                } else {
                    retval = rover->data_ptr;
                    rover_prev->next = rover->next;
                    free(rover->key);
                    free(rover);
                }
                T->num_keys_stored_in_table--;
                return retval;
            }
            rover_prev = rover;
            rover = rover->next;
        }
    }
    return NULL;
}

/* Given a key, K, retrieve the pointer to the information, I, from the table,
 * but do not remove (K, I) from the table.  Return NULL if the key is not
 * found.
 */
data_t table_retrieve(table_t *T, hashkey_t K){
    int i, i_initial;
    int ProbeDecrement;
    T->num_probes_for_most_recent_call = 0;
    i = table_hash(K,T->table_size_M);
    i_initial = i;
    if (T->type_of_probing_used_for_this_table != CHAIN){
        if (T->type_of_probing_used_for_this_table == DOUBLE){
            ProbeDecrement = table_probe_dec(K, T->table_size_M);
        } else {
            ProbeDecrement = 1;
        }
        while (T->oa[i].key != NULL) {
            T->num_probes_for_most_recent_call++;
            if (T->oa[i].deleted != 1){
                if (strcmp(T->oa[i].key, K)==0){
                    return T->oa[i].data_ptr;
                }
            }
            i -= ProbeDecrement; /* compute next probe location */
            if (i < 0) {
                i += T->table_size_M; /* wrap around if needed */
            }
            if (i == i_initial){
                return NULL;
            }
        }
    } else {
        sep_chain_t* rover = T->sc[i];
        if (rover == NULL){
            return NULL;
        }
        if (rover->key == NULL){
            return NULL;
        }
        while (rover != NULL){
            T->num_probes_for_most_recent_call++;
            if (strcmp(rover->key,K)==0){
                return rover->data_ptr;
            }
            rover = rover->next;
        }
    }
    return NULL;
}

/* Free all information in the table, the table itself, and any additional
 * headers or other supporting data structures.  
 */
void table_destruct(table_t *T){
    if (T->type_of_probing_used_for_this_table != CHAIN){
        for (int i=0; i<T->table_size_M; i++){
            if (T->oa[i].deleted != 1){
                free(T->oa[i].data_ptr);
            }
            free(T->oa[i].key);
        }
        free(T->oa);
        free(T);
    } else {
        for (int i=0; i<T->table_size_M; i++){
            sep_chain_t* rover = T->sc[i];
            sep_chain_t* rover_next = rover;
            while (rover != NULL){
                rover_next = rover->next;
                free(rover->data_ptr);
                free(rover->key);
                free(rover);
                rover = rover_next;
            }        
        }
        free(T->sc);
        free(T);
    }

}

/* The number of probes for the most recent call to table_retrieve,
 * table_insert, or table_delete 
 */
int table_stats(table_t *T){
    return T->num_probes_for_most_recent_call;
}

/* This function is for testing purposes only.  Given an index position into
 * the hash table return the value of the key if data is stored in this 
 * index position.  If the index position does not contain data, then the
 * return value must be zero.  Make the first
 * lines of this function 
 *       assert(0 <= index && index < table_size); 
 *       assert(0 <= list_position); 
 */
hashkey_t table_peek(table_t *T, int index, int list_position){
    assert(0 <= index && index < T->table_size_M); 
    assert(0 <= list_position); 
    if (T->type_of_probing_used_for_this_table != CHAIN){
        if (T->oa[index].key != NULL && T->oa[index].deleted != 1){
            return T->oa[index].key;
        }        
    } else {
        if (T->sc[index] != NULL && T->sc[index]->key != NULL){
            int i = 0;
            sep_chain_t *rover = T->sc[index]; 
            while (rover != NULL && i != list_position){
                rover = rover->next;
                i++;
            }
            if (rover == NULL){
                return NULL;
            }
            return rover->key;
        }
    }
    return NULL;
}

/* Print the table position and keys in a easily readable and compact format.
 * Only useful when the table is small.
 */
void table_debug_print(table_t *T){
    if (T->type_of_probing_used_for_this_table != CHAIN){
        for (int i=0; i<T->table_size_M; i++){
            if (T->oa[i].key != NULL){
                if (T->oa[i].deleted == 0){
                    printf("T->oa[%d] %s\n", i, T->oa[i].key);
                } else {
                    printf("T->oa[%d] %s\n", i, "--del--");
                }
            } else {

                printf("T->oa[%d] %s\n", i, "∅");
            }
        }
    } else {
        sep_chain_t* rover = NULL;
        for (int i=0; i<T->table_size_M; i++){
            rover = T->sc[i];
            if (rover != NULL){
                if (rover->key != NULL){
                    printf("T->sc[%d] %s ", i, rover->key);
                } else {
                    printf("T->sc[%d] %s ", i, "∅");
                }
            } else {
                printf("T->sc[%d] %s ", i, "∅");
            }
            while (rover != NULL){
                rover = rover->next;
                if (rover != NULL){
                    printf("→ %s ", rover->key);
                }
            }
            printf("\n");
        }
    }
}