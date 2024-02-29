/* ap_support.h 
 * Michael Morris
 * Mvmorri
 * ECE 2230 Fall 2023
 * MP2
 *
 * Propose: A template for MP2 
 *
 * Assumptions: structure definitions and public functions as defined for
 *              assignment.  There are no extensions.
 *
 *              All function prototypes in this file must start with the prefix
 *              ap_ and are public.  
 *
 * Bugs: None..
 *
 * You may change this file if needed for your design.
 */

#define MAXLINE 170

/* prototype function definitions */

/* function to compare access point records */
int ap_rank_aps(const ap_info_t *rec_a, const ap_info_t *rec_b);
int ap_match_eth(const ap_info_t *rec_a, const ap_info_t *rec_b);

/* functions to create and cleanup a AP list */
twl_list_t *ap_create_leaderboard(void);
void ap_cleanup(twl_list_t *);

/* Functions to get and print AP information */
ap_info_t *ap_create_info(int id);   /* collect input from user */
void ap_print_info(ap_info_t *rec);  /* print one record */
void ap_print_list(twl_list_t *list_ptr, const char *);      /* print list of records */
void ap_stats(twl_list_t *leaderboard, twl_list_t *queue);

/* functions for sorted list */
void ap_add(twl_list_t *, int, int);
void ap_find(twl_list_t *, int);
void ap_remove(twl_list_t *, int);
void ap_inc(twl_list_t *, int);
void ap_dec(twl_list_t *, int);
void ap_removeall(twl_list_t *);

/* functions for unsorted FIFO list 
 * inserts at the back, removes at the front, 
 */
void ap_dequeue(twl_list_t *, twl_list_t *, int);
void ap_enqueue(twl_list_t *, int);

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
