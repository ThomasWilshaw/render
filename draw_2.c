/*
*work out draw structure
*Add test to draw to see if pixel is inside the image
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "matrix.h"
#include "stack.h"
#include "objects.h"

#define PASS 999999
#define SCALE 100

struct int2 {
    int x;
	int y;
}int2;

struct double4 {
    double x;
	double y;
	double z;
	double w;
}double4;

typedef struct {
	int **image;
	int x;
	int y;
}Image;

void drawPoint(Image *i, int x, int y, int col);
void drawWuLine (Image *i, short X0, short Y0, short X1, short Y1,
         short BaseColor, short NumLevels, unsigned short IntensityBits);
void drawQuad(Image *i, int colour, struct int2 a, struct int2 b, struct int2 c, struct int2 d);
void drawTri(Image *i, int colour, struct int2 a, struct int2 b, struct int2 c);
void createCanvas(Image *i, FILE *f, int colour, int width, int height);
void writeImage(FILE *f, Image *i);
void matrixTests();
void drawObject(Image *i, int colour, Object *o, Matrix c);
void objectInit(Object *o, int vertnum, int polynum);
void drawFromFile(Image *i, FILE *save, FILE *f);
int comConvert(char * com);
int shapeConvert(char * com);

int main(int argc, char* argv[])
{
	if(argc > 1){
		if(strcmp(argv[1], "-t") == 0){
			matrixTests();
			return 1;
		}
	}

	/*Create image*/
	FILE *f = fopen("image.ppm", "w");
	Image image;
	createCanvas(&image, f, 255, 1024, 1024);
	
	/*Create translation stack*/
	stackT transformStack;
	stackInit(&transformStack, 10);
	
	/*Draw stuff*/
	if(argc == 3){
		if(strcmp(argv[1], "-f") == 0){
			char name[80];
			strcpy(name, argv[2]);
			FILE *file = fopen(name, "r");
			if(file == NULL){
				printf("Error, could not open file: %s\n", name);
				exit(EXIT_FAILURE);
			}
			drawFromFile(&image, f, file);
			fclose(file);
			return 1;
		}
	}
	
	Object square;
	square = new_square();
	Object plane;
	plane = new_plane();
	Object cube;
	cube = new_cube();
	Object oct;
	oct = new_oct();
	
	Matrix C;
	C = new_matrix(4, 4);
	init_identity_matrix(&C);
	C = pers(C, 15, 6.8, 25);
	//C = ortho(C, 10);
	C = tran(C, 0, 2, 20);
	C = rot(C, 70, 1);
	C = rot(C, 48, 3);
	C = scal(C, 1, 1, -1);
	C = scal(C, 2, 2, 2);
	stackPush(&transformStack, C);
	drawObject(&image, 0, &square, C);
	C = tran(C, 2, 0, 0);
	drawObject(&image, 0, &plane, C);
	C = stackPop(&transformStack);
	stackPush(&transformStack, C);
	C = tran(C, 0, 2, 0);
	drawObject(&image, 0, &plane, C);
	C = stackPop(&transformStack);
	stackPush(&transformStack, C);
	C = rot(C, 20, 3);
	C = tran(C, 0, 0, 1);
	drawObject(&image, 0, &cube, C);
	C = stackPop(&transformStack);
	C = tran(C, 5, 0, 0);
	C = rot(C, -15, 3);
	drawObject(&image, 0, &oct, C);
	
	
	
	/*Close image and save*/
	writeImage(f, &image);
	return 1;
}

void matrixTests()
{
	Matrix r;
	r = new_matrix(4, 4);
	double data[16] = {0.9239, 0, 0, 0, 0, 0.9239, 0, 0, 0, 0, 0.8064, 0.3827, 0, 0, -4.99, 0};
	r.t = data;
	//print_matrix(r);
	
	Matrix vert;
	vert = new_matrix(4, 4);
	double data2[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,-1.41,9,1};
	vert.t = data2;
	//print_matrix(vert);
	
	Matrix result = new_matrix(4, 4);
	result = matrix_mult(vert, r);
	print_matrix(result);
	
	Matrix i = new_matrix(4, 4);
	init_identity_matrix(&i);
	init_identity_matrix(&i);
	i = pers(i, 45, 6.2, 11.8);
	i = tran(i, 0, -1.41, 9);
	//i = rot(i, -80, 1);
	print_matrix(i);
}

