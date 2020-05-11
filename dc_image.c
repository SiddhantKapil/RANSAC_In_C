#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include "stb_image.h"
#include "stb_image_write.h"

#include "dc_image.h"

#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )
#define SIGN(x)   ( (x) <= 0 ? (-1): (1) )
//-------------------------------------------------------
// A few image helper functions
//-------------------------------------------------------

// Memory Allocation

byte **malloc2d(int rows, int cols) {
	int y;
	byte **ptr = (byte**)malloc(rows*sizeof(byte*));
	for (y=0; y<rows; y++)
		ptr[y] = (byte*)calloc(cols,sizeof(byte));
	return ptr;
}

byte ***malloc3d(int rows, int cols, int chan) {
	int y,x;
	byte ***ptr = (byte***)malloc(rows*sizeof(byte**));
	for (y=0; y<rows; y++) {
		ptr[y] = (byte**)malloc(cols*sizeof(byte*));
		for (x=0; x<cols; x++)
			ptr[y][x] = (byte*)calloc(chan,sizeof(byte));
	}
	return ptr;
}

void free2d(byte **data, int rows)
{
	int y;
	for (y=0; y<rows; y++)
		free(data[y]);
	free(data);
}

void free3d(byte ***data, int rows, int cols)
{
	int y,x;
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++)
			free(data[y][x]);
		free(data[y]);
	}
	free(data);
}


// Loading / Saving Images

void SaveRgbPng(byte ***in, const char *fname, int rows, int cols)
{
	printf("SaveRgbaPng %s %d %d\n", fname, rows, cols);
	
	int y,x,c,i=0;
	byte *data = (byte*)malloc(cols*rows*3*sizeof(byte));
	for (y=0; y<rows; y++) {
		//printf("y %d rows %d cols %d\n", y, rows, cols);
		for (x=0; x<cols; x++) {
			for (c=0; c<3; c++) {
				data[i++] = in[y][x][c];
			}
		}
	}
	stbi_write_png(fname, cols, rows, 3, data, cols*3);
	free(data);
}

void SaveGrayPng(byte **in, const char *fname, int rows, int cols)
{
	printf("SaveGrayPng %s %d %d\n", fname, rows, cols);
	
	int y,x,c,i=0;
	byte *data = malloc(cols*rows*3*sizeof(byte));
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			data[i++] = in[y][x];   // red
			data[i++] = in[y][x];   // green
			data[i++] = in[y][x];   // blue
//			data[i++] = 255;        // alpha
		}
	}
	stbi_write_png(fname, cols, rows, 3, data, cols*3);
	free(data);
}


void DrawLines(byte ***img, int x1, int y1, int x2, int y2, int rows, int cols)
{
	
//	x1 = &x1;
//	x2 = &x2;
	//y1 = &y1;
	//y2 = &y2;
	int dy = y2 - y1;
	int dx = x2 - x1;
	int x,y, mp, xm1, xm2, ym1, ym2;
	int margin = 5;
	//int temp_d = 0;
	//byte **edges = malloc2d(rows, cols);
	// check for larger slope
	img[xm1][ym1][0] = 0;
	img[xm1][ym1][1] = 255;
	img[xm1][ym1][2] = 255;
	img[xm2][ym2][0] = 0;
	img[xm2][ym2][1] = 255;
	img[xm2][ym2][2] = 255;
	if (ABS(dy) > ABS(dx)) {

			for (y=2*margin; y < cols-2*margin; y += 1) {
				//printf("%d\n",y);
				int leave = 0;
				for (int mcount = 0; mcount<margin; mcount++) {
					xm1 = x1 +mcount + (y - y1) * dx / dy;
					xm2 = x1 -mcount + (y - y1) * dx / dy;
					if ((xm1>rows-2*margin) || (xm2>rows-2*margin) || (xm1<2*margin) || (xm2<2*margin)) {
						leave = 1;
						break;
					}
					//x = x1 + (y - y1) * dx / dy;
					//img[xm1][y][2] = 255;
					//img[xm2][y][2] = 255;
					if (img[xm1][y][0]==255) {
						img[xm1][y][0] = 0;
						img[xm1][y][1] = 150;
						img[xm1][y][2] = 255;
					}
					if (img[xm2][y][0]==255) {
						img[xm2][y][0] = 0;
						img[xm2][y][1] = 150;
						img[xm2][y][2] = 255;
					}
				}
				if (leave ==1) {
					break;
				}
			}
		}
		
		else {

			//for (x=x1; x != x2; x += SIGN(dx)) {
			for (x=2*margin; x < rows-2*margin; x += 1) {	
				int leave = 0;
				for (int mcount = 0; mcount<margin; mcount++) {
					ym1 = y1+mcount + (x - x1) * dy / dx;
					ym2 = y1-mcount + (x - x1) * dy / dx;
					if ((ym1>cols-2*margin) || (ym2>cols-2*margin)|| (ym1<2*margin) || (ym2<2*margin)) {
						leave = 1;
						break;
					}					
					//y = y1 + (x - x1) * dy / dx;
					//img[x][ym1][2] = 255;
					//img[x][ym2][2] = 255;
					if (img[x][ym1][0]==255) {
						img[x][ym1][0] = 0;
						img[x][ym1][1] = 150;
						img[x][ym1][2] = 255;
					}
					if (img[x][ym2][0]==255) {
						img[x][ym2][0] = 0;
						img[x][ym2][1] = 150;
						img[x][ym2][2] = 255;
					}
				}
				if (leave ==1) {
					break;
				}
			}
		}
	
	SaveRgbPng(img, "out/7_edges.png", rows, cols);
}
byte ***LoadRgb(const char *fname, int *rows, int *cols, int *chan)
{
	printf("LoadRgba %s\n", fname);
	
	int y,x,c,i=0;	
	byte *data = stbi_load(fname, cols, rows, chan, 3);
	/*
	// Convert rgb to rgba
	if (*chan==3) {
		int N = *rows * *cols;
		printf("N %d\n", N);
		byte *rgb = data;
		data = malloc(N * 4 * sizeof(byte));
		for (i=0; i<N; i++) {
			//printf("i %d\n", i);
			data[4*i+0] = rgb[3*i+0];
			data[4*i+1] = rgb[3*i+1];
			data[4*i+2] = rgb[3*i+2];
			data[4*i+3] = 255;
		}
		free(rgb);
		printf("done convert\n");
	}
	*chan = 4;
	*/
	if (*chan != 3) {
		printf("error: expected 3 channels (red green blue)\n");
		exit(1);
	}
	
	i=0;
	byte ***img = malloc3d(*rows,*cols,*chan);
	for (y=0; y<*rows; y++)
		for (x=0; x<*cols; x++)
			for (c=0; c<*chan; c++)
				img[y][x][c] = data[i++];
	free(data);
	printf("done read\n");
	return img;
}

byte **LoadGray(const char *fname, int *rows, int *cols)
{
	printf("LoadRgba %s\n", fname);
	
	int y,x,c=1,i=0;	

	byte *data = stbi_load(fname, cols, rows,&c, 3);

	
	i=0;
	byte **img = malloc2d(*rows,*cols);
	for (y=0; y<*rows; y++)
		for (x=0; x<*cols; x++)
			img[y][x] = data[i++];
	free(data);
	printf("done read\n");
	return img;
}

