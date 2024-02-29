/* lab1.c
 * Michael Morris
 * mvmorri
 * ECE 2230-001 Fall 2023
 * MP1
 *
 * Purpose: This program acts as a way to create, update, monitor, and destroy
 *      a list of access points and their accompanying data. This program presents the 
 *      user with a command line interface menu that will perform various tasks on the 
 *      list. This machine problem is designed to test our understanding of dynamic 
 *      memory allocation and the use of pointers to create a stable, efficient program 
 *      capable of handling large data sets.  
 *
 * Assumptions: Input data values for each entry will be properly formatted
 *      and within normal bounds. It is forbidden to access any of the members 
 *      in the ap_list_t structure. The members that start with apl followed 
 *      by an underscore are not found in this file.
 *
 * Bugs: The input eth_address is accepted as a signed integer. This does not have 
 *      an effect on this specific MP unless a large eth_address is provided and the result is
 *      interpreted as a negative number.
 *      Additionally - not necessarily a bug - the expected output that was provided to us shows 
 *      that if the list is full and an eth_address already in the list is added, the error should 
 *      show that the list is full and not that the entry is already in the list. Again, this
 *      doesn't effect the output as both result in the same outcome..
 *      
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "aplist.h"

#define MAXLINE 256

// function prototypes for lab1.c
void get_ap_info(struct ap_info_t *new, int sta_id);
void print_ap_info(struct ap_info_t *rec);

int main(int argc, char *argv[])
{
    char line[MAXLINE];
    char command[MAXLINE];
    char extra[MAXLINE];
    int list_size = -1;
    int token_cnt;
    int i;
    int ap_id;                         // Access Point ID or ethernet address
    struct ap_info_t *rec_ptr = NULL;  // Record of information about an Access Point
    struct ap_list_t *list_ptr = NULL; // WiFi Access Points

    if (argc != 2) {
        printf("Error with input paramters.  Usage: ./lab1 list_size\n");
        exit(1);
    }
    list_size = atoi(argv[1]);
    if (list_size < 2) {
        printf("Invalid list size %d input was %s\n", list_size, argv[1]);
        exit(1);
    } else {
        printf("List size: %d\n", list_size);
    }

    printf("lab1 commands:\n");
    printf("\tADD sta\n\tREMOVE sta\n\tFIND sta\n\tDEC sta\n\tINC sta\n\tREMOVEALL\n\tPRINT\n\tSTATS\n\tQUIT\n");
    

    list_ptr = aplist_construct(list_size);

    // fgets includes newline \n unless line too long 
    while (fgets(line, MAXLINE, stdin) != NULL) {
        token_cnt = sscanf(line, "%s%d%s", command, &ap_id, extra);
        if (token_cnt == 2 && strcmp(command, "ADD") == 0) {
            rec_ptr = (struct ap_info_t *) calloc(1, sizeof(struct ap_info_t));
            get_ap_info(rec_ptr, ap_id);

            int add_return = -2;
            add_return = aplist_add(list_ptr, rec_ptr);

            if (add_return == 0) {
                printf("Inserted %d\n", ap_id);
            } else if (add_return == 1) {
                printf("Rejected %d because list is full with %d entries\n", ap_id, aplist_number_entries(list_ptr));
            } else if (add_return == 2) {
                printf("Rejected %d already in list\n", ap_id);
            } else {
                printf("Error for ADD return value.\n");
            }
            free(rec_ptr);
            rec_ptr = NULL;
        } else if (token_cnt == 2 && strcmp(command, "REMOVE") == 0) {
            rec_ptr = aplist_remove(list_ptr, ap_id);
 
            if (rec_ptr == NULL) {
                printf("Remove did not find: %d\n", ap_id);
            } else {
                printf("Removed: %d\n", ap_id);
                print_ap_info(rec_ptr);
                // free(rec_ptr);
            }
            free(rec_ptr);
            rec_ptr = NULL;
        } else if (token_cnt == 2 && strcmp(command, "FIND") == 0) {
            rec_ptr = aplist_find(list_ptr, ap_id);

            if (rec_ptr == NULL) {
                printf("Did not find access point with id: %d\n", ap_id);
            } else {
                // after searching list for record, verify found correct one 
                printf("%d mobiles registered with AP %d\n", rec_ptr->mobile_count, ap_id); 
                assert(rec_ptr->eth_address == ap_id);
            }
            rec_ptr = NULL;
        } else if (token_cnt == 2 && strcmp(command, "DEC") == 0) {
            int dec_return = aplist_dec(list_ptr, ap_id);
            if (dec_return == -2) {
                printf("Decrement failed.  AP %d not found\n", ap_id);
            } else if (dec_return == -1) {
                printf("Decrement for AP %d failed.  Count is already zero\n", ap_id);
            } else if (dec_return >= 0) {
                printf("Decremented AP %d to %d\n", ap_id, dec_return);
            } else {
                printf("Decrement return value %d invalid for AP %d.  Fix your code.\n", dec_return, ap_id);
            }
        } else if (token_cnt == 2 && strcmp(command, "INC") == 0) {
            int inc_return = aplist_inc(list_ptr, ap_id);
            if (inc_return == -2) {
                printf("Increment failed.  AP %d not found\n", ap_id);
            } else if (inc_return > 0) {
                printf("Incremented AP %d to %d\n", ap_id, inc_return);
            } else {
                printf("Increment return value %d invalid for AP %d.  Fix your code.\n", inc_return, ap_id);
            }
        } else if (token_cnt == 1 && strcmp(command, "REMOVEALL") == 0) {
            int count = aplist_number_entries(list_ptr);
            aplist_remove(list_ptr, -1);
            if (count == 0) {
                printf("No APs found\n");
            } else {
                printf("Removed %d station%s\n", count, count==1?"":"s");
            }
        } else if (token_cnt == 1 && strcmp(command, "PRINT") == 0) {
            int list_count = aplist_number_entries(list_ptr);
            if (list_count == 0) {
                printf("List empty\n");
            } else {
                printf("List has %d record%s\n", list_count, list_count==1?"":"s");
                for (i = 0; i < list_count; i++) {
                    printf("%d: ", i);
                    rec_ptr = aplist_access(list_ptr, i);
                    print_ap_info(rec_ptr);
                }
            }
            rec_ptr = NULL;
        } else if (token_cnt == 1 && strcmp(command, "STATS") == 0) {
            // get the number in list and print size of the list
            int list_count = aplist_number_entries(list_ptr);
            printf("Number records: %d, Array size: %d\n", list_count, list_size);
        } else if (token_cnt == 1 && strcmp(command, "QUIT") == 0) {
            aplist_remove(list_ptr, -1);
            aplist_destruct(list_ptr);
            printf("Cleaning up.  Goodbye\n");
            break;
        } else {
            printf("# %s", line);
        }
    }
    exit(0);
}

/* Prompts user for information about Access Point.
 * The input is not checked for errors but will default to an acceptable value
 * if the input is incorrect or missing.
 *
 * The input to the function assumes that the structure has already been
 * allocated.  The contents of the structure are filled in.
 *
 * There is no return value.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void get_ap_info(struct ap_info_t *new, int sta_id)
{
    char line[MAXLINE];
    char str[MAXLINE];
    char letter;
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
}

/* print the information for a particular access point record 
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void print_ap_info(struct ap_info_t *rec)
{
    const char *pri_str[] = {"none", "WEP", "WPA", "WPA2"};
    assert(rec != NULL);
    printf("mc: %d,", rec->mobile_count);
    printf(" eth: %d,", rec->eth_address);
    printf(" IP: %d, Loc: %d,", rec->ip_address, 
            rec->location_code);
    printf(" Auth: %s,", rec->authenticated ? "T" : "F"); 
    printf(" Pri: %s, L: %c, B: %g, C: %d,", pri_str[rec->privacy], 
            (char) (rec->standard_letter + 'a'), rec->band, rec->channel);
    printf(" R: %g", rec->data_rate);
    printf(" Time: %d\n", rec->time_received);
}


/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
