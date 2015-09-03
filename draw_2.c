#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "matrix.h"
#include "stack.h"
#include "objects.h"

#define PASS 999999
#define SCALE 100

double currentX = 0;
double currentY = 0;
int colourAdd = 0;

typedef struct int2 {
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

typedef struct {
	double **p;
	int n;
}Poly;

typedef struct{
	double a[3];
	double b[3];
	int boundry;
	int colour;
} Edge;

typedef struct{
	int n;
	Edge *edges;
}EdgeList;

typedef struct{
	int n;
	Poly *polys;
}PolyList;

typedef struct{
	double alpha;
	int deltaQI;
}Intersection;

typedef struct{
	int n;
	Intersection *inters;
}IntersList;

void drawPoint(Image *i, int x, int y, int col);
void drawWuLine (Image *i, short X0, short Y0, short X1, short Y1,
         short BaseColor, short NumLevels, unsigned short IntensityBits);
void drawQuad(Image *i, int colour, struct int2 a, struct int2 b, struct int2 c, struct int2 d);
void drawTri(Image *i, int colour, struct int2 a, struct int2 b, struct int2 c);
void createCanvas(Image *i, FILE *f, int colour, int width, int height);
void writeImage(FILE *f, Image *i);
void matrixTests();
void addObject(EdgeList *eList, PolyList *pList, int colour, Object *o, Matrix c);
void drawFromFile(Image *i, FILE *save, FILE *f);
int comConvert(char * com);
int shapeConvert(char * com);
void subset(double *dest, double *start, int size);
void objectTests();
void addEdge(EdgeList *list, Edge edge);
void addPoly(PolyList *list, Poly poly);
void renderScene(EdgeList *list, PolyList *pList, Image *im);
Matrix findNormal(Poly *poly);
void moveTo(Image *im, double x, double y);
void drawTo(Image *im, double x, double y);
void sortIntersections(IntersList *list);

int main(int argc, char* argv[])
{
	if(argc > 1){
		if(strcmp(argv[1], "-t") == 0){
			matrixTests();
			return 1;
		}
		if(strcmp(argv[1], "-o") == 0){
			objectTests();
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
	
	/*Close image and save*/
	writeImage(f, &image);
	return 1;
}

void objectTests()
{
	printf("Object tests:\n");
	Object o;
	o = new_house();
	
	int i, j;
	for(i=0; i<o.polynum; i++){
		printf("poly %d:\n", i);
		int cur;
		j = 0;
		while((cur=o.polys[i][j])!=0){
			int next;
			if(o.polys[i][j+1] == 0){
				next = o.polys[i][0];
			}else{
				next = o.polys[i][j+1];
			}
			printf("line between %d and %d\n", cur, next);
			j++;
		}
	}
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
	
	Matrix a;
	Matrix b;
	Matrix s;
	a = new_matrix(2, 1);
	b = new_matrix(2, 1);
	s = new_matrix(2, 1);
	double data3[2] = {1, 1};
	a.t = data3;
	b.t = data3;
	a = matrix_scalar_mult(a, -1);
	s = matrix_add(a, b);
	print_matrix(s);
}

void moveTo(Image *im, double x, double y)
{
	double x_offset = (im->x)/2;
	double y_offset = (im->y)/2;
	currentX = x*SCALE + x_offset;
	currentY = y*SCALE + y_offset;
}

void drawTo(Image *im, double x, double y){
	double x_offset = (im->x)/2;
	double y_offset = (im->y)/2;
	drawWuLine(im, currentX, currentY, x*SCALE+x_offset, y*SCALE+y_offset, 0, 256, 8);
	moveTo(im, x, y);
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
	int colour;
	if(col<0){
		colour = 0;
	}
	colour = col+colourAdd;
	if(colour>255){
		colour = 255;
	}
	
    if ((unsigned)(x) <= (i->x)-1){
	    if ((unsigned)(y) <= (i->y)-1){
		    i->image[x][y] = colour;
		}
	}
	/*image[x][y] = col; */
};

void subset(double *dest, double *start, int size)
{
	int i;
	for(i=0; i<size; i++){
		dest[i] = *(start+i);
	}
}

Matrix findNormal(Poly *poly)
{
	Matrix a;
	Matrix b;
	Matrix c;
	Matrix crossA, crossB;
	Matrix n;
	a = new_matrix(4, 1);
	b = new_matrix(4, 1);
	c = new_matrix(4, 1);
	crossA = new_matrix(4, 1);
	crossB = new_matrix(4, 1);
	n = new_matrix(4, 1);
	
	a.t = poly->p[0];
	b.t = poly->p[1];
	c.t = poly->p[2];
	a = matrix_scalar_mult(a, -1);
	crossA = matrix_add(b, a);
	crossB = matrix_add(c, a);
	n = matrix_cross(crossA, crossB);
	
	return n;
}

int cull(Poly *poly)
{
	Matrix a;
	Matrix b;
	Matrix c;
	Matrix crossA, crossB;
	Matrix n;
	a = new_matrix(4, 1);
	b = new_matrix(4, 1);
	c = new_matrix(4, 1);
	crossA = new_matrix(4, 1);
	crossB = new_matrix(4, 1);
	n = new_matrix(4, 1);
	
	a.t = poly->p[0];
	b.t = poly->p[1];
	c.t = poly->p[2];
	//print_matrix(a);
	//print_matrix(b);
	//print_matrix(c);
	a = matrix_scalar_mult(a, -1);
	crossA = matrix_add(b, a);
	crossB = matrix_add(c, a);
	//printf("test\n");
	n = matrix_cross(crossA, crossB);
	
	
	double cross;
	cross = ((a.t[0])*n.t[0]) + ((a.t[1])*n.t[1]) + ((a.t[2])*n.t[2]);
	//printf("cull = %f\n", cross);
	if(cross <= 0){
		return 0;
	}
	return 1;
}

void addObject(EdgeList *eList, PolyList *pList, int colour, Object *o, Matrix c)
{
	int i, j;
	Object transformed;
	transformed = objectInitFrom(o);
	transformed.polys = o->polys;
	for(i = 0; i<o->vertnum; i++){
		Matrix vertice;
		Matrix result;
		vertice = new_matrix(1, 4);
		result = new_matrix(1, 4);
		double vertData[4] = {0, 0, 0, 0};
		
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
	
	
	//double off_x = im->x/2;
	//double off_y = im->y/2;
	
	for(j=0; j<(o->polynum); j++){
		int *polyverts;
		int count = 0;
		Poly poly;
		polyverts = &o->polys[j][0];
		//printf("polyverts = %d\n", *polyverts);
		while(o->polys[j][count] != 0){
			count++;
		}
		//printf("count: %d\n", count);
		poly.p = (double **)malloc(sizeof(double) * count);
		poly.n = count;
		int k;
		for(k=0; k<count; k++){
			poly.p[k] = (double *)malloc(sizeof(double) * 4);
			subset(poly.p[k], &transformed.verts[(*(polyverts+k)-1)*4], 4);
		}
	
		if(!cull(&poly)){
			//printf("not culling\n");
			addPoly(pList, poly);
			int m;
			int cur = -1;
			int next = 0;
			m = 0;
			//
			while(o->polys[j][m]!=0){
				if(o->polys[j][m+1] == 0){
					next = 0;
					cur++;
				}else{
					next++;
					cur++;
				}
				Edge edge;
				edge.boundry = 0;
				edge.a[0] = poly.p[cur][0];
				edge.a[1] = poly.p[cur][1];
				edge.a[2] = poly.p[cur][2];
				
				edge.b[0] = poly.p[next][0];
				edge.b[1] = poly.p[next][1];
				edge.b[2] = poly.p[next][2];
				
				edge.colour = colourAdd;
				
				addEdge(eList, edge);
				//drawWuLine(im, (poly.p[cur][1]*SCALE)+off_y, (poly.p[cur][0]*SCALE)+off_x, (poly.p[next][1]*SCALE)+off_y, (poly.p[next][0]*SCALE)+off_x, 0, 256, 8);
				m++;
			}
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
	
	/*Edge list*/
	EdgeList eList;
	eList.n = 0;
	eList.edges = malloc(sizeof(Edge));
	
	/*Poly list*/
	PolyList pList;
	pList.n = 0;
	pList.polys = malloc(sizeof(Poly));
	
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
				//objectInit(&o);
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
					case 5:
						o = new_house();
						pass = 1;
						break;
					case 6:
						o = new_icosahedron();
						pass = 1;
						break;
					default:
						break;
				}
				if(pass){
					addObject(&eList, &pList, 0, &o, C);
					printf("Added object %s\n", com[1]);
					objectDeInit(&o);
					
				}else{
					printf("Unknown object %s\n", com[1]);
				}
				break;
			case 9:
				colourAdd = (int) atof(com[1]);
				break;
			default:
				break;
		}
		
		free(com);
	}
	renderScene(&eList, &pList, i);
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
	if(strcmp(com, "COLOUR") == 0) return 9;
	
	return 0;
}

int shapeConvert(char * com)
{
	com[strcspn(com, "\n")] = 0;
	if(strcmp(com, "CUBE") == 0) return 1;
	if(strcmp(com, "PLANE") == 0) return 2;
	if(strcmp(com, "OCT") == 0) return 3;
	if(strcmp(com, "SQUARE") == 0) return 4;
	if(strcmp(com, "HOUSE") == 0) return 5;
	if(strcmp(com, "ICOSAHEDRON") == 0) return 6;
	
	return 0;
}

void addEdge(EdgeList *list, Edge edge)
{
	int n = list->n;
	int i;
	for(i=0; i<n; i++){
		if((list->edges[i].a[0]==edge.b[0]) && (list->edges[i].a[1]==edge.b[1])){
			if((list->edges[i].b[0]==edge.a[0]) && (list->edges[i].b[1]==edge.a[1])){
				list->edges[i].boundry = 0;
				//printf("NOT BOUNDRY!!\n");
				//printf("%f, %f\n", edge.a[0]*SCALE+512, edge.a[1]*SCALE+512);
				return;
			}
		}
	}
	edge.boundry = 1;
	list->edges = realloc(list->edges, sizeof(Edge)*(list->n + 1));
	list->edges[list->n++] = edge;
}

void addPoly(PolyList *list, Poly poly)
{
	list->polys = realloc(list->polys, sizeof(Poly)*(list->n + 1));
	list->polys[list->n++] = poly;
}

void addInter(IntersList *list, Intersection i){
	list->inters = realloc(list->inters, sizeof(Intersection)*(list->n + 1));
	list->inters[list->n++] = i;
}

void removeInter(IntersList *list, int n){
	free(&(list->inters[n]));
	list->n--;
}

void sortIntersections(IntersList *list)
{
	int n = list->n;
	//printf("n:%d\n", n);
	int i, j;
	Intersection temp;
	double smallest = 2;
	int pos;
	for(i=0; i<n; i++){
		for(j=(i+1); j<n; j++){
			if(list->inters[j].alpha < smallest){
				smallest = list->inters[j].alpha;
				pos = j;
			}
		}
		if(smallest < list->inters[i].alpha){
			temp = list->inters[i];
			list->inters[i] = list->inters[pos];
			list->inters[pos] = temp;
		}
	}
	//for(i=0; i<n; i++){
		//printf("alpha: %f\n", list->inters[i].alpha);
	//}
}

void renderScene(EdgeList *eList, PolyList *pList, Image *im)
{
	printf("Rendering...\n");
	int lineTotal, i, j;
	lineTotal = eList->n;
	//printf("%d\n", lineTotal);
	int polyCur = 0;
	int lineNumber = 0;
	for(i=0; i<lineTotal; i++){
		if(lineNumber < pList->polys[polyCur].n){
			lineNumber++;
		}else{
			polyCur++;
			lineNumber = 0;
		}
		int flag = 0;
		/*Face-vertex*/
		/*************/
		double testP[3] = {eList->edges[i].a[0], eList->edges[i].a[1], 1}; 										//test point, start of line
		int polyN = pList->n; 																					//total number of polys
		
		int QI = 0;	//Quantitative invisibility 
		
		for(j=0; j<polyN; j++){
			flag = 0;
			if(polyCur == j){
				//continue;
			}
			
			int vertexNumber;
			for(vertexNumber=pList->polys[j].n-1; vertexNumber>=0; vertexNumber--){
				
				//define points
				double curP[3] = {pList->polys[j].p[vertexNumber][0], pList->polys[j].p[vertexNumber][1], 1}; 	//ith polygon
				double nextP[3];																				//ith+1 polygon
				if(vertexNumber != 0){
					nextP[0] = pList->polys[j].p[vertexNumber-1][0]; 
					nextP[1] = pList->polys[j].p[vertexNumber-1][1];
					nextP[2] = 1;
				}else{
					nextP[0] = pList->polys[j].p[pList->polys[j].n-1][0]; 
					nextP[1] = pList->polys[j].p[pList->polys[j].n-1][1];
					nextP[2] = 1;
				}
				
				//calculate the D's
				double Di = curP[1] - testP[1];
				double DiPlusOne = nextP[1] - testP[1];
				if(signbit(Di*DiPlusOne)==0){ //positive if same sign so skip this vertex
					continue;
				}
				//different sign
				double DInfinity = nextP[1] - curP[1];
				double D = (nextP[1] - testP[1]) * (curP[0] - testP[0]) - (nextP[0] - testP[0])*(curP[1] - testP[1]);
				if(signbit(D * DInfinity)==0){ //positive if same sign so skip this vertex
					continue;
				}else{
					flag = (flag + 1)%2;
				}
			}

			if(flag){
				//test z depth
				Matrix n;
				n = new_matrix(4, 1);
				n = findNormal(&(pList->polys[j]));
				//normal
				double a = n.t[0];
				double b = n.t[1];
				double c = n.t[2];
				//point on plane;
				double x0 = pList->polys[j].p[0][0];
				double y0 = pList->polys[j].p[0][1];
				double z0 = pList->polys[j].p[0][2];
				//test point
				double x = testP[0];
				double y = testP[1];
				double z = ((-a*(x-x0) - b*(y-y0))/c) + z0;

				if(z < eList->edges[i].a[2]){
					QI++;
				}else{
					flag = 0;
				}
			}else{
				continue;
			}
		}
		
		//printf("initial QI=%d\n", QI);
		/*Find Intersections*/
		/********************/
		IntersList list;
		list.n = 0;
		list.inters = malloc(sizeof(Intersection));
		int n;
		for(n=0; n<lineTotal; n++){
			if(n == i){	//don't compare to self
				continue;
			}
			if(eList->edges[n].boundry == 0){	//only compare to boundary edges	
				continue;
			}
			double x1 = eList->edges[i].a[0];
			double y1 = eList->edges[i].a[1];
			double x2 = eList->edges[i].b[0];
			double y2 = eList->edges[i].b[1];
			double x3 = eList->edges[n].a[0];
			double y3 = eList->edges[n].a[1];
			double x4 = eList->edges[n].b[0];
			double y4 = eList->edges[n].b[1];
			
			double D1 = (x4-x3)*(y1-y3) - (x1-x3)*(y4-y3);
			double D2 = (x4-x3)*(y2-y3) - (x2-x3)*(y4-y3);
			if(signbit(D1*D2)==0){
				continue;
			}
			double D3 = (x1-x3)*(y2-y3) - (x2-x3)*(y1-y3);
			double D4 = D1 - D2 + D3;
			if(signbit(D3*D4)==0){
				continue;
			}
			if (signbit(D3) && D3==0) D3 *= -1;
			if (signbit(D4) && D4==0) D4 *= -1;
			if(D1 == 0){
				continue;
			}
			double alpha = D1 / (D1 - D2);
			double beta = D3 / (D3 - D4);
			
			double z1 = eList->edges[i].a[2];
			double z2 = eList->edges[i].b[2];
			double z3 = eList->edges[n].a[2];
			double z4 = eList->edges[n].b[2];
			
			double Zi = z1 + alpha*(z2-z1);
			double Zj = z3 + beta*(z4-z3);
			
			if(Zi <= Zj){
				continue;
			}
			Intersection intersection;
			intersection.deltaQI = D1 < 0 ? 1:-1;
			intersection.alpha = alpha;
			
			addInter(&list, intersection);
		}
		
		/*Draw edge in bits*/
		/*******************/
		if(QI == 0){
			moveTo(im, eList->edges[i].a[0], eList->edges[i].a[1]);
		}
		colourAdd = eList->edges[i].colour;
		int a;
		int deltaQI;
		double alpha;
		sortIntersections(&list);
		for(a=0; a<list.n; a++){
			deltaQI = list.inters[a].deltaQI;
			
			QI = QI + deltaQI;
			alpha = list.inters[a].alpha;
			double newX = eList->edges[i].a[0] + alpha*(eList->edges[i].b[0] - eList->edges[i].a[0]);
			double newY = eList->edges[i].a[1] + alpha*(eList->edges[i].b[1] - eList->edges[i].a[1]);
			
			//if(QI<0){
				//printf("x:%f, y:%f\n\n\n", eList->edges[i].a[0]*SCALE+(im->x/2), eList->edges[i].a[1]*SCALE+(im->y/2));
				//printf("QI: %d, deltaQI: %d\n", QI, deltaQI);
				//printf("Error, negative QI!\n");
			//}
			if(QI==0 && deltaQI==-1){
				moveTo(im, newX, newY);
				continue;
			}
			if(QI==1 && deltaQI==1){
				drawTo(im, newX, newY);
				continue;
			}
		}
		if(QI==0){
			drawTo(im, eList->edges[i].b[0], eList->edges[i].b[1]);
		}
		colourAdd = 0;
	}
}