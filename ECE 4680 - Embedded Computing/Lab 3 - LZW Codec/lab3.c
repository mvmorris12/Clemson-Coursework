/*  Lab 3 - LZW Compression / Decompression
 *  Michael Morris
 *  mvmorri
 *  ECE 4680
 *  2/20/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define TOTAL_DICT_ENTRIES 65535

typedef struct file_info_t{
    char *filename;
    unsigned int filesize;
    FILE *f_ptr;
    void *buffer;
} file_info;


typedef struct params_t{
    unsigned char compress_flag, decompress_flag;
    file_info *input_file, *output_file;
} params;


typedef struct dict_t{
    unsigned char entry[TOTAL_DICT_ENTRIES][1000];
    unsigned short entry_length[TOTAL_DICT_ENTRIES];
    unsigned short code_idx[TOTAL_DICT_ENTRIES];
    unsigned int entry_cnt;
    unsigned char codec;
} dict;


/* Fx Prototypes */
void parse_commands(int argc, char *argv[], params *p);
unsigned char open_files(params *p);
void read_input_file(params *p);
unsigned short concat(unsigned char *txtP, unsigned short lenP, unsigned char* txtC, unsigned short lenC, unsigned char *txtPC);
void compress_file(params *p, dict *d);
void decompress_file(params *p, dict *d);
char dict_pattern_search(dict *d, unsigned char *text_to_search, unsigned short length, unsigned int *hash_idx);
char dict_code_search(dict *d, unsigned short code_to_search, unsigned short *length, unsigned int *hash_idx);
void dict_add(dict *d, unsigned char *text_to_add, unsigned short length);
dict* initialize_dict(unsigned char codec);
unsigned int hash(unsigned char *label, int length);

/* DJB (mod?) Hash function */
unsigned int hash(unsigned char *label, int length){
    unsigned int i, h=0;
    for (i=0; i<length; i++){
        h = ((h+label[i])*17)%(TOTAL_DICT_ENTRIES);
    }
    
    return h;
}

