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
}Object;

void objectInit(Object *o, int vertnum, int polynum)
{
	double *verts;
	int *polys;
	
	verts = (double *)malloc(sizeof(double)*vertnum*4);
	polys = (int *)malloc(sizeof(int)*polynum*4);
	
	o->verts = verts;
	o->polys = polys;
	o->vertnum = vertnum;
	o->polynum = polynum;
}

void objectDeInit(Object *o)
{
	free(o->verts);
	free(o->polys);
	o->vertnum = 0;
	o->polynum = 0;
}

Object new_square()
{
	Object square;
	objectInit(&square, 4, 1);
	double verts[16] = {1.0, 1.0, 0, 1.0, 1.0, -1.0, 0, 1.0, -1.0, -1.0, 0, 1.0, -1.0, 1.0, 0, 1.0};
	int polys[4] = {4, 3, 2, 1};
	memcpy(square.verts, verts, sizeof(verts));//VERY important!!!
	memcpy(square.polys, polys, sizeof(polys));
	return square;
}

Object new_plane()
{
	Object plane;
	objectInit(&plane, 25, 16);
	double verts[100] = {4, 4, 0, 1, 4, 3, 0, 1, 4, 2, 0, 1, 4, 1, 0, 1, 4, 0, 0, 1, 3, 0, 0, 1, 2, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 4, 0, 1, 1, 4, 0, 1, 2, 4, 0, 1, 3, 4, 0, 1, 3, 3, 0, 1, 3, 2, 0, 1, 3, 1, 0, 1, 2, 1, 0, 1, 1, 1, 0, 1, 1, 2, 0, 1, 1, 3, 0, 1, 2, 3, 0, 1, 2, 2, 0, 1};
	int polys[64] = {1, 2, 17, 16, 2, 3, 18, 17, 3, 4, 19, 18, 4, 5, 6, 19, 19, 6, 7, 20, 20, 7, 8, 21, 21, 8, 9, 10, 22, 21, 10, 11, 23, 22, 11, 12, 14, 23, 12, 13, 15, 24, 23, 14, 15, 16, 17, 24, 17, 18, 25, 24, 18, 19, 20, 25, 25, 20, 21, 22, 24, 25, 22, 23};
	memcpy(plane.verts, verts, sizeof(verts));
	memcpy(plane.polys, polys, sizeof(polys));
	return plane;
}

Object new_cube()
{
	Object cube;
	objectInit(&cube, 8, 6);
	double verts[32] = {1, 1, 1, 1, 1, 1, -1, 1, 1, -1, 1, 1, 1, -1, -1, 1, -1, 1, 1, 1, -1, 1, -1, 1, -1, -1, 1, 1, -1, -1, -1, 1};
	int polys[24] = {2, 1, 3, 4, 5, 6, 8, 7, 1, 2, 6, 5, 4, 3, 7, 8, 3, 1, 5, 7, 2, 4, 8, 6};
	memcpy(cube.verts, verts, sizeof(verts));
	memcpy(cube.polys, polys, sizeof(polys));
	return cube;
}

Object new_oct()
{
	Object oct;
	objectInit(&oct, 6, 8);
	double verts[24] = {1, 0, 0, 1, -1, 0, 0, 1, 0, 1, 0, 1, 0, -1, 0, 1, 0, 0, 1, 1, 0, 0, -1, 1};
	int polys[32] = {PASS, 2, 3, 5, PASS, 3, 1, 6, PASS, 4, 1, 5, PASS, 1, 4, 6, PASS, 3, 2, 5, PASS, 2, 3, 6, PASS, 2, 4, 5, PASS, 4, 2, 6};
	memcpy(oct.verts, verts, sizeof(verts));
	memcpy(oct.polys, polys, sizeof(polys));
	return oct;
}

#endif