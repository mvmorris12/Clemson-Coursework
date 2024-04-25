#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define PI              3.14159265358979323846
#define ROWS_IMG_OUT    256
#define COLS_IMG_OUT    256
#define NEAR_ZERO_VAL   0.0001

typedef struct vertices{
    float x,y,z;
} vert_t;


typedef struct bounding_box{
    vert_t *min, *max, *center, *left, *right, *top, *bottom, *topleft;
    float E, a;
} bb_t;


typedef struct file_info_t{
    char *filename;
    unsigned int filesize;
    FILE *f_ptr;
    void *buffer;
} file_info_t;


typedef struct img{
    vert_t *vertices;
    unsigned int vertices_total;
    unsigned short **faces;
    unsigned int faces_total;
    bb_t *bounding_box;
    vert_t *camera;
    vert_t *camera_init;
    vert_t *up;
    vert_t *pixels[ROWS_IMG_OUT][COLS_IMG_OUT];
    float zbuffer[ROWS_IMG_OUT][COLS_IMG_OUT];
} img_t;


typedef struct params{
    file_info_t  *input_file, *output_file;
    img_t *img;
} params_t;



/* Fx Prototypes */
void parse_commands(int argc, char *argv[], params_t *p);
unsigned char open_files(params_t*p);
void read_input_file(params_t*p);
void rotate_camera(vert_t *v, vert_t *a);
void move_camera(img_t *img);


void cross_v(vert_t *v0, vert_t *v1, vert_t *vf);
float dot_v(vert_t *v0, vert_t *v1);
vert_t subtract_v(vert_t *v0, vert_t *v1);
vert_t add_v(vert_t *v0, float v0_mult, vert_t *v1, float v1_mult);


