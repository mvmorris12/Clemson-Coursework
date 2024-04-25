/*  Lab 4 - Huffman Codec
 *  Michael Morris & Justin Wolf
 *  mvmorri
 *  ECE 4680
 *  3/5/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


typedef struct tree_node{
    unsigned char entry;
    unsigned int frequency;
    unsigned int branch_levels;
    unsigned int branch_weight_tot;
    unsigned int branch_id;
    struct tree_node *left;
    struct tree_node *right;
    struct tree_node *parent;
    struct tree_node *next;
    struct tree_node *prev;
} tree_node_t;


typedef struct tree{
    tree_node_t *first;
    tree_node_t *root;
    unsigned int branches;
    unsigned int *buffer;
    unsigned char *buffer_len;
} tree_t;


typedef struct ll_node{
    unsigned char entry;
    unsigned int frequency;
    struct ll_node *prev;
    struct ll_node *next;
} ll_node_t;


typedef struct ll{
    ll_node_t *front;
    ll_node_t *back;
    unsigned char entries;
} ll_t;


typedef struct file_info{
    char *filename;
    unsigned int filesize;
    FILE *f_ptr;
    void *buffer;
    
    ll_t *freq_ll;
    ll_node_t **freq_ll_ptr_arr;
    tree_t *bst;
} file_info_t;


typedef struct params{
    unsigned char compress_flag, decompress_flag;
    file_info_t *input_file, *output_file;
} params_t;



/* Fx Prototypes */
void parse_commands(int argc, char *argv[], params_t *p);
unsigned char open_files(params_t*p);
void read_input_file(params_t*p);
void compress_file(params_t*p);
void decompress_file(params_t*p);

void ll_sort_asc(ll_t *ll);
tree_node_t *bst_get_branch_root_and_weight(tree_node_t *node, unsigned int *weight, unsigned int *levels);

tree_node_t *bst_get_root_node(tree_node_t *node);

