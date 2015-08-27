#ifndef PASS
#define PASS 999999
#endif

#ifndef _OBJECTS_H
#define _OBJECTS_H

typedef struct {
    int vertnum;
	int polynum;
	double *verts;
	int *polys;
}oldObject;

typedef struct {
	int vertnum;
	int polynum;
	double *verts;
	int **polys;
}Object;

Object objectInitFrom(Object *o)
{
	Object ob;
	//double *verts;
	//int **polys;
	
	ob.verts = (double *)malloc(sizeof(double) * o->vertnum*4);
	ob.polys = (int **)malloc(sizeof(int)* o->polynum);
	int i, j;
	for(i=0; i<(o->polynum); i++){
		int count = 0;
		j = 0;
		while(o->polys[i][j] != 0){
			count++;
			j++;
		}
		ob.polys[i] = (int *)malloc(sizeof(int) * (count+1));
	}
	
	//ob.verts = o->verts;
	//ob.polys = o->polys;
	ob.vertnum = o->vertnum;
	ob.polynum = o->polynum;
	
	return ob;
}

void objectInit(Object *o)
{
	o->vertnum = 0;
	o->polynum = 0;
	o->verts = (double *)malloc(sizeof(double));
	o->polys = (int **)malloc(sizeof(int));
}

void objectDeInit(Object *o)
{
	//free(o->verts);
	int i;
	for(i=0; i<o->polynum; i++){
		free(o->polys[i]);
	}
	free(o->polys);
	o->vertnum = 0;
	o->polynum = 0;
}

Object new_house()
{
	Object o;
	o.vertnum = 5;
	o.polynum = 2;
	double verts[20] = {0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0.5, 2, 0, 1};
	o.polys = (int **)malloc(sizeof(int) * o.polynum);
	o.polys[0] =(int *)malloc(sizeof(int) * 5);//{1, 2, 3, 4};
	o.polys[1] = (int *)malloc(sizeof(int) * 4);
	o.verts = malloc(sizeof(double)*20);
	int poly1[5] = {1, 2, 3, 4, 0};
	int poly2[4] = {4, 3, 5, 0};
	memcpy(o.verts, verts, sizeof(verts));
	memcpy(o.polys[0], &poly1, sizeof(poly1));
	memcpy(o.polys[1], &poly2, sizeof(poly2));
	return o;
}

Object new_square()
{
	Object square;
	square.vertnum = 4;
	square.polynum = 1;
	double verts[16] = {1.0, 1.0, 0, 1.0, 1.0, -1.0, 0, 1.0, -1.0, -1.0, 0, 1.0, -1.0, 1.0, 0, 1.0};
	square.polys = (int **)malloc(sizeof(int) * square.polynum);
	square.polys[0] = (int *)malloc(sizeof(int) * 5);
	square.verts = malloc(sizeof(double)*16);
	int poly1[5] = {4, 3, 2, 1, 0};
	memcpy(square.verts, verts, sizeof(verts));//VERY important!!!
	memcpy(square.polys[0], &poly1, sizeof(poly1));
	return square;
}

Object new_plane()
{
	Object plane;
	plane.vertnum = 25;
	plane.polynum = 16;
	double verts[100] = {4, 4, 0, 1, 4, 3, 0, 1, 4, 2, 0, 1, 4, 1, 0, 1, 4, 0, 0, 1, 3, 0, 0, 1, 2, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 4, 0, 1, 1, 4, 0, 1, 2, 4, 0, 1, 3, 4, 0, 1, 3, 3, 0, 1, 3, 2, 0, 1, 3, 1, 0, 1, 2, 1, 0, 1, 1, 1, 0, 1, 1, 2, 0, 1, 1, 3, 0, 1, 2, 3, 0, 1, 2, 2, 0, 1};
	plane.polys = (int **)malloc(sizeof(int) * plane.polynum);
	int i;
	for(i=0; i<plane.polynum; i++){
		plane.polys[i] = (int *)malloc(sizeof(int) * 5);
	}
	int poly1[5] = {1, 16, 17, 2, 0};
	int poly2[5] = {17, 18, 3, 2, 0};
	int poly3[5] = {18, 19, 4, 3, 0};
	int poly4[5] = {19, 6, 5, 4, 0};
	int poly5[5] = {20, 7, 6, 19, 0};
	int poly6[5] = {21, 8, 7, 20, 0};
	int poly7[5] = {10, 9, 8, 21, 0};
	int poly8[5] = {11, 10, 21, 22, 0};
	int poly9[5] = {12, 11, 22, 23, 0};
	int poly10[5] = {13, 12, 23, 14, 0};
	int poly11[5] = {14, 23, 24, 15, 0};
	int poly12[5] = {24, 17, 16, 15, 0};
	int poly13[5] = {24, 25, 18, 17, 0};
	int poly14[5] = {18, 19, 20, 25, 0};
	int poly15[5] = {22, 21, 20, 25, 0};
	int poly16[5] = {23, 22, 25, 24, 0};
	plane.verts = malloc(sizeof(double) * 100);
	memcpy(plane.verts, verts, sizeof(verts));
	memcpy(plane.polys[0], &poly1, sizeof(poly1));
	memcpy(plane.polys[1], &poly2, sizeof(poly2));
	memcpy(plane.polys[2], &poly3, sizeof(poly3));
	memcpy(plane.polys[3], &poly4, sizeof(poly4));
	memcpy(plane.polys[4], &poly5, sizeof(poly5));
	memcpy(plane.polys[5], &poly6, sizeof(poly6));
	memcpy(plane.polys[6], &poly7, sizeof(poly7));
	memcpy(plane.polys[7], &poly8, sizeof(poly8));
	memcpy(plane.polys[8], &poly9, sizeof(poly9));
	memcpy(plane.polys[9], &poly10, sizeof(poly10));
	memcpy(plane.polys[10], &poly11, sizeof(poly11));
	memcpy(plane.polys[11], &poly12, sizeof(poly12));
	memcpy(plane.polys[12], &poly13, sizeof(poly13));
	memcpy(plane.polys[13], &poly14, sizeof(poly14));
	memcpy(plane.polys[14], &poly15, sizeof(poly15));
	memcpy(plane.polys[15], &poly16, sizeof(poly16));
	return plane;
}