void createCanvas(Image *i, FILE *f, int colour, int width, int height)
{
	int x, y;
	i->x = width;
	i->y = height;
	//FILE *f = fopen("file.ppm", "w");
	if (f== NULL) {
	    printf("Error opening\n");
		exit(EXIT_FAILURE);
	}
	i->image = (int **)malloc(width * sizeof(int));
	for (x=0; x<width; x++)
	    i->image[x] = (int *)malloc(height * sizeof(int));
		
	for (x=0; x<width; x++){
	    for (y=0; y<height; y++)
		    i->image[x][y] = colour;
	}
	
	fprintf(f, "P2\n%d %d\n256\n", width, height);
}

void writeImage(FILE *f, Image *i) //closes image
{
	int x, y;
	for (x=0; x<(i->x); x++){
	    for (y=0; y<(i->y); y++)
		    fprintf(f, "%d ", i->image[x][y]);
		fprintf(f, "\n");
	}
	printf("SAVING...\n");
	fclose(f);
	//free(image);
}

void drawTri(Image *i, int colour, struct int2 a, struct int2 b, struct int2 c)
{
    drawWuLine(i, a.y, a.x, b.y, b.x, colour, 256, 8);
	drawWuLine(i, b.y, b.x, c.y, c.x, colour, 256, 8);
	drawWuLine(i, c.y, c.x, a.y, a.x, colour, 256, 8);
}

void drawQuad(Image *i, int colour, struct int2 a, struct int2 b, struct int2 c, struct int2 d)	
{
	drawWuLine(i, a.y, a.x, b.y, b.x, colour, 256, 8);
	drawWuLine(i, b.y, b.x, c.y, c.x, colour, 256, 8);
	drawWuLine(i, c.y, c.x, d.y, d.x, colour, 256, 8);
    drawWuLine(i, d.y, d.x, a.y, a.x, colour, 256, 8);
}