/* parses command line arguments and allocates space for variables */
void parse_commands(int argc, char *argv[], params_t*p){
    // int c;
    char *p1;

    /* tell user how to use program if incorrect arguments */
    if (argc < 5){
        printf("Usage:  lab5  [-i : input filename] [-o : output filename] [-c : #X #Y #Z]\n");
        printf("\t-c = camera orientation (x,y,z)\n");
        exit(EXIT_FAILURE);
    }

    /* allocate space for objects */
    p->input_file = (file_info_t*)malloc(sizeof(file_info_t));
    p->output_file = (file_info_t*)malloc(sizeof(file_info_t));
    p->input_file->filename = p->output_file->filename = NULL;
  
    for (int i=0; i<argc; i++){
        if (strcmp(argv[i], "-i") == 0){
            p->input_file->filename = (char*)malloc((strlen(argv[i+1])+1)*sizeof(char)); 
            strcpy(p->input_file->filename, argv[i+1]);
        }
        if (strcmp(argv[i], "-o") == 0){
            p->output_file->filename = (char*)malloc((strlen(argv[i+1])+1)*sizeof(char));
            strcpy(p->output_file->filename, argv[i+1]);
        }
        if (strcmp(argv[i], "-c") == 0){
            p->img = (img_t*)calloc(1, sizeof(img_t));
            p->img->camera = (vert_t*)malloc(sizeof(vert_t));
            p->img->camera_init = (vert_t*)malloc(sizeof(vert_t));
            p->img->camera->x = 1;
            p->img->camera->y = 0;
            p->img->camera->z = 0;
            p->img->up = (vert_t*)calloc(1, sizeof(vert_t));
            p->img->up->x = 0;
            p->img->up->y = 0;
            p->img->up->z = 1;
            p->img->camera_init->x = strtof(argv[i+1], &p1);
            p->img->camera_init->y = strtof(argv[i+2], &p1);
            p->img->camera_init->z = strtof(argv[i+3], &p1);
        }
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
    p->output_file->buffer = (unsigned char*)calloc(ROWS_IMG_OUT*COLS_IMG_OUT, sizeof(unsigned char));
    
    return 1;
}


/* read input file into buffer */
void read_input_file(params_t*p){
    char s1[60], s2[20], s3[20];
    int dummy;
    do{
        fgets(s1, 40, p->input_file->f_ptr);
    } while (strstr(s1, "vertex") == 0);
    sscanf(s1, "%s %s %d\n", s2, s3, &p->img->vertices_total);
    p->img->vertices = (vert_t*)calloc(p->img->vertices_total, sizeof(vert_t));
    do{
        fgets(s1, 40, p->input_file->f_ptr);
    } while (strstr(s1, "face") == 0);
    sscanf(s1, "%s %s %d\n", s2, s3, &p->img->faces_total);
    p->img->faces = (unsigned short**)malloc(sizeof(unsigned short*)*p->img->faces_total);

    do{
        fgets(s1, 40, p->input_file->f_ptr);
    } while (strstr(s1, "end_header") == 0);

    for (int i=0; i<p->img->vertices_total; i++){
        fscanf(p->input_file->f_ptr, "%f %f %f\n", &p->img->vertices[i].x, &p->img->vertices[i].y, &p->img->vertices[i].z);
    }

    for (int i=0; i<p->img->faces_total; i++){
        p->img->faces[i] = (unsigned short*)malloc(sizeof(unsigned short)*3);
        fscanf(p->input_file->f_ptr, "%d %hd %hd %hd\n", &dummy, &p->img->faces[i][0], &p->img->faces[i][1], &p->img->faces[i][2]);
    }
    
    fclose(p->input_file->f_ptr);
}


void write_output_file(params_t *p){
    char header[40];

    sprintf(header, "%s %d %d %d ", "P5", COLS_IMG_OUT, ROWS_IMG_OUT, 255);
    fprintf(p->output_file->f_ptr, "%s", header);
    p->output_file->filesize += strlen(header) + ROWS_IMG_OUT*COLS_IMG_OUT;
    fwrite((unsigned char*)p->output_file->buffer, ROWS_IMG_OUT*COLS_IMG_OUT, sizeof(unsigned char), p->output_file->f_ptr);
    fclose(p->output_file->f_ptr);   
}


void calculate_bounding_box(params_t *p){
    p->img->bounding_box = (bb_t*)malloc(sizeof(bb_t));
    p->img->bounding_box->min = (vert_t*)calloc(1, sizeof(vert_t));
    p->img->bounding_box->max = (vert_t*)calloc(1, sizeof(vert_t));
    p->img->bounding_box->min->x = __FLT_MAX__;
    p->img->bounding_box->min->y = __FLT_MAX__;
    p->img->bounding_box->min->z = __FLT_MAX__;
    for (int i=0; i<p->img->vertices_total; i++){
        if (p->img->vertices[i].x < p->img->bounding_box->min->x){
            p->img->bounding_box->min->x = p->img->vertices[i].x;
        }
        if (p->img->vertices[i].y < p->img->bounding_box->min->y){
            p->img->bounding_box->min->y = p->img->vertices[i].y;
        }
        if (p->img->vertices[i].z < p->img->bounding_box->min->z){
            p->img->bounding_box->min->z = p->img->vertices[i].z;
        }
        if (p->img->vertices[i].x > p->img->bounding_box->max->x){
            p->img->bounding_box->max->x = p->img->vertices[i].x;
        }
        if (p->img->vertices[i].y > p->img->bounding_box->max->y){
            p->img->bounding_box->max->y = p->img->vertices[i].y;
        }
        if (p->img->vertices[i].z > p->img->bounding_box->max->z){
            p->img->bounding_box->max->z = p->img->vertices[i].z;
        }
    }

    p->img->bounding_box->E = p->img->bounding_box->max->x - p->img->bounding_box->min->x;
    if (p->img->bounding_box->max->y - p->img->bounding_box->min->y > p->img->bounding_box->E){
        p->img->bounding_box->E = p->img->bounding_box->max->y - p->img->bounding_box->min->y;
    }
    if (p->img->bounding_box->max->z - p->img->bounding_box->min->z > p->img->bounding_box->E){
        p->img->bounding_box->E = p->img->bounding_box->max->z - p->img->bounding_box->min->z;
    }

    p->img->bounding_box->center->x = (p->img->bounding_box->min->x + p->img->bounding_box->max->x)/2;
    p->img->bounding_box->center->y = (p->img->bounding_box->min->y + p->img->bounding_box->max->y)/2;
    p->img->bounding_box->center->z = (p->img->bounding_box->min->z + p->img->bounding_box->max->z)/2;

    for (int r=0; r<ROWS_IMG_OUT; r++){
        for (int c=0; c<COLS_IMG_OUT; c++){
            p->img->zbuffer[r][c] = __FLT_MAX__;
        }
    }
}


void rotate_camera(vert_t *v, vert_t *a){
    float rad_a,rad_b,rad_c;
    float tmp_x,tmp_y,tmp_z;
    
    rad_c = a->x*PI/180*(-1);
    rad_a = a->y*PI/180*(-1);
    rad_b = a->z*PI/180*(-1);

    tmp_x = v->x*(cos(rad_a)*cos(rad_b)) + 
            v->y*(sin(rad_a)*cos(rad_b)) +
            v->z*((-1)*sin(rad_b));

    tmp_y = v->x*(cos(rad_a)*sin(rad_b)*sin(rad_c)-sin(rad_a)*cos(rad_c)) +
            v->y*(sin(rad_a)*sin(rad_b)*sin(rad_c)+cos(rad_a)*cos(rad_c)) +
            v->z*(cos(rad_b)*sin(rad_c));
            
    tmp_z = v->x*(cos(rad_a)*sin(rad_b)*cos(rad_c)+sin(rad_a)*sin(rad_c)) +
            v->y*(sin(rad_a)*sin(rad_b)*cos(rad_c)-cos(rad_a)*sin(rad_c)) +
            v->z*(cos(rad_b)*cos(rad_c));

    v->x = tmp_x;
    v->y = tmp_y;
    v->z = tmp_z;    
}


void move_camera(img_t *img){
    *img->camera = add_v(img->camera, 1.5*img->bounding_box->E, img->bounding_box->center, 1);
}


void calculate_3d_coords(img_t *img){
    vert_t vsubt;

    img->bounding_box->left = (vert_t*)calloc(1, sizeof(vert_t));
    img->bounding_box->right = (vert_t*)calloc(1, sizeof(vert_t));
    img->bounding_box->top = (vert_t*)calloc(1, sizeof(vert_t));
    img->bounding_box->bottom = (vert_t*)calloc(1, sizeof(vert_t));
    img->bounding_box->topleft = (vert_t*)calloc(1, sizeof(vert_t));

    vsubt = subtract_v(img->bounding_box->center, img->camera);
    cross_v(img->up, &vsubt, img->bounding_box->left);
    img->bounding_box->a = sqrt((img->bounding_box->left->x*img->bounding_box->left->x)+(img->bounding_box->left->y*img->bounding_box->left->y)+(img->bounding_box->left->z*img->bounding_box->left->z));   
    *img->bounding_box->left = add_v(img->bounding_box->left, img->bounding_box->E/(2*img->bounding_box->a), img->bounding_box->center, 1);
    cross_v(&vsubt, img->up, img->bounding_box->right);
    *img->bounding_box->right = add_v(img->bounding_box->right, img->bounding_box->E/(2*img->bounding_box->a), img->bounding_box->center, 1);
    *img->bounding_box->top = add_v(img->up, img->bounding_box->E/2, img->bounding_box->center, 1);
    *img->bounding_box->bottom = add_v(img->up, (-1)*img->bounding_box->E/2, img->bounding_box->center, 1);
    *img->bounding_box->topleft = add_v(img->up, img->bounding_box->E/2, img->bounding_box->left, 1);
}


void calculate_img_pixels(params_t *p){
    unsigned short c, r;
    for (r=0; r<ROWS_IMG_OUT; r++){
        for (c=0; c<COLS_IMG_OUT; c++){
            p->img->pixels[r][c] = (vert_t*)malloc(sizeof(vert_t));

            p->img->pixels[r][c]->x = p->img->bounding_box->topleft->x + 
                (((float)c/(COLS_IMG_OUT-1))*(p->img->bounding_box->right->x - p->img->bounding_box->left->x)) +
                (((float)r/(ROWS_IMG_OUT-1))*(p->img->bounding_box->bottom->x - p->img->bounding_box->top->x));

            p->img->pixels[r][c]->y = p->img->bounding_box->topleft->y + 
                (((float)c/(COLS_IMG_OUT-1))*(p->img->bounding_box->right->y - p->img->bounding_box->left->y)) +
                (((float)r/(ROWS_IMG_OUT-1))*(p->img->bounding_box->bottom->y - p->img->bounding_box->top->y));

            p->img->pixels[r][c]->z = p->img->bounding_box->topleft->z + 
                (((float)c/(COLS_IMG_OUT-1))*(p->img->bounding_box->right->z - p->img->bounding_box->left->z)) +
                (((float)r/(ROWS_IMG_OUT-1))*(p->img->bounding_box->bottom->z - p->img->bounding_box->top->z));
        }
    }
}


void calculate_plane_eqn(params_t *p){
    int idx, r, c;
    idx = r = c = 0;
    vert_t vs1, vs2, vs3, ABC, intersect;
    vert_t v2_0, v1_0, v0_1, v2_1, v1_2, v0_2, vi_0, vi_1, vi_2;
    vert_t cross_d1_0, cross_d1_1, cross_d2_0, cross_d2_1, cross_d3_0, cross_d3_1; 
    float n, d, D, dot1, dot2, dot3;

    for (r=0; r<ROWS_IMG_OUT; r++){
        for (c=0; c<COLS_IMG_OUT; c++){
            ((unsigned char*)p->output_file->buffer)[(r*ROWS_IMG_OUT)+c] = 0;
            for (idx=0; idx<p->img->faces_total; idx++){
                vs1 = subtract_v(&p->img->vertices[p->img->faces[idx][1]], &p->img->vertices[p->img->faces[idx][0]]);
                vs2 = subtract_v(&p->img->vertices[p->img->faces[idx][2]], &p->img->vertices[p->img->faces[idx][0]]);
                cross_v(&vs1, &vs2, &ABC);
                D = (-1.0)*dot_v(&ABC, &p->img->vertices[p->img->faces[idx][0]]);
                n = (-1.0)*dot_v(&ABC, p->img->camera) - D;
                vs3 = subtract_v(p->img->pixels[r][c], p->img->camera);
                d = dot_v(&ABC, &vs3);
                if (d > NEAR_ZERO_VAL){
                    intersect = add_v(p->img->camera, 1, &vs3, (n/d));
                    v0_1 = subtract_v(&p->img->vertices[p->img->faces[idx][0]], &p->img->vertices[p->img->faces[idx][1]]);
                    v0_2 = subtract_v(&p->img->vertices[p->img->faces[idx][0]], &p->img->vertices[p->img->faces[idx][2]]);
                    v1_0 = subtract_v(&p->img->vertices[p->img->faces[idx][1]], &p->img->vertices[p->img->faces[idx][0]]);
                    v1_2 = subtract_v(&p->img->vertices[p->img->faces[idx][1]], &p->img->vertices[p->img->faces[idx][2]]);
                    v2_0 = subtract_v(&p->img->vertices[p->img->faces[idx][2]], &p->img->vertices[p->img->faces[idx][0]]);
                    v2_1 = subtract_v(&p->img->vertices[p->img->faces[idx][2]], &p->img->vertices[p->img->faces[idx][1]]);
                    vi_0 = subtract_v(&intersect, &p->img->vertices[p->img->faces[idx][0]]);
                    vi_1 = subtract_v(&intersect, &p->img->vertices[p->img->faces[idx][1]]);
                    vi_2 = subtract_v(&intersect, &p->img->vertices[p->img->faces[idx][2]]);
                    cross_v(&v2_0, &v1_0, &cross_d1_0);
                    cross_v(&vi_0, &v1_0, &cross_d1_1);
                    cross_v(&v0_1, &v2_1, &cross_d2_0);
                    cross_v(&vi_1, &v2_1, &cross_d2_1);
                    cross_v(&v1_2, &v0_2, &cross_d3_0);
                    cross_v(&vi_2, &v0_2, &cross_d3_1);
                    dot1 = dot_v(&cross_d1_0, &cross_d1_1);
                    dot2 = dot_v(&cross_d2_0, &cross_d2_1);
                    dot3 = dot_v(&cross_d3_0, &cross_d3_1);
                    if ((dot1 > 0) && (dot2 > 0) && (dot3 > 0)){
                        if  (p->img->zbuffer[r][c] > (n/d)){
                            p->img->zbuffer[r][c] = n/d;
                            ((unsigned char*)p->output_file->buffer)[(r*ROWS_IMG_OUT)+c] = 155 + (idx % 100);
                        }
                    }
                }
            }
        }
    }
}



vert_t add_v(vert_t *v0, float v0_mult, vert_t *v1, float v1_mult){
    vert_t ret_v;
    ret_v.x = (v0->x*v0_mult)+(v1->x*v1_mult);
    ret_v.y = (v0->y*v0_mult)+(v1->y*v1_mult);
    ret_v.z = (v0->z*v0_mult)+(v1->z*v1_mult);
    return ret_v;
}


vert_t subtract_v(vert_t *v0, vert_t *v1){
    vert_t ret_v;
    ret_v.x = v0->x - v1->x;
    ret_v.y = v0->y - v1->y;
    ret_v.z = v0->z - v1->z;
    return ret_v;
}


void cross_v(vert_t *v0, vert_t *v1, vert_t *vf){
    vf->x = ((v0->y*v1->z)-(v0->z*v1->y));
    vf->y = ((v0->z*v1->x)-(v0->x*v1->z)); 
    vf->z = ((v0->x*v1->y)-(v0->y*v1->x));     
}


float dot_v(vert_t *v0, vert_t *v1){
    return ((v0->x*v1->x) + (v0->y*v1->y) + (v0->z*v1->z));
}


int main (int argc, char *argv[]){
    params_t *parameters = (params_t*)malloc(sizeof(params_t));

    clock_t start, finish;

    start = clock();
    parse_commands(argc, argv, parameters);
    open_files(parameters);
    read_input_file(parameters);
    calculate_bounding_box(parameters);
    rotate_camera(parameters->img->camera, parameters->img->camera_init);
    rotate_camera(parameters->img->up, parameters->img->camera_init);
    move_camera(parameters->img);
    calculate_3d_coords(parameters->img);
    calculate_img_pixels(parameters);
    calculate_plane_eqn(parameters);
    write_output_file(parameters);
    
    finish = clock();
    
    printf("Time elapsed:        %g ms\n", 1000*((double)(finish-start))/CLOCKS_PER_SEC);
    printf("Input filesize:      %d bytes\n", parameters->input_file->filesize);
    printf("Output filesize:     %d bytes\n", parameters->output_file->filesize);
    printf("Done\n");

	return 0;
}