Object new_cube()
{
	Object cube;
	cube.vertnum = 8;
	cube.polynum = 6;
	double verts[32] = {1, 1, 1, 1, 1, 1, -1, 1, 1, -1, 1, 1, 1, -1, -1, 1, -1, 1, 1, 1, -1, 1, -1, 1, -1, -1, 1, 1, -1, -1, -1, 1};
	cube.polys = (int **)malloc(sizeof(int) * cube.polynum);
	int i;
	for(i=0; i<cube.polynum; i++){
		cube.polys[i] = (int *)malloc(sizeof(int) * 5);
	}
	int poly1[5] = {2, 1, 3, 4, 0};
	int poly2[5] = {5, 6, 8, 7, 0};
	int poly3[5] = {1, 2, 6, 5, 0};
	int poly4[5] = { 4, 3, 7, 8, 0};
	int poly5[5] = {3, 1, 5, 7, 0};
	int poly6[5] = {2, 4, 8, 6, 0};
	
	cube.verts = malloc(sizeof(double) * 32);
	memcpy(cube.verts, verts, sizeof(verts));
	
	memcpy(cube.polys[0], &poly1, sizeof(poly1));
	memcpy(cube.polys[1], &poly2, sizeof(poly2));
	memcpy(cube.polys[2], &poly3, sizeof(poly3));
	memcpy(cube.polys[3], &poly4, sizeof(poly4));
	memcpy(cube.polys[4], &poly5, sizeof(poly5));
	memcpy(cube.polys[5], &poly6, sizeof(poly6));
	return cube;
}

Object new_oct()
{
	Object oct;
	oct.vertnum = 6;
	oct.polynum = 8;
	double verts[24] = {1, 0, 0, 1, -1, 0, 0, 1, 0, 1, 0, 1, 0, -1, 0, 1, 0, 0, 1, 1, 0, 0, -1, 1};
	oct.polys = (int **)malloc(sizeof(int) * oct.polynum);
	int i;
	for(i=0; i<oct.polynum; i++){
		oct.polys[i] = (int *)malloc(sizeof(int) * 4);
	}
	int poly1[4] = {1, 3, 5, 0};
	int poly2[4] = {3, 1, 6, 0};
	int poly3[4] = {4, 1, 5, 0};
	int poly4[4] = {1, 4, 6, 0};
	int poly5[4] = {3, 2, 5, 0};
	int poly6[4] = {2, 3, 6, 0};
	int poly7[4] = {2, 4, 5, 0};
	int poly8[4] = {4, 2, 6, 0};
	
	oct.verts = malloc(sizeof(double) * 24);
	memcpy(oct.verts, verts, sizeof(verts));
	
	memcpy(oct.polys[0], poly1, sizeof(poly1));
	memcpy(oct.polys[1], poly2, sizeof(poly2));
	memcpy(oct.polys[2], poly3, sizeof(poly3));
	memcpy(oct.polys[3], poly4, sizeof(poly4));
	memcpy(oct.polys[4], poly5, sizeof(poly5));
	memcpy(oct.polys[5], poly6, sizeof(poly6));
	memcpy(oct.polys[6], poly7, sizeof(poly7));
	memcpy(oct.polys[7], poly8, sizeof(poly8));
	return oct;
}

#endif