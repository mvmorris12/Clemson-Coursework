/*  Lab 2 - RLE Encoding / Decoding
 *  Michael Morris
 *  mvmorri
 *  ECE 4680
 *  2/8/2024
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

typedef struct file_info_t{
    char *filename;
    int filesize;
    FILE *f_ptr;
    unsigned char *buffer;
} file_info;

typedef struct params_t{
    unsigned char compress_flag, decompress_flag;
    file_info *input_file, *output_file;
} params;

// void parse_commands(int argc, char *argv[], char *c_fl, char *dc_fl, char **input, char **output){
void parse_commands(int argc, char *argv[], params *p){
    int c;

    /* tell user how to use program if incorrect arguments */
    if (argc < 2){
        printf("Usage:  lab2 [input filename] [-c] [-d] [-o : filename]\n");
        printf("\t-c = compress file\n");
        printf("\t-d = decompress file\n");
        printf("\t-o = output filename\n");
        exit(1);
    }

    /* allocate space for objects */
    p->input_file = (file_info*)malloc(sizeof(file_info));
    p->output_file = (file_info*)malloc(sizeof(file_info));
    p->compress_flag = p->decompress_flag = false;
    p->input_file->filename = p->output_file->filename = NULL;
  
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
                exit(1);
        }
    }

    if (p->compress_flag == p->decompress_flag){
        printf("Error\nMust select EITHER compress or decompress\n");
        exit(1);
    }

    if (p->input_file->filename == NULL || p->output_file->filename == NULL){
        printf("Error\nMust provide BOTH input and output filenames\n");
        exit(1);
    }
}


unsigned char open_files(params *p){
    /* open file for reading */
    p->input_file->f_ptr=fopen(p->input_file->filename,"rb");
    if (p->input_file->f_ptr == NULL){
        printf("Unable to open %s for writing\n",p->input_file->filename);
        return 0;
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
        return 0;
    }

    return 1;
}

void read_input_file(params *p){
    
    p->input_file->buffer = (unsigned char*)calloc(p->input_file->filesize,sizeof(unsigned char));
    
    /* read input file into buffer */
    fread(p->input_file->buffer, sizeof(unsigned char), p->input_file->filesize, p->input_file->f_ptr);
    fclose(p->input_file->f_ptr);
}


void compress_file(params *p){
    unsigned char   val_cur, val_prev, runcount;
    int             i;
    runcount = 1;

    val_prev = p->input_file->buffer[0];

    /* iterate through buffer to find repeating characters */
    for (i=1; i<=p->input_file->filesize; i++){
        val_cur = p->input_file->buffer[i];
        if (val_cur == val_prev){
            if (runcount == 255){
                p->output_file->filesize += 2;
                p->output_file->buffer = (unsigned char*)realloc(p->output_file->buffer, p->output_file->filesize*sizeof(unsigned char));
                p->output_file->buffer[p->output_file->filesize-2] = runcount;
                p->output_file->buffer[p->output_file->filesize-1] = val_prev;
                
                runcount = 1;
                val_prev = val_cur;
            } else {
                runcount++;
            }
        } else {
            p->output_file->filesize += 2;
            p->output_file->buffer = (unsigned char*)realloc(p->output_file->buffer, p->output_file->filesize*sizeof(unsigned char));
            p->output_file->buffer[p->output_file->filesize-2] = runcount;
            p->output_file->buffer[p->output_file->filesize-1] = val_prev;    
            
            runcount = 1;
            val_prev = val_cur;
        }
    }

    fwrite(p->output_file->buffer, p->output_file->filesize, sizeof(unsigned char), p->output_file->f_ptr);
    fclose(p->output_file->f_ptr);
}


void decompress_file(params *p){
    unsigned char   val_char, val_cnt;
    int             i,j;

    /* decode the input file buffer */
    for (i=0; i<=p->input_file->filesize; i+=2){
        val_cnt = p->input_file->buffer[i];
        val_char = p->input_file->buffer[i+1];
        p->output_file->filesize += val_cnt;
        p->output_file->buffer = (unsigned char*)realloc(p->output_file->buffer, p->output_file->filesize*sizeof(unsigned char));
        for (j=val_cnt; j>0; j--){
            p->output_file->buffer[p->output_file->filesize-j] = val_char;
        }
    }

    fwrite(p->output_file->buffer, p->output_file->filesize, sizeof(unsigned char), p->output_file->f_ptr);
    fclose(p->output_file->f_ptr);
}


int main (int argc, char *argv[]){
    params *parameters = (params*)malloc(sizeof(params));
      
    parse_commands(argc, argv, parameters);
    if (!open_files(parameters)){
        exit(0);
    }
    read_input_file(parameters);

    if (parameters->compress_flag){
        compress_file(parameters);
    } else if (parameters->decompress_flag){
        decompress_file(parameters);
    }

    printf("input filesize:  %d bytes\n", parameters->input_file->filesize);
    printf("output filesize: %d bytes\n", parameters->output_file->filesize);
    printf("Done\n");
	return 0;
}