/* parses command line arguments and allocates space for variables */
void parse_commands(int argc, char *argv[], params_t*p){
    int c;

    /* tell user how to use program if incorrect arguments */
    if (argc < 2){
        printf("Usage:  lab4 [input filename] [-c] [-d] [-o : filename]\n");
        printf("\t-c = compress file\n");
        printf("\t-d = decompress file\n");
        printf("\t-o = output filename\n");
        exit(EXIT_FAILURE);
    }

    /* allocate space for objects */
    p->input_file = (file_info_t*)malloc(sizeof(file_info_t));
    p->output_file = (file_info_t*)malloc(sizeof(file_info_t));
    p->compress_flag = p->decompress_flag = false;
    p->input_file->filename = p->output_file->filename = NULL;
  
    /* get command line arguments and parse */
    while ((c = getopt(argc, argv, "o:i:cd")) != -1){
        switch(c) { 
            case 'c': 
                p->compress_flag = true;
                break;
            case 'd': 
                p->decompress_flag = true;
                break;
            case 'i':
                p->input_file->filename = (char*)malloc((strlen(optarg)+1)*sizeof(char)); 
                strcpy(p->input_file->filename, optarg);
                break;
            case 'o': 
                p->output_file->filename = (char*)malloc((strlen(optarg)+1)*sizeof(char));
                strcpy(p->output_file->filename, optarg);
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (p->compress_flag == p->decompress_flag){
        printf("Error\nMust select EITHER compress or decompress\n");
        exit(EXIT_FAILURE);
    }

    if (p->input_file->filename == NULL || p->output_file->filename == NULL){
        printf("Error\nMust provide BOTH input and output filenames\n");
        exit(EXIT_FAILURE);
    }
}


unsigned char open_files(params_t*p){
    /* open file for reading */
    p->input_file->f_ptr=fopen(p->input_file->filename,"rb");
    if (p->input_file->f_ptr == NULL){
        printf("Unable to open %s for writing\n", p->input_file->filename);
        exit(EXIT_FAILURE);
    } else {
        /* get file size */
        fseek(p->input_file->f_ptr, 0L, SEEK_END);
        p->input_file->filesize = ftell(p->input_file->f_ptr);
        rewind(p->input_file->f_ptr);
        p->output_file->filesize = 0;
    }

    /* open file for writing */
    p->output_file->f_ptr=fopen(p->output_file->filename,"wb");
    if (p->output_file->f_ptr == NULL){
        printf("Unable to open %s for writing\n", p->output_file->filename);
        fclose(p->input_file->f_ptr);
        exit(EXIT_FAILURE);
    }

    return 1;
}


/* read input file into buffer */
void read_input_file(params_t*p){
    if (p->compress_flag){
        p->input_file->buffer = (unsigned char*)malloc(sizeof(unsigned char)*p->input_file->filesize);
        fread((unsigned char*)p->input_file->buffer, 1, p->input_file->filesize, p->input_file->f_ptr);
    } else {
        p->input_file->buffer = (unsigned int*)malloc(sizeof(unsigned int)*p->input_file->filesize);
        fread((unsigned int*)p->input_file->buffer, sizeof(unsigned int), p->input_file->filesize, p->input_file->f_ptr);
        // fread((unsigned char*)p->input_file->buffer, sizeof(unsigned int), p->input_file->filesize-256*(sizeof(unsigned int)), p->input_file->f_ptr);
    }

    fclose(p->input_file->f_ptr);
}


/* allocate space for linked list and insert empty nodes */
void ll_create(params_t *p){
    ll_node_t *ll_node_ptr;
    ll_node_t *ll_node_prev_ptr;
    int i=0;

    p->input_file->freq_ll = (ll_t*)malloc(sizeof(ll_t));
    p->input_file->freq_ll_ptr_arr = (ll_node_t**)malloc(sizeof(ll_node_t*)*256);
    p->input_file->freq_ll->entries = 0;
    for (i=0; i<256; i++){
        ll_node_ptr = (ll_node_t*)malloc(sizeof(ll_node_t));
        ll_node_ptr->entry = i;
        ll_node_ptr->frequency = 0;
        if (i){
            ll_node_ptr->prev = ll_node_prev_ptr;
            ll_node_prev_ptr->next = ll_node_ptr;
            ll_node_ptr->next = NULL;
        } else {
            ll_node_ptr->prev = NULL;
            ll_node_ptr->next = NULL;
            p->input_file->freq_ll->front = ll_node_ptr;
        }
        p->input_file->freq_ll->back = ll_node_ptr;
        p->input_file->freq_ll_ptr_arr[i] = ll_node_ptr;
        ll_node_prev_ptr = ll_node_ptr;
    }

    /* calculate frequencies of each symbol */
    if (p->compress_flag){
        for (i=0; i<p->input_file->filesize; i++){
            p->input_file->freq_ll_ptr_arr[((unsigned char*)p->input_file->buffer)[i]]->frequency++;
            if (p->input_file->freq_ll_ptr_arr[((unsigned char*)p->input_file->buffer)[i]]->frequency == 1){
                p->input_file->freq_ll->entries++;
            }
        }
        
    } else {
        for (i=0; i<256; i++){
            p->input_file->freq_ll_ptr_arr[i]->frequency = ((unsigned int*)p->input_file->buffer)[i];
            if (p->input_file->freq_ll_ptr_arr[i]->frequency){
                p->input_file->freq_ll->entries++;
                
                
            }
        }
    }
}


/* sort linked list into ascending order in preparation for Huffman encoding */
void ll_sort_asc(ll_t *ll){
    ll_node_t *ll_ptr = ll->front->next;
    ll_node_t *ll_ptr_next = ll_ptr->next;
    ll_node_t *rover = ll->front;

    while (ll_ptr != NULL){
        while ((rover != NULL) && (rover->frequency <= ll_ptr->frequency) && (rover->entry < ll_ptr->entry)){
            rover = rover->next;
            if (rover->frequency == ll_ptr->frequency){
                while(rover->frequency == ll_ptr->frequency && rover->entry <= ll_ptr->entry){
                    rover = rover->next;
                }   
                break;
            }
        }
        if (ll_ptr->entry != rover->entry){
            if (ll_ptr->next != NULL){
                ll_ptr->next->prev = ll_ptr->prev;
            } else {
                ll->back = ll_ptr->prev;
            }
            if (ll_ptr->prev != NULL){
                ll_ptr->prev->next = ll_ptr->next;
            }
            if (rover->prev != NULL){
                ll_ptr->prev = rover->prev;
                rover->prev->next = ll_ptr;
            } else {
                ll->front = ll_ptr;
                ll_ptr->prev = NULL;
            }
            rover->prev = ll_ptr;
            ll_ptr->next = rover;
        }
        if (ll_ptr_next != NULL){
            ll_ptr = ll_ptr_next;
            ll_ptr_next = ll_ptr_next->next;
        } else {
            return;
        }   
        rover = ll->front;

    }
}


void bst_add_nodes(ll_node_t *ll_node, tree_t *bst){
    bst->first = (tree_node_t*)malloc(sizeof(tree_node_t));
    tree_node_t *bst_node_prev = bst->first;
    
    unsigned char i=0;
    while (ll_node != NULL){
        tree_node_t *bst_node_new = (tree_node_t*)malloc(sizeof(tree_node_t));
        bst_node_new->entry = ll_node->entry;
        bst_node_new->frequency = ll_node->frequency;
        bst_node_new->branch_weight_tot = ll_node->frequency;
        bst_node_new->prev = NULL;
        bst_node_new->next = NULL;
        bst_node_new->left = NULL;
        bst_node_new->right = NULL;
        bst_node_new->parent = NULL;
        bst_node_new->branch_levels = 1;
        bst_node_new->branch_id = i;
        if (i++){
            bst_node_prev->next = bst_node_new;
            bst_node_new->prev = bst_node_prev;
            bst_node_prev = bst_node_new;
        } else {
            bst->first->entry = ll_node->entry;
            bst->first->frequency = ll_node->frequency;
            bst->first->branch_levels = 1;
            bst->first->branch_weight_tot = ll_node->frequency;
            bst->first->branch_id = 0;
            bst_node_prev = bst->first;
        }
        ll_node = ll_node->next;
        
    }
    bst->branches = i;
}



tree_node_t *bst_get_leftmost_node(tree_node_t *start_node){

    tree_node_t *left_node = start_node;

    while ((left_node != NULL) && (left_node->parent != NULL)){
        left_node = left_node->parent;
    }
    while (left_node->left != NULL){
        left_node = left_node->left;
    }
    return left_node;

}


void bst_resort(params_t *p){
    tree_node_t *node_next = p->input_file->bst->first;
    tree_node_t *node = p->input_file->bst->first;
    tree_node_t *rover = p->input_file->bst->first;
    unsigned int first_bid = node->branch_id;
    while ((node != NULL) && (node->branch_id == first_bid)){
        node = node->next;
        if (p->input_file->bst->branches == 1){
            return;
        }
        while (rover != NULL && node != NULL && rover->entry != node->entry){
            if ((node->branch_weight_tot < rover->branch_weight_tot) || 
                ((node->branch_weight_tot == rover->branch_weight_tot) && 
                 ((node->branch_levels < rover->branch_levels) || 
                  ((node->branch_levels == rover->branch_levels) && 
                   (node->branch_id < rover->branch_id))))){
                rover = bst_get_leftmost_node(rover);
                if (node->prev != NULL){
                    node->prev->next = node->next;
                    node_next = node->next;
                }
                if (node->next != NULL){
                    node->next->prev = node->prev;
                }
                node->next = rover;
                node->prev = rover->prev;
                if (rover->prev != NULL){
                    rover->prev->next = node;
                }
                rover->prev = node;
                if (rover == p->input_file->bst->first){
                    p->input_file->bst->first = node;
                    node->prev = NULL;
                }
                rover = p->input_file->bst->first;
                node = node_next;
            } 
            rover = rover->next;
        }
    }
}


tree_node_t *bst_get_branch_root_and_weight(tree_node_t *node, unsigned int *weight, unsigned int *levels){
    tree_node_t *rover = node;
    *levels = 1;
    while (rover->parent != NULL){
        (*levels)++;
        rover = rover->parent;
    }
    *weight = rover->frequency;
    return rover;
}


tree_node_t *bst_get_root_node(tree_node_t *node){
    if ((node == NULL) || (node->parent == NULL)){
        return node;
    }
    tree_node_t *rover = node;
    while (rover->parent != NULL){
        rover = rover->parent;
    }
    return rover;
}


void bst_update_nodes_recursive(tree_node_t *root_node, unsigned int weight, unsigned int levels, unsigned int branch_id){
    if (root_node == NULL){
        return;
    }
    root_node->branch_weight_tot = weight;
    root_node->branch_levels = levels;
    root_node->branch_id = branch_id;
    bst_update_nodes_recursive(root_node->left, weight, levels, branch_id);
    bst_update_nodes_recursive(root_node->right, weight, levels, branch_id);
}


void bst_update_nodes(params_t *p){
    tree_node_t *rover = p->input_file->bst->first;
    tree_node_t *root = NULL;
    tree_node_t *prev_root = NULL;
    unsigned int weight = 1;
    unsigned int branch_id;
    while ((rover != NULL) && (rover->parent != NULL)){
        unsigned int levels;
        root = bst_get_branch_root_and_weight(rover, &weight, &levels);
        branch_id = rover->branch_id;
        if (root != prev_root){
            bst_update_nodes_recursive(root, weight, levels, branch_id);
            prev_root = root;
        }
        rover = rover->next;
    }
}


unsigned int bst_combine_nodes(params_t *p){
    tree_node_t *rover = p->input_file->bst->first->next;
    tree_node_t *rover_root = NULL;
    tree_node_t *rover_prev = p->input_file->bst->first;
    tree_node_t *rover_prev_root = NULL;
    tree_node_t *parent_node = NULL;
    while ((rover != NULL) && (rover->branch_id == rover_prev->branch_id)){
        rover_prev = rover;
        rover = rover->next;
    }
    if (rover == NULL){
        printf("Error... combine rover is null, exiting\n");
        exit(EXIT_FAILURE);
    }
    rover_root = bst_get_root_node(rover);
    rover_prev_root = bst_get_root_node(rover_prev);

    if ((rover_root->parent == NULL) && (rover_prev_root->parent == NULL)){
        parent_node = (tree_node_t*)malloc(sizeof(tree_node_t));
        rover_root->parent = parent_node;
        rover_prev_root->parent = parent_node;
        parent_node->left = rover_prev_root;
        parent_node->right = rover_root;
        parent_node->frequency = rover_root->frequency + rover_prev_root->frequency;
        p->input_file->bst->branches--;
    } else {
        printf("error\n");
    }
    return 0;
}


void bst_build(params_t *p){
    p->input_file->bst = (tree_t*)malloc(sizeof(tree_t));
    p->input_file->bst->branches = 0;
    ll_node_t *ll_node = p->input_file->freq_ll->back;
    while ((ll_node->prev != NULL) && (ll_node->prev->frequency != 0)){
        ll_node = ll_node->prev;
    }
    bst_add_nodes(ll_node, p->input_file->bst);
}


void bst_populate_output_buffer_length(params_t *p){
    tree_node_t *node = p->input_file->bst->first;
    tree_node_t *rover = node;
    tree_node_t *rover_prev;
    unsigned int level = 0;
    p->input_file->bst->buffer_len = (unsigned char*)calloc(256, sizeof(unsigned char));
    p->input_file->bst->buffer = (unsigned int*)calloc(256, sizeof(unsigned int));
    
    while (node != NULL){
        rover = node;
        while (rover->parent != NULL){
            rover_prev = rover;
            rover = rover->parent;
            if (rover->right == rover_prev){
                p->input_file->bst->buffer[node->entry] |= (0x1 << level);
            }
            level++;
        }
        p->input_file->bst->buffer_len[node->entry] = level;
        level = 0;
        node = node->next;
    }
}


void write_output_file(params_t *p){
    tree_node_t *rover = p->input_file->bst->first;
    unsigned char write_char = 0;
    unsigned char write_byte = 0;
    unsigned char read_byte = 0;
    unsigned char read_bit = 0;
    char shift_bit = 7;
    unsigned char bits_to_skip = 0;
    unsigned int bit_count = 0;
    unsigned char ret_flag = false;
    if (p->compress_flag){
        p->output_file->buffer = (unsigned int*)calloc(256, sizeof(unsigned int));
        while (rover != NULL){
            ((unsigned int*)p->output_file->buffer)[rover->entry] = rover->frequency;
            rover = rover->next;
        }
        fwrite(p->output_file->buffer, sizeof(unsigned int), 256, p->output_file->f_ptr);
        p->output_file->filesize += 256*sizeof(unsigned int);
        for (int i=0; i<p->input_file->filesize; i++){
            write_char = ((unsigned char*)p->input_file->buffer)[i];
            for (int j=p->input_file->bst->buffer_len[write_char]-1; j>=0; j--){
                write_byte |= ((p->input_file->bst->buffer[((unsigned char*)p->input_file->buffer)[i]] >> j) & 0x1) << shift_bit;
                if (shift_bit <= 0){
                    shift_bit = 7;
                    fwrite(&write_byte, 1, 1, p->output_file->f_ptr);
                    p->output_file->filesize++;
                    write_byte = 0;
                } else {
                    shift_bit--;
                }
            }
        }
        if (++shift_bit < 8){
            fwrite(&write_byte, 1, 1, p->output_file->f_ptr);
            p->output_file->filesize++;
        } else {
            shift_bit = 0;
        }
            write_byte = 0;
            for (int l=0; l<p->output_file->filesize%4; l++){
                fwrite(&write_byte, 1, 1, p->output_file->f_ptr);
                shift_bit+=8;
                p->output_file->filesize++;
            }
        for (int i=0; i<3; i++){
            fwrite(&write_byte, 1, 1, p->output_file->f_ptr);
            p->output_file->filesize++;
        }

        fwrite(&shift_bit, 1, 1, p->output_file->f_ptr);
        p->output_file->filesize++;
    } else {

        bits_to_skip = ((unsigned int*)p->input_file->buffer)[(p->input_file->filesize-1)/4]>>24;
        int i=0;
        rover = p->input_file->bst->root;

        for (i=0; i<((p->input_file->filesize-1) - 256*sizeof(unsigned int))/4; i++){
            for (int j=0; j<4; j++){
                read_byte = (((unsigned int*)p->input_file->buffer)[256+i])>>(j*8);
                for (int k=7; k>=0; k--){
                    read_bit = (read_byte >> k) & 0x1;
                    bit_count++;
                    if (read_bit == 0){
                        if (rover != NULL && rover->left != NULL){
                            rover = rover->left;
                        }
                    } else if (read_bit == 1){
                        if (rover != NULL && rover->right != NULL){
                            rover = rover->right;
                        }
                    }
                    if (rover != NULL && ((rover->right == NULL) || (rover->left == NULL))){
                        fwrite(&rover->entry, 1, 1, p->output_file->f_ptr);
                        p->output_file->filesize++;
                        rover = p->input_file->bst->root;
                        if (ret_flag == true){
                            return;
                        }
                    }
                    if (bit_count >= ((p->input_file->filesize-4)-1024)*8-bits_to_skip){
                        ret_flag = true;
                        return;
                    }
                }
            }
        }      
    }
}


void codec_file(params_t *p){
    ll_create(p);
    ll_sort_asc(p->input_file->freq_ll);
    bst_build(p);
    while (p->input_file->bst->branches > 1){
        bst_combine_nodes(p);
        bst_update_nodes(p);
        bst_resort(p);
    }
    p->input_file->bst->root = bst_get_root_node(p->input_file->bst->first);
    bst_populate_output_buffer_length(p);
    write_output_file(p);
}


int main (int argc, char *argv[]){
    params_t *parameters = (params_t*)malloc(sizeof(params_t));

    clock_t start, finish;
    parse_commands(argc, argv, parameters);
    open_files(parameters);
    read_input_file(parameters);
    start = clock();
    codec_file(parameters);
    finish = clock();
    
    printf("Time elapsed:        %g ms\n", 1000*((double)(finish-start))/CLOCKS_PER_SEC);
    printf("Input filesize:      %d bytes\n", parameters->input_file->filesize);
    printf("Output filesize:     %d bytes\n", parameters->output_file->filesize);
    printf("Done\n");

	return 0;
}
