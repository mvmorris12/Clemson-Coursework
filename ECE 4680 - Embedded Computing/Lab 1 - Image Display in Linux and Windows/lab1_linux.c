#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


uint8_t* convert_8b_to_16b_gray(FILE *fpt, int ROWS, int COLS){
    uint8_t *image, *image_gray_buffer;
    int	    i;

    image = (uint8_t*)calloc(ROWS*COLS, sizeof(uint8_t));
    image_gray_buffer = (uint8_t*)calloc(ROWS*COLS*2, sizeof(uint8_t));
    fread(image, sizeof(uint8_t), ROWS*COLS, fpt);

    /* 8 bit -> 16 bit grayscale conversion (Linux 5-6-5) */
    for (i=0; i<ROWS*COLS; i++){
        image_gray_buffer[i*2+0] = ((image[i] & 0xF8) | (image[i] >> 5));
        image_gray_buffer[i*2+1] = ((image[i] & 0x1C) << 3) | (image[i] >> 3);
    }
    return image_gray_buffer;
}


uint8_t* convert_24b_to_16b_rgb(FILE *fpt, int ROWS, int COLS){
    uint8_t *image, *image_rgb_buffer;
    int     i;

    image = (uint8_t*)calloc(ROWS*COLS*3, sizeof(uint8_t));
    image_rgb_buffer = (uint8_t*)calloc(ROWS*COLS*6, sizeof(uint8_t));
    fread(image, sizeof(uint8_t), ROWS*COLS*3, fpt);

    /* 24 bit -> 16 bit grayscale conversion (Linux 5-6-5) */
    for (i=0; i<ROWS*COLS; i++){
        // image_rgb_buffer[i*6+0] = ((image[i*3+0] & 0xF8) | (image[i*3+1] >> 5));
        // image_rgb_buffer[i*6+1] = ((image[i*3+1] & 0x1C) << 3) | (image[i*3+2] >> 3);

        image_rgb_buffer[i*6+0] = ((image[i*3+0] & 0xF8));
        // image_rgb_buffer[i*6+1] = ((image[i*3+1] & 0x1C) << 3) | (image[i*3+2] >> 3);
        image_rgb_buffer[i*6+2] = ((image[i*3+1] & 0xF8));// | (image[i*3+1] >> 5));
        // image_rgb_buffer[i*6+3] = ((image[i*3+1] & 0x1C) << 3) | (image[i*3+2] >> 3);
        image_rgb_buffer[i*6+4] = ((image[i*3+2] & 0xF8));// | (image[i*3+1] >> 5));
        // image_rgb_buffer[i*6+5] = ((image[i*3+1] & 0x1C) << 3) | (image[i*3+2] >> 3);
        // printf("%d\t%d %d %d  %X  %X\n", i, image[i*3+0], image[i*3+1], image[i*3+2], image_rgb_buffer[i*2+0], image_rgb_buffer[i*2+1]);
    }
    return image_rgb_buffer;
}


void display_image(uint8_t *img_buffer, int ROWS, int COLS){
	GC				ImageGC;
	XImage			*Picture;
	Display 		*Monitor;
	Window 			Window;
	XEvent 			Event;
	int 			Screen;

	Monitor = XOpenDisplay(NULL);
	if (Monitor == NULL) {
        printf("Unable to open graphics display\n");
        exit(0);
	}

	Screen = DefaultScreen(Monitor);
	// printf("Rows: %d\nCols: %d\n", ROWS, COLS);
	Window = XCreateSimpleWindow(Monitor, RootWindow(Monitor, Screen), 10, 10, COLS, ROWS, 1,
			BlackPixel(Monitor, Screen), WhitePixel(Monitor, Screen));

	ImageGC=XCreateGC(Monitor,Window,0,NULL);
	XSelectInput(Monitor, Window, ExposureMask | KeyPressMask);
	XMapWindow(Monitor, Window);
	Picture=XCreateImage(Monitor,DefaultVisual(Monitor,Screen),
		DefaultDepth(Monitor,Screen),
		ZPixmap,	/* format */
		0,			/* offset */
		img_buffer,	/* the data */
		COLS,ROWS,	/* size of the image data */
		16,			/* pixel quantum (8, 16 or 32) */
		0);			/* bytes per line (0 causes compute) */

	while (1) {
		XNextEvent(Monitor, &Event);
		if (Event.type == Expose) {
			XPutImage(Monitor,Window,ImageGC,Picture,0,0,0,0,COLS,ROWS);

			XFlush(Monitor);
		}
		if (Event.type == KeyPress)
			break;
	}

	XCloseDisplay(Monitor);
}


int main (int argc, char *argv[]){
    FILE          	*fpt;
    uint8_t        	*image_disp_buffer;
    char		    header[80];
    int		        i,p,ROWS,COLS,BYTES;

    /* tell user how to use program if incorrect arguments */
    if (argc != 2){
        printf("Usage:  lab1 [filename]\n");
        exit(0);
    }


    /* open image for reading */
    fpt=fopen(argv[1],"r");
    if (fpt == NULL){
        printf("Unable to open %s for writing\n",argv[1]);
        exit(0);
    }

    /* read image header */
    i=fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
    fseek(fpt, 1, SEEK_CUR);
    
    if (i != 4 || BYTES != 255){
        printf("Image header is incorrectly formatted, exiting.\n");
        fclose(fpt);
        exit(0);
    } else if (strcmp(header,"P5") == 0){
        // printf("doing gray\n");
        image_disp_buffer = convert_8b_to_16b_gray(fpt, ROWS, COLS);
        fclose(fpt);
        fpt=fopen("bridge2.ppm","wb");
        fprintf(fpt, "%s %d %d %d ", "P5", COLS, ROWS, 65535);
        fwrite(image_disp_buffer, ROWS*COLS, sizeof(image_disp_buffer[0])*2, fpt);
        fclose(fpt);        
    } else if (strcmp(header,"P6") == 0){
        // printf("doing rgb\n");
        image_disp_buffer = convert_24b_to_16b_rgb(fpt, ROWS, COLS);
        fclose(fpt);
        fpt=fopen("retina2.ppm","wb");
        fprintf(fpt, "%s %d %d %d ", "P6", COLS, ROWS, 65535);
        fwrite(image_disp_buffer, ROWS*COLS, sizeof(image_disp_buffer[0]*2), fpt);
        fclose(fpt);
    } else {
        printf("Image header P# value not handled by this program, exiting.\n");
        fclose(fpt);
        exit(0);
    }f
	
	display_image(image_disp_buffer, ROWS, COLS);

	return 0;
}