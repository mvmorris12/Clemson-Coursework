/* lab2.c
 * Michael Morris
 * Mvmorri
 * ECE 2230 Fall 2023
 * MP2
 *
 * Propose: This project creates and maintains a list of wifi access points
 *          through the use of linked lists. The user is able to insert and
 *          remove items from the sorted list, add to a running unsorted queue,
 *          increase / decrease the mobile count of items in the sorted list,
 *          and print statistics about both lists. 
 *
 * Assumptions: The main function collects input commands and
 *              calls the appropriate wifi function.
 *
 *              Other than updating comments, this file has not been changed
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "twl_list.h"
#include "ap_support.h"

int main(int argc, char * argv[])
{
    twl_list_t *ap_leaderboard;
    twl_list_t *ap_queue;
    char line[MAXLINE];
    char command[MAXLINE];
    char junk[MAXLINE];
    int num_items;
    int ap_id;

    if (argc != 2) {
        printf("Error with input paramters.  Usage: ./lab2 leaderboard_size\n");
        exit(1);
    }
    int lb_listsize = atoi(argv[1]);
    if (lb_listsize < 2) {
        printf("Invalid leaderboard size %d from input %s\n", lb_listsize, argv[1]);
        exit(1);
    } else {
        printf("Leaderboard lists the top %d access points\n", lb_listsize);
    }

    printf("Leaders  : ADD sta; REMOVE sta; FIND sta; INC sta; DEC sta, REMOVEALL; PRINT\n");
    printf("Queue    : JOINQ sta; MOVEQTOL; PRINTQ\n");
    printf("         : STATS; QUIT\n");

    /* this list is sorted and the size of the list is limited */
    ap_leaderboard = ap_create_leaderboard();

    /* this list is unsorted and the list size is not limited */
    ap_queue = twl_list_construct(NULL);

    /* remember fgets includes newline \n unless line too long */
    while (fgets(line, MAXLINE, stdin) != NULL) {
        num_items = sscanf(line, "%s%d%s", command, &ap_id, junk);
        if (num_items == 2 && strcmp(command, "ADD") == 0) {
            ap_add(ap_leaderboard, ap_id, lb_listsize);
        } else if (num_items == 2 && strcmp(command, "REMOVE") == 0) {
            ap_remove(ap_leaderboard, ap_id);
        } else if (num_items == 2 && strcmp(command, "FIND") == 0) {
            ap_find(ap_leaderboard, ap_id);
        } else if (num_items == 2 && strcmp(command, "INC") == 0) {
            ap_inc(ap_leaderboard, ap_id);
        } else if (num_items == 2 && strcmp(command, "DEC") == 0) {
            ap_dec(ap_leaderboard, ap_id);
        } else if (num_items == 1 && strcmp(command, "PRINT") == 0) {
            ap_print_list(ap_leaderboard, "Leaderboard");
        } else if (num_items == 1 && strcmp(command, "REMOVEALL") == 0) {
            ap_removeall(ap_leaderboard);
        } else if (num_items == 2 && strcmp(command, "JOINQ") == 0) {
            ap_enqueue(ap_queue, ap_id);
        } else if (num_items == 1 && strcmp(command, "MOVEQTOL") == 0) {
            ap_dequeue(ap_queue, ap_leaderboard, lb_listsize);
        } else if (num_items == 1 && strcmp(command, "PRINTQ") == 0) {
            ap_print_list(ap_queue, "Queue");
        } else if (num_items == 1 && strcmp(command, "STATS") == 0) {
            ap_stats(ap_leaderboard, ap_queue);
        } else if (num_items == 1 && strcmp(command, "QUIT") == 0) {
            ap_cleanup(ap_leaderboard);
            ap_cleanup(ap_queue);
            printf("Goodbye\n");
            break;
        } else {
            printf("# %s", line);
        }
    }
    exit(0);
}
/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
