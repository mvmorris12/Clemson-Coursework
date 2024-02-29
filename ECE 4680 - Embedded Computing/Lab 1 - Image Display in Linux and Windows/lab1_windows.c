#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

LRESULT CALLBACK EventProcessor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return(DefWindowProc(hWnd, uMsg, wParam, lParam));
}

uint8_t *convert_8b_to_colormap_gray(FILE *fpt, int ROWS, int COLS){
	uint8_t *image, *image_gray_buffer;
	int	    i;
	image = (uint8_t*)calloc(ROWS * COLS, sizeof(uint8_t));
	image_gray_buffer = (uint8_t*)calloc(ROWS * COLS *2, sizeof(uint8_t));
	fread(image_gray_buffer, sizeof(uint8_t), ROWS * COLS, fpt);

	return image_gray_buffer;
}


uint8_t* convert_24b_to_16b_rgb(FILE* fpt, int ROWS, int COLS) {
	uint8_t *image, *image_rgb_buffer;
	int     i;

	image = (uint8_t*)calloc(ROWS * COLS * 3, 1);
	image_rgb_buffer = (uint8_t*)calloc(ROWS * COLS * 2, 1);
	fread(image, 1, ROWS * COLS * 3, fpt);

	/* 24 bit -> 16 bit RGB conversion (Windows X-5-5-5) */
	for (i = 0; i < ROWS * COLS; i++) {
		image_rgb_buffer[i*2+1] = ((image[i*3+0] & 0xF8) >> 1) | ((image[i*3+1]) >> 6);
		image_rgb_buffer[i*2+0] = ((image[i*3+1] & 0x38) << 2) | ((image[i*3+2] & 0xF8) >> 2);
	}
	return image_rgb_buffer;
}


uint8_t *open_image(char* filepath, int *ROWS, int *COLS, uint8_t *isCM) {
	FILE	*fpt;
	errno_t err;
	uint8_t	*image_disp_buffer;
	int		i,BYTES;
	char	header[80];
	if ((err = fopen_s(&fpt, filepath, "rb")) != 0) {
		MessageBox(NULL, "Image failed to open", "Error", MB_OK | MB_APPLMODAL);
		return NULL;
	}
	else {
		i = fscanf(fpt, "%s %d %d %d", header, COLS, ROWS, &BYTES);
		fseek(fpt, 1, SEEK_CUR);

		if (i != 4 || BYTES != 255) {
			MessageBox(NULL, "Image header is incorrectly formatted, exiting.", "Error", MB_OK | MB_APPLMODAL);
			fclose(fpt);
			exit(0);
		}
		else if (strcmp(header, "P5") == 0) {
			image_disp_buffer = convert_8b_to_colormap_gray(fpt, *ROWS, *COLS);
			*isCM = TRUE;
			fclose(fpt);
			return image_disp_buffer;
		}
		else if (strcmp(header, "P6") == 0) {
			image_disp_buffer = convert_24b_to_16b_rgb(fpt, *ROWS, *COLS);
			*isCM = FALSE;
			fclose(fpt);
			return image_disp_buffer;
		}
		else {
			MessageBox(NULL, "Image header P# value not handled by this program, exiting.", "Error", MB_OK | MB_APPLMODAL);
			exit(0);
		}
	}
}


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,int nCmdShow){
	WNDCLASS	wc;
	HWND		WindowHandle;
	int			ROWS, COLS, i;
	uint8_t		*displaydata, isColormap;
	BITMAPINFO	*bm_info;
	HDC			hDC;

	wc.style=CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc=(WNDPROC)EventProcessor;
	wc.cbClsExtra=wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=wc.lpszMenuName=NULL;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName="Image Window Class";

	if (RegisterClass(&wc) == 0)
	  exit(0);

	if (__argc == 2) {		
		displaydata = open_image(__argv[1], &ROWS, &COLS, &isColormap);
	}
	else {
		MessageBox(NULL, "Usage:  lab1 [filename]", "Syntax Error", MB_OK | MB_APPLMODAL);
		exit(0);
	}

	WindowHandle=CreateWindow("Image Window Class","ECE468 Lab1",
							  WS_OVERLAPPEDWINDOW,
							  10,10,COLS+16,ROWS+38,
							  NULL,NULL,hInstance,NULL);
	if (WindowHandle == NULL){
		MessageBox(NULL,"No window","Try again",MB_OK | MB_APPLMODAL);
		exit(0);
	}
	ShowWindow (WindowHandle, SW_SHOWNORMAL);

	bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
	hDC=GetDC(WindowHandle);

	/* ... set up bmiHeader field of bm_info ... */
	bm_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bm_info->bmiHeader.biWidth = COLS;
	bm_info->bmiHeader.biHeight = -ROWS;
	bm_info->bmiHeader.biPlanes = 1;
	bm_info->bmiHeader.biBitCount = 16;
	bm_info->bmiHeader.biClrUsed = 0;
	bm_info->bmiHeader.biClrImportant = 0;
	bm_info->bmiHeader.biCompression = BI_RGB;
	bm_info->bmiHeader.biSizeImage = 0;
	bm_info->bmiHeader.biXPelsPerMeter = 0;
	bm_info->bmiHeader.biYPelsPerMeter = 0;

	if (isColormap){
		for (i=0; i<256; i++){
			bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
			bm_info->bmiColors[i].rgbReserved=0;
		} 
		bm_info->bmiHeader.biBitCount = 8;
	} else { 
		bm_info->bmiHeader.biBitCount = 16;
	}

	SetDIBitsToDevice(hDC,0,0,COLS,ROWS,0,0,
				  0, /* first scan line */
				  ROWS, /* number of scan lines */
				  displaydata,bm_info,DIB_RGB_COLORS);
	ReleaseDC(WindowHandle,hDC);
	free(bm_info);
	MessageBox(NULL,"Press OK to continue","",MB_OK | MB_APPLMODAL);
}