void drawWuLine (Image *i, short X0, short Y0, short X1, short Y1,
         short BaseColor, short NumLevels, unsigned short IntensityBits)
{
   unsigned short IntensityShift, ErrorAdj, ErrorAcc;
   unsigned short ErrorAccTemp, Weighting, WeightingComplementMask;
   short DeltaX, DeltaY, Temp, XDir;

   /* Make sure the line runs top to bottom */
   if (Y0 > Y1) {
      Temp = Y0; Y0 = Y1; Y1 = Temp;
      Temp = X0; X0 = X1; X1 = Temp;
   }
   /* Draw the initial pixel, which is always exactly intersected by
      the line and so needs no weighting */
   drawPoint(i,X0, Y0, BaseColor);

   if ((DeltaX = X1 - X0) >= 0) {
      XDir = 1;
   } else {
      XDir = -1;
      DeltaX = -DeltaX; /* make DeltaX positive */
   }
   /* Special-case horizontal, vertical, and diagonal lines, which
      require no weighting because they go right through the center of
      every pixel */
   if ((DeltaY = Y1 - Y0) == 0) {
      /* Horizontal line */
      while (DeltaX-- != 0) {
         X0 += XDir;
         drawPoint(i,X0, Y0, BaseColor);
      }
      return;
   }
   if (DeltaX == 0) {
      /* Vertical line */
      do {
         Y0++;
         drawPoint(i,X0, Y0, BaseColor);
      } while (--DeltaY != 0);
      return;
   }
   if (DeltaX == DeltaY) {
      /* Diagonal line */
      do {
         X0 += XDir;
         Y0++;
         drawPoint(i,X0, Y0, BaseColor);
      } while (--DeltaY != 0);
      return;
   }
   /* Line is not horizontal, diagonal, or vertical */
   ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
   /* # of bits by which to shift ErrorAcc to get intensity level */
   IntensityShift = 16 - IntensityBits;
   /* Mask used to flip all bits in an intensity weighting, producing the
      result (1 - intensity weighting) */
   WeightingComplementMask = NumLevels - 1;
   /* Is this an X-major or Y-major line? */
   if (DeltaY > DeltaX) {
      /* Y-major line; calculate 16-bit fixed-point fractional part of a
         pixel that X advances each time Y advances 1 pixel, truncating the
         result so that we won't overrun the endpoint along the X axis */
      ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;
      /* Draw all pixels other than the first and last */
      while (--DeltaY) {
         ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
         ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
         if (ErrorAcc <= ErrorAccTemp) {
            /* The error accumulator turned over, so advance the X coord */
            X0 += XDir;
         }
         Y0++; /* Y-major, so always advance Y */
         /* The IntensityBits most significant bits of ErrorAcc give us the
            intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
         Weighting = ErrorAcc >> IntensityShift;
         drawPoint(i,X0, Y0, BaseColor + Weighting);
         drawPoint(i,X0 + XDir, Y0,
               BaseColor + (Weighting ^ WeightingComplementMask));
      }
      /* Draw the final pixel, which is 
         always exactly intersected by the line
         and so needs no weighting */
      drawPoint(i,X1, Y1, BaseColor);
      return;
   }
   /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
      pixel that Y advances each time X advances 1 pixel, truncating the
      result to avoid overrunning the endpoint along the X axis */
   ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;
   /* Draw all pixels other than the first and last */
   while (--DeltaX) {
      ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
      ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
      if (ErrorAcc <= ErrorAccTemp) {
         /* The error accumulator turned over, so advance the Y coord */
         Y0++;
      }
      X0 += XDir; /* X-major, so always advance X */
      /* The IntensityBits most significant bits of ErrorAcc give us the
         intensity weighting for this pixel, and the complement of the
         weighting for the paired pixel */
      Weighting = ErrorAcc >> IntensityShift;
      drawPoint(i,X0, Y0, BaseColor + Weighting);
      drawPoint(i,X0, Y0 + 1,
            BaseColor + (Weighting ^ WeightingComplementMask));
   }
   /* Draw the final pixel, which is always exactly intersected by the line
      and so needs no weighting */
   drawPoint(i,X1, Y1, BaseColor);
}

void drawPoint(Image *i, int x, int y, int col) 
{
    if ((unsigned)(x) <= (i->x)-1){
	    if ((unsigned)(y) <= (i->y)-1){
		    i->image[x][y] = col;
		}
	}
	/*image[x][y] = col; */
};

void drawObject(Image *im, int colour, Object *o, Matrix c)
{
	int i, j;
	Object transformed;
	objectInit(&transformed, o->vertnum, o->polynum);
	transformed.polys = o->polys;
	for(i = 0; i<o->vertnum; i++){
		Matrix vertice;
		Matrix result;
		vertice = new_matrix(1, 4);
		result = new_matrix(1, 4);
		double vertData[4];
		vertData[0] =  o->verts[i*4];
		vertData[1] =  o->verts[i*4+1];
		vertData[2] =  o->verts[i*4+2];
		vertData[3] =  o->verts[i*4+3];
		vertice.t = vertData;
		
		result = matrix_mult(vertice, c);
		transformed.verts[i*4] = result.t[0]/result.t[3];
		transformed.verts[i*4+1] = result.t[1]/result.t[3];
		transformed.verts[i*4+2] = result.t[2]/result.t[3];
		transformed.verts[i*4+3] = result.t[3];
		//print_matrix(c);
		//print_matrix(vertice);
		//print_matrix(result);
	}
	
	int *polyverts;
	double off_x = im->x/2;
	double off_y = im->y/2;
	
	for(j=0; j<(o->polynum); j++){
		polyverts = &o->polys[j*4];
		//printf("polyverts =%d\n", *(polyverts));
		if(*polyverts == PASS){
			struct int2 a = {(transformed.verts[(*(polyverts+1)-1)*4]*SCALE)+off_x, (transformed.verts[((*(polyverts+1)-1)*4)+1]*SCALE)+off_y};
			struct int2 b = {(transformed.verts[(*(polyverts+2)-1)*4]*SCALE)+off_x, (transformed.verts[((*(polyverts+2)-1)*4)+1]*SCALE)+off_y};
			struct int2 c = {(transformed.verts[(*(polyverts+3)-1)*4]*SCALE)+off_x, (transformed.verts[((*(polyverts+3)-1)*4)+1]*SCALE)+off_y};
			//printf("%d\n", c.y);
			drawTri(im, colour, a, b, c);
		}else{
			struct int2 a = {(transformed.verts[(*(polyverts)-1)*4]*SCALE)+off_x, (transformed.verts[((*(polyverts)-1)*4)+1]*SCALE)+off_y};
			struct int2 b = {(transformed.verts[(*(polyverts+1)-1)*4]*SCALE)+off_x, (transformed.verts[((*(polyverts+1)-1)*4)+1]*SCALE)+off_y};
			struct int2 c = {(transformed.verts[(*(polyverts+2)-1)*4]*SCALE)+off_x, (transformed.verts[((*(polyverts+2)-1)*4)+1]*SCALE)+off_y};
			struct int2 d = {(transformed.verts[(*(polyverts+3)-1)*4]*SCALE)+off_x, (transformed.verts[((*(polyverts+3)-1)*4)+1]*SCALE)+off_y};
			drawQuad(im, colour, a, b, c, d);
		}
	}
}

void drawFromFile(Image *i, FILE *save, FILE *f)
{
	/*Line buffer*/
	int bufferSize = 80;
	char *buffer = malloc(bufferSize*sizeof(char));
	
	/*Create translation stack*/
	stackT transformStack;
	stackInit(&transformStack, 10);
	
	/*Transform matrix*/
	Matrix C;
	C = new_matrix(4, 4);
	init_identity_matrix(&C);
	
	/*Object test*/
	int pass;
	
	while(fgets(buffer, bufferSize, f) != NULL){
		//printf("%s", buffer);
		char **com = NULL;
		char *p = strtok(buffer, " ");
		int spaces = 0;
		
		while(p){
			com = realloc(com, sizeof(char*) * ++spaces);
			
			if(com == NULL){
				printf("memory error.\n");
				exit(-1);
			}
			com[spaces-1] = p;
			p = strtok(NULL, " ");
		}
		com = realloc(com, sizeof(char*) * (spaces+1));
		com[spaces] = 0;
		//for(i=0; i<spaces+1; i++){
			//printf("%s\n", com[i]);
		//}
		
		switch(comConvert(com[0])){
			case 1://tran
				C = tran(C, atof(com[1]), atof(com[2]), atof(com[3]));
				break;
			case 2://pers
				C = pers(C, atof(com[1]), atof(com[2]), atof(com[3]));
				break;
			case 3:
				C = rot(C, atof(com[1]), atof(com[2]));
				break;
			case 4:
				C = scal(C, atof(com[1]), atof(com[2]), atof(com[3]));
				break;
			case 5:
				C = stackPop(&transformStack);
				break;
			case 6:
				stackPush(&transformStack, C);
				break;
			case 7:
				C = ortho(C, atof(com[1]));
				break;
			case 8: ;
				Object o;
				pass = 0;
				switch(shapeConvert(com[1])){
					case 1:
						o = new_cube();
						pass = 1;
						break;
					case 2:
						o = new_plane();
						pass = 1;
						break;
					case 3:
						o = new_oct();
						pass = 1;
						break;
					case 4:
						o = new_square();
						pass = 1;
						break;
					default:
						break;
				}
				if(pass){
					drawObject(i, 0, &o, C);
					objectDeInit(&o);
				}else{
					printf("Unknown object %s\n", com[1]);
				}
				break;
			default:
				break;
		}
		
		free(com);
	}
	writeImage(save, i);
}

int comConvert(char *com){
	if(strcmp(com, "TRAN") == 0) return 1;
	if(strcmp(com, "PERS") == 0) return 2;
	if(strcmp(com, "ROT") == 0) return 3;
	if(strcmp(com, "SCAL") == 0) return 4;
	if(strcmp(com, "POP") == 0) return 5;
	if(strcmp(com, "PUSH") == 0) return 6;
	if(strcmp(com, "ORTH") == 0) return 7;
	if(strcmp(com, "DRAW") == 0) return 8;
	
	return 0;
}

int shapeConvert(char * com)
{
	com[strcspn(com, "\n")] = 0;
	if(strcmp(com, "CUBE") == 0) return 1;
	if(strcmp(com, "PLANE") == 0) return 2;
	if(strcmp(com, "OCT") == 0) return 3;
	if(strcmp(com, "SQUARE") == 0) return 4;
	
	return 0;
}