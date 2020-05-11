#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dc_image.h"
#include<time.h>

//#define CANNY_THRESH 170
//#define CANNY_BLUR   3



#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )
#define SIGN(x)   ( (x) <= 0 ? (-1): (1) )

int main()
{
	for (int z = 0; z<4; z++) 
	{
		int rows, cols, chan;
		int keep_thresh = 5;
		int CANNY_THRESH;
		int CANNY_BLUR;
		byte ***img;
		if  (z==0) {
			CANNY_THRESH=45;
			CANNY_BLUR = 10;
			keep_thresh = 5;
			img = LoadRgb("pentagon.png", &rows, &cols, &chan);
			printf("img %p rows %d cols %d chan %d\n", img, rows, cols, chan);
			printf("1 %d\n",img[0][0][0]);
			printf("1 %d\n",img[0][0][0]);
		}
		else if (z==1) {
			CANNY_THRESH=120;
			CANNY_BLUR =2;
			img = LoadRgb("sidewalk.png", &rows, &cols, &chan);
			printf("img %p rows %d cols %d chan %d\n", img, rows, cols, chan);
		}
		else if (z==2) {
			CANNY_THRESH=100;
			CANNY_BLUR= 6;
			keep_thresh = 5;
			img = LoadRgb("puppy.png", &rows, &cols, &chan);
			printf("img %p rows %d cols %d chan %d\n", img, rows, cols, chan);
		}
		else{
			CANNY_THRESH=100;
			CANNY_BLUR =2;
			img = LoadRgb("building.png", &rows, &cols, &chan);
			printf("img %p rows %d cols %d chan %d\n", img, rows, cols, chan);
		}

		int y,x;
		
		
		

		//-----------------
		// Read the image    [y][x][c]   y number rows   x cols  c 3
		//-----------------
		

		SaveRgbPng(img, "out/1_img.png", rows, cols);
		
		//-----------------
		// Convert to Grayscale
		//-----------------
		byte **gray = malloc2d(rows, cols);
		for (y=0; y<rows; y++){
			for (x=0; x<cols; x++) {
				int r = img[y][x][0];
				int g = img[y][x][1];
				int b = img[y][x][2];
				gray[y][x] =  (r+g+b) / 3;
			}
		}
		
		SaveGrayPng(gray, "out/2_gray.png", rows, cols);

		//-----------------
		// Box Blur   ToDo: Gaussian Blur is better
		//-----------------
		
		// Box blur is separable, so separately blur x and y
		int k_x=CANNY_BLUR, k_y=CANNY_BLUR;
		
		// blur in the x dimension
		byte **blurx = (byte**)malloc2d(rows, cols);
		for (y=0; y<rows; y++) {
			for (x=0; x<cols; x++) {
				
				// Start and end to blur
				int minx = x-k_x/2;      // k_x/2 left of pixel
				int maxx = minx + k_x;   // k_x/2 right of pixel
				minx = MAX(minx, 0);     // keep in bounds
				maxx = MIN(maxx, cols);
				
				// average blur it
				int x2;
				int total = 0;
				int count = 0;
				for (x2=minx; x2<maxx; x2++) {
					total += gray[y][x2];    // use "gray" as input
					count++;
				}
				blurx[y][x] = total / count; // blurx is output
			}
		}
		
		SaveGrayPng(blurx, "out/3_blur_just_x.png", rows, cols);
		
		// blur in the y dimension
		byte **blur = (byte**)malloc2d(rows, cols);
		for (y=0; y<rows; y++) {
			for (x=0; x<cols; x++) {
				
				// Start and end to blur
				int miny = y-k_y/2;      // k_x/2 left of pixel
				int maxy = miny + k_y;   // k_x/2 right of pixel
				miny = MAX(miny, 0);     // keep in bounds
				maxy = MIN(maxy, rows);
				
				// average blur it
				int y2;
				int total = 0;
				int count = 0;
				for (y2=miny; y2<maxy; y2++) {
					total += blurx[y2][x];    // use blurx as input
					count++;
				}
				blur[y][x] = total / count;   // blur is output
			}
		}
		
		SaveGrayPng(blur, "out/3_blur.png", rows, cols);
		
		
		//-----------------
		// Take the "Sobel" (magnitude of derivative)
		//  (Actually we'll make up something similar)
		//-----------------
		
		byte **sobel = (byte**)malloc2d(rows, cols);
		
		for (y=0; y<rows; y++) {
			for (x=0; x<cols; x++) {
				int mag=0;
				
				if (y>0)      mag += ABS(blur[y-1][x] - blur[y][x]);
				if (x>0)      mag += ABS(blur[y][x-1] - blur[y][x]);
				if (y<rows-1) mag += ABS(blur[y+1][x] - blur[y][x]);
				if (x<cols-1) mag += ABS(blur[y][x+1] - blur[y][x]);
				
				sobel[y][x] = 3*mag;
			}
		}
		
		SaveGrayPng(sobel, "out/4_sobel.png", rows, cols);
		
		//-----------------
		// Non-max suppression
		//-----------------
		byte **nonmax = malloc2d(rows, cols);    // note: *this* initializes to zero!
		if (z==0) {
			for (y=1; y<rows-1; y++)
			{
				for (x=1; x<cols-1; x++)
				{
					// Is it a local maximum
					int is_y_max = (sobel[y][x] > sobel[y-1][x] && sobel[y][x]>=sobel[y+1][x]);
					int is_x_max = (sobel[y][x] > sobel[y][x-1] && sobel[y][x]>=sobel[y][x+1]);
					if (is_y_max || is_x_max)
						nonmax[y][x] = 255;
					else
						nonmax[y][x] = 0;
				}
			}
		}
		else {
			nonmax = sobel;
		}
		SaveGrayPng(nonmax, "out/5_nonmax.png", rows, cols);
		
		//-----------------
		// Final Threshold
		//-----------------
		byte **edges = malloc2d(rows, cols);    // note: *this* initializes to zero!
		
		for (y=0; y<rows; y++) {
			for (x=0; x<cols; x++) {
				if (nonmax[y][x] > CANNY_THRESH)
					edges[y][x] = 255;
				else
					edges[y][x] = 0;
			}
		}
		
		SaveGrayPng(edges, "out/6_edges.png", rows, cols);
		
		//------------------
		//Apply RANSAC
		//------------------
		
		// Iterations

		int iter = 20000;
		int best_count = 0;
		int best_p[4];
		
		int count_li[iter][5];
		srand(time(0));
		byte ***lines = malloc3d(rows, cols, chan);
		
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<cols;j++)
			{
				lines[i][j][0] = edges[i][j];
			}
		}
		
		for (int i=0; i<iter; i++) {
			
			
			int x1 = rand()%(rows-20)+10;
			int y1 = rand()%(cols-20)+10;
			int x2=x1,y2=y1;
			while (x2==x1) {
			
			x2 = rand()%(rows-20)+10;
			}
			
			while (y2==y1) {
			
			y2 = rand()%(cols-20)+10;
			}
			
			int count = 0;
			//printf("P1 %d %d\n",x1, y1);
			//printf("P2 %d %d\n",x2, y2);
			int dy = y2 - y1;
			int dx = x2 - x1;
			int ym1, ym2, xm1, xm2;
			int margin = 5;
			//printf("slope %d %d %d %d\n",x1, y1, x2, y2);
			 
			//printf("%d %d\n",dy, dx);
			
			// check for larger slope
			
			if (ABS(dy) > ABS(dx)) {

				//printf("YESSSSS\n");
				// count overlapping
				int keeps[cols];
				int iter_keeps = cols;
				
				for (int w = 0; w<iter_keeps; w++) {
					keeps[w]=0;
				}
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
						//printf("%d %d %d %d\n", mcount, y, xm1, xm2 );
						//x = x1 + (y - y1) * dx / dy;
						//printf("%d %d %d\n", xm1, xm2, y);
						
						if (lines[xm1][y][0]==255) {
							//lines[xm1][y][0]=0;
							//lines[xm1][y][1]=255;
							count += 1;
							keeps[y] += 1;
						}
						if (lines[xm2][y][0]==255) {
							//lines[xm2][y][0]=0;
							//lines[xm2][y][1]=255;
							count += 1;
							keeps[y] += 1;
						}
						
					
					}
					if (leave ==1) {
						break;
					}
				}
				
				/*int best_count_nonzero = 0, count_nonzero = 0, count_zero = 0, count_weight = 0;
				int new_x1 = 0, new_y1=0, new_x2=0, new_y2 =0;
				int start_again = 1;
				int start = 0, end = 0;
				int b_start = 0, b_end = 0;
				for (int w = 0; w<iter_keeps; w++) {
					if (keeps[w]<=1) {
						count_zero += 1;
					}
					else {
					if (start_again==1) {
						start = w;
						start_again = 0;
					}
					count_weight += keeps[w];
					count_zero=0;
					}
					
					if (count_zero >keep_thresh) {
						end = w-keep_thresh;
						if (end-start>best_count_nonzero) {
							best_count_nonzero = end-start;
							b_start = start;
							b_end = end;
						}
						start_again = 1;
						count_weight = 0;
						count_zero = 0;
						
					}
					
					
				
				}*/
				//count = count_weight;
				//if (b_end-b_start>0) {
				//	if (y2 > y1) {
				//	y1= b_start;
				//	y2 = b_end;
				//	}
				//	else {
				//	y2= b_start;
				//	y1 = b_end;	
				//	}
				//	//printf(" start end %d %d  \n", b_start, b_end);
				//}
			}
			
			else {
				int keeps[rows];
				
				int iter_keeps = rows;
				for (int w = 0; w<iter_keeps; w++) {
				keeps[w]=0;
				}
				
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
						if (lines[x][ym1][0]==255) {
							//lines[x][ym1][0]=0;
							//lines[x][ym1][1]=255;
							count += 1;
							keeps[x] += 1;
						}
						if (lines[x][ym2][0]==255) {
							//lines[x][ym2][0]=0;
							//lines[x][ym2][1]=255;
							count += 1;
							keeps[x] += 1;
						}
					}
					if (leave ==1) {
						break;
					}
					
				}
				
				/*int best_count_nonzero = 0, count_nonzero = 0, count_zero = 0, count_weight = 0, b_count_weight = 0;
				int new_x1 = 0, new_y1=0, new_x2=0, new_y2 =0;
				int start_again = 1;
				int start = 0, end = 0;
				int b_start = 0, b_end = 0;
				for (int w = 0; w<iter_keeps; w++) {
					if (keeps[w]<=1) {
						count_zero += 1;
					}
					else {
					if (start_again==1) {
						start = w;
						start_again = 0;
					}
					count_weight += keeps[w];
					count_zero=0;
					}
					
					if (count_zero >keep_thresh) {
						end = w-keep_thresh;
						if (end-start>best_count_nonzero) {
							best_count_nonzero = end-start;
							b_start = start;
							b_end = end;
							b_count_weight = count_weight;
						}
						start_again = 1;
						count_weight = 0;
						count_zero = 0;
						
					}
					
					
				
				}
				//count = b_count_weight;
				//if (b_end-b_start>0) {
				//	if (x1 > x2) {
				//	x2= b_start;
				//	x1 = b_end;
				//	}
				//	else {
				//	x1= b_start;
				//	x2 = b_end;
				//	}
					//printf(" start end %d %d  \n", b_start, b_end);
				//}*/
			}	
			
			
			//
			if (count>=best_count){
				
				best_count = count;
				best_p[0] = x1;
				best_p[1] = y1;
				best_p[2] = x2;
				best_p[3] = y2;
			}
			count_li[i][0] = count;
			count_li[i][1] = x1;
			count_li[i][2] = y1;
			count_li[i][3] = x2;
			count_li[i][4] = y2;
			//printf("count %d\n", count);
		}
		printf("%d %d %d %d", best_p[0],best_p[1],best_p[2],best_p[3]);

		

		

		//Sorting
		int row_=iter,col_=5;
		int i,j,k=0,x3,temp;
		for(i=0;i<row_;i++)
		{
				for(j=i+1;j<row_;j++)
				{
						if(count_li[i][0] > count_li[j][0])
						{
							for(x3=0;x3<col_;x3++) {
								temp=count_li[i][x3];
								count_li[i][x3]=count_li[j][x3];
								count_li[j][x3]=temp;
								}
						}
				}
		}
		
		int top_k = 5;
		
		
		
		int slopes[iter][2], count1=0;
		for(i=row_-1;i>=0;i--)
		{
			int x1 = count_li[i][1];
			int x2 = count_li[i][3];
			int y1 = count_li[i][2];
			int y2 = count_li[i][4];
			int dy = count_li[i][4] - count_li[i][2];
			int dx = count_li[i][3] - count_li[i][1];
			float m = dy/dx;
			int add_slope = 1;
			int x,y, mp, xm1, xm2, ym1, ym2;
			int margin = 5;
			int count = 0;
			
			//printf("start count %d  \n", count_li[i][0]);
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

						if (lines[xm1][y][0]==255) {
							count += 1;
						}
						if (lines[xm2][y][0]==255) {
							count += 1;
						}
					}
					if (leave ==1) {
						break;
					}
				}
			}
			
			else {

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

						if (lines[x][ym1][0]==255) {
							count += 1;
						}
						if (lines[x][ym2][0]==255) {
							count += 1;
						}
					}
					if (leave ==1) {
						break;
					}
				}
			}
			//printf("count1 %d %d %d \n", count, count_li[i][0], count_li[i-1][0]);
			
	//		for(int k=0; k<count1; k++) {
	//			if ( ABS(count_li[i][2] - slopes[k][1])<10 ){
	//				if (ABS(m-slopes[k][0])>1) {
	//				
	//				add_slope = 0;
	//				break;
	//				}
	//			}
	//		}


			if (count>=.9*count_li[i][0]) {
				slopes[count1][0] = m;
				slopes[count1][1] = count_li[i][2];
				count1 += 1;
				printf("%d %d %d %d %f %d %d\n", count_li[i][1], count_li[i][2], count_li[i][3], count_li[i][4], m, count_li[i][2], count_li[i][0]);
				DrawLines(lines, count_li[i][1], count_li[i][2], count_li[i][3], count_li[i][4], rows, cols);

				byte ***img1 = LoadRgb("out/7_edges.png", &rows, &cols, &chan);
				***lines = ***img1;
			}
			if (count1 == top_k) {
				break;
			}


			
		}
		//printf("top 5\n");
		if  (z==0) {

			SaveRgbPng(lines, "out/out_petagon.png", rows, cols);
		}
		else if (z==1) {

			SaveRgbPng(lines, "out/out_sidewalk.png", rows, cols);
		}
		else if (z==2) {

			SaveRgbPng(lines, "out/out_puppy.png", rows, cols);
			
		}
		else if (z==3) {

			SaveRgbPng(lines, "out/out_building.png", rows, cols);
		}
		//SaveRgbPng(lines, "out/7_edges.png", rows, cols);
		//printf("Done!\n");
	}
	return 0;
}

/*

	printf("load image\n");
	byte *data = stbi_load("puppy.jpg", &cols, &rows, &chan, 4);

	printf("data = %p\n", data);
	int rt=stbi_write_png("output.png", cols, rows, 4, data, cols*4);
*/