/* parses command line arguments and allocates space for variables */
void parse_commands(int argc, char *argv[], params *p){
    int c;

    /* tell user how to use program if incorrect arguments */
    if (argc < 2){
        printf("Usage:  lab2 [input filename] [-c] [-d] [-o : filename]\n");
        printf("\t-c = compress file\n");
        printf("\t-d = decompress file\n");
        printf("\t-o = output filename\n");
        exit(EXIT_FAILURE);
    }

    /* allocate space for objects */
    p->input_file = (file_info*)malloc(sizeof(file_info));
    p->output_file = (file_info*)malloc(sizeof(file_info));
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


unsigned char open_files(params *p){
    /* open file for reading */
    p->input_file->f_ptr=fopen(p->input_file->filename,"rb");
    if (p->input_file->f_ptr == NULL){
        printf("Unable to open %s for writing\n",p->input_file->filename);
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
        printf("Unable to open %s for writing\n",p->output_file->filename);
        fclose(p->input_file->f_ptr);
        exit(EXIT_FAILURE);
    }

    return 1;
}


/* read input file into buffer */
void read_input_file(params *p){
    if (p->compress_flag){
        p->input_file->buffer = (unsigned char*)malloc(sizeof(unsigned char)*p->input_file->filesize);
        fread((unsigned char*)p->input_file->buffer, 1, p->input_file->filesize, p->input_file->f_ptr);
    } else {
        p->input_file->buffer = (unsigned short*)malloc(sizeof(unsigned short)*p->input_file->filesize);
        fread((unsigned short*)p->input_file->buffer, 2, p->input_file->filesize, p->input_file->f_ptr);
    }
    fclose(p->input_file->f_ptr);
}

/* concatenates 2 unsigned char arrays (*txtP, *txtC) into *txtPC */
unsigned short concat(unsigned char *txtP, unsigned short lenP, unsigned char* txtC, unsigned short lenC, unsigned char *txtPC){ 
    if (lenP){
        memcpy(txtPC, txtP, lenP);
        memcpy(txtPC+lenP, txtC, lenC);
        return (lenP+lenC);
    } else {
        *txtPC = *txtC;
        return 1;
    }
}


void compress_file(params *p, dict *d){
    unsigned char   c_val[1], p_val[1000], pc_val[2000];
    int             i;
    unsigned short  c_len, p_len, pc_len;
    unsigned int    hash_idx;

    p_len = pc_len = hash_idx = 0;
    c_len = 1;
    p_val[0] = 0;

    /* iterate through input file and add symbols to output file */
    for (i=0; i<p->input_file->filesize; i++){
        c_val[0] = ((unsigned char*)p->input_file->buffer)[i];
        pc_len = concat(p_val, p_len, c_val, c_len, pc_val);
        /* if PC already exists in dictionary.. */
        if (dict_pattern_search(d, pc_val, pc_len, &hash_idx)){
            memcpy(p_val, pc_val, pc_len);
            p_len = pc_len;
        /* add symbol to output file */
        } else {
            if(dict_pattern_search(d, p_val, p_len, &hash_idx) == 0){
                printf("Error\nAlgorithm failure, exiting..\n");
                exit(EXIT_FAILURE);
            }
            fwrite(&d->code_idx[hash_idx], 1, sizeof(unsigned short), p->output_file->f_ptr);
            p->output_file->filesize+=2;
            dict_add(d, pc_val, pc_len);
            memset(p_val, 0x00 , 1000);
            memcpy(p_val, c_val, c_len);
            
            p_len = c_len;
            c_len = 1;
        }
    }
    dict_pattern_search(d, p_val, p_len, &hash_idx);
    fwrite(&d->code_idx[hash_idx], 1, sizeof(unsigned short), p->output_file->f_ptr);
    p->output_file->filesize+=2;
    fclose(p->output_file->f_ptr);
}


void decompress_file(params *p, dict *d){
    unsigned char   x_val[1000], y_val[1000], xy_val[2000], z_val[1000], xz_val[2000];
    int             i;
    unsigned short  c_val, p_val, c_len, p_len, x_len, y_len, xy_len, z_len, xz_len;
    unsigned int    hash_idx;

    p_len = x_len = y_len = xy_len = z_len = xz_len = hash_idx = p_val = 0;
    c_len = 1;
    x_val[0] = y_val[0] = 0;
    c_val = ((unsigned short*)p->input_file->buffer)[0];

    /* write first pattern to file */
    dict_code_search(d, c_val, &c_len, &hash_idx);
    fwrite(d->entry[hash_idx], c_len, sizeof(unsigned char), p->output_file->f_ptr);
    p->output_file->filesize += c_len;
    
    /* iterate through input file, add remaining patterns to output file */
    for (i=c_len; i<(p->input_file->filesize/2); i++){
        p_val = c_val;
        p_len = d->entry_length[c_val];
        c_val = ((unsigned short*)p->input_file->buffer)[i];
        /* if C already exists in dictionary, add X+Y to output file */
        if (dict_code_search(d, c_val, &c_len, &hash_idx)){
            fwrite(d->entry[c_val], d->entry_length[c_val], sizeof(unsigned char), p->output_file->f_ptr);
            p->output_file->filesize += d->entry_length[c_val];
            memcpy(x_val, d->entry[p_val], d->entry_length[p_val]);
            x_len = p_len;
            y_val[0] = d->entry[c_val][0];
            y_len = 1;
            xy_len = concat(x_val, x_len, y_val, y_len, xy_val);
            dict_add(d, xy_val, xy_len);
        /* if not, add X+Z to output file */
        } else {
            dict_code_search(d, p_val, &p_len, &hash_idx);
            memcpy(x_val, d->entry[hash_idx], p_len);
            x_len = d->entry_length[p_val];
            z_val[0] = d->entry[p_val][0];
            z_len = 1;
            xz_len = concat(x_val, x_len, z_val, z_len, xz_val);
            fwrite(xz_val, xz_len, sizeof(unsigned char), p->output_file->f_ptr);
            p->output_file->filesize += xz_len;
            dict_add(d, xz_val, xz_len);
        }
    }    
    fclose(p->output_file->f_ptr);
}

/* adds entry into dictionary */
void dict_add(dict *d, unsigned char *text_to_add, unsigned short length){
    /* hash text_to_add into index value */
    unsigned int hash_idx = hash(text_to_add, length);
    unsigned int hash_idx_start = hash_idx;

    /* compression dictionary add */
    if (d->codec == 0){
        /* collision detection */
        while (d->entry_length[hash_idx]){
            hash_idx++;
            /* reached end of table, go back to 0 index */
            if (hash_idx >= TOTAL_DICT_ENTRIES){
                hash_idx = 0;
            }
            /* made it back to starting index, array is full */
            if (hash_idx_start == hash_idx){
                printf("Error: table is too small, exiting..\n");
                exit(EXIT_FAILURE);
            }
        }
        memcpy(d->entry[hash_idx], text_to_add, length);
        d->entry_length[hash_idx] = length;
        d->code_idx[hash_idx] = d->entry_cnt;
        d->entry_cnt++;  
    /* decompression dictionary add - no hashing necessary */
    } else {
        memcpy(d->entry[d->entry_cnt], text_to_add, length);
        d->entry_length[d->entry_cnt] = length;
        d->code_idx[d->entry_cnt] = d->entry_cnt;
        d->entry_cnt++;
    }
}

/* searches dictionary for entry - for compression */
char dict_pattern_search(dict *d, unsigned char *text_to_search, unsigned short length, unsigned int *h_idx){
    unsigned int hash_idx = hash(text_to_search, length);
    unsigned int hash_idx_start = hash_idx;
    
    /* check all possible hash positions, factoring in collisions */
    while ((memcmp(d->entry[hash_idx], text_to_search, length) != 0) || (d->entry_length[hash_idx] != length)){
        if (hash_idx >= TOTAL_DICT_ENTRIES){
                hash_idx = 0;
        } else if (d->entry_length[hash_idx] == 0){
            return 0;
        } else {
            hash_idx++;
        }
        if (hash_idx_start == hash_idx){
            return 0;
        }
    }

    *h_idx = hash_idx;
    return 1;
}

/* checks if a dictionary index is populated - for decompression */
char dict_code_search(dict *d, unsigned short code_to_search, unsigned short *length, unsigned int *h_idx){
    if (d->entry_length[code_to_search] != 0){
        *h_idx = code_to_search;
        *length = d->entry_length[code_to_search];
        return 1;
    }

    return 0;
}

/* initialize dictionary with entries 0 to 255 */
dict* initialize_dict(unsigned char codec){
    int i;
    unsigned char pattern;
    dict *dict_ret = (dict*)malloc(sizeof(dict));
    dict_ret->entry_cnt = 0;
    dict_ret->codec = codec;

    for (i=0; i<TOTAL_DICT_ENTRIES; i++){
        dict_ret->entry_length[i] = 0;
        memset(dict_ret->entry[i], 0x00, 1000);   
    }

    for (i=0; i<256; i++){
        pattern = i;
        dict_add(dict_ret, &pattern, 1);
    }

    return dict_ret;
}


int main (int argc, char *argv[]){
    params *parameters = (params*)malloc(sizeof(params));
    dict *dictionary;

    clock_t start, end;
    parse_commands(argc, argv, parameters);
    open_files(parameters);
    read_input_file(parameters);

    start = clock();
    if (parameters->compress_flag){
        dictionary = initialize_dict(0);
        compress_file(parameters, dictionary);
    } else if (parameters->decompress_flag){
        dictionary = initialize_dict(1);
        decompress_file(parameters, dictionary);
    }
    end = clock();
    
    printf("Time elapsed:        %g ms\n", 1000*((double)(end-start))/CLOCKS_PER_SEC);
    printf("Input filesize:      %d bytes\n", parameters->input_file->filesize);
    printf("Output filesize:     %d bytes\n", parameters->output_file->filesize);
    printf("Dictionary entries:  %d\n", dictionary->entry_cnt);
    printf("Done\n");

	return 0;
}









