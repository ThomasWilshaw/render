#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

/*Matrix {
    int rows;
	int cols;
	double *t;
};
*/
#define M(m,x,y) m.t[x+y*m.cols]

double rad(double x){
	double i = 180.0;
	return (x*M_PI)/i;
}

Matrix new_matrix(int rows, int cols)
{
    Matrix t;
	t.rows = rows;
	t.cols = cols;
	t.t = (double *)malloc(sizeof(double)*cols*rows);
	return t;
}

void init_identity_matrix(Matrix *t)
{
	int rows = t->rows;
	int cols = t->cols;
	
	if(rows != cols){
		printf("Matrix error: Identity matrix must be square.\n");
		exit(EXIT_FAILURE);
	}
	
	int n = rows*cols;
	double *data = (double *)malloc(sizeof(double)*n);
	int i, j;
	int k = 0;
	for(i=0; i<rows; i++){
		for(j=0; j<cols; j++){
			data[i+j*cols] = 0;
		}
	}
	
	for(i=0; i<rows; i++){
		data[i+(k)*cols] = 1;
		k++;
	}
	
	for(k=0; k<n; k++){
		t->t[k] = data[k];
	}
}

void print_matrix(Matrix t)
{
    int i, j;
	for ( i=0; i<t.rows; i++) {
	    printf("| ");
		for (j=0; j<t.cols; j++)
		    printf("%f ", M(t, j, i));
		printf("|\n");
	}
	printf("\n");
}

Matrix matrix_mult(Matrix a, Matrix b)
{
    int x, y, z;
	Matrix r;
	r = new_matrix(b.cols, a.rows);
	if(a.cols != b.rows){
		printf("ERROR, matrix dimension mismatch.\n");
		exit(EXIT_FAILURE);
	}
	
	for (x=0; x<a.rows; x++){
	    for (y=0; y<b.cols; y++){
		    M(r, y, x) = 0.0;
			for (z=0; z<a.cols; z++){
			   M(r, y, x) += M(a, z, x)*M(b, y, z);
			}
			if (M(r, x, y) == 0 && signbit(M(r, x, y))) M(r, x, y) *= -1;
		}
	} 
	return r;
}

Matrix matrix_add(Matrix a, Matrix b)
{
	if(a.cols!=b.cols || a.rows!=b.rows){
		printf("ERROR, matrix dimension mismatch.\n");
		exit(EXIT_FAILURE);
	}
    int x, y;
	Matrix r;
	r = new_matrix(a.rows, a.rows);
	
	for (x=0; x<a.rows; x++){
	    for (y=0; y<b.cols; y++){
		    M(r, y, x) = M(a, y, x)+M(b, y, x);
			
		}
	}
	return r;
}

Matrix matrix_scalar_mult(Matrix a, double b)
{
    int x, y;
	Matrix r;
	r = new_matrix(a.rows, a.rows);
	
	for (x=0; x<a.rows; x++){
	    for (y=0; y<a.cols; y++){
		    M(r, y, x) = M(a, y, x)* b;
			
		}
	}
	return r;
}

Matrix tran(Matrix a, double x, double y, double z)
{
    Matrix r, tran;
	r = new_matrix(4, 4);
	tran = new_matrix(4, 4);
	double elem[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1};
	tran.t = elem;
	r = matrix_mult(tran, a);
	
	return r;
}

Matrix scal(Matrix a, double sx, double sy, double sz)
{
    Matrix r, scal;
	r = new_matrix(4, 4);
	scal = new_matrix(4, 4);
	double elem[16] = {sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1};
	scal.t = elem;
	r = matrix_mult(scal, a);
	
	return r;
}

Matrix rot(Matrix a, double theta, int axis)
{
	if(axis!=1 && axis!=2 && axis!=3){
		printf("Error, axis must equal 1, 2, or 3.");
		exit(EXIT_FAILURE);
	}
    /*left or right coordianate system??*/
	double s, c;
	Matrix r, rot;
	r = new_matrix(4, 4);
	rot = new_matrix(4, 4);
	
	s = sin(rad(theta));
	c = cos(rad(theta));
	double elem1[16] = {1, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1};
	double elem2[16] = {c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1};
	double elem3[16] = {c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	
	switch(axis){
	    case 1:
		    rot.t = elem1;
			break;
		case 2:
		    rot.t = elem2;
			break;
		case 3:
		    rot.t = elem3;
			break;
	}
	r = matrix_mult(rot, a);
	
	return r;
}

Matrix pers(Matrix a, double alpha, double zn, double zf)
{
	if(zn>zf){
		printf("Error, zn must be less then zf.");
		exit(EXIT_FAILURE);
	}
    double s, c, q;
	Matrix r, pers;
	r = new_matrix(4, 4);
	pers = new_matrix(4, 4);
	
	s = sin(rad(alpha)/2.0);
	c = cos(rad(alpha)/2.0);
	q = s/(1-(zn/zf));
	
	double elem[16] = {c, 0, 0, 0, 0, c, 0, 0, 0, 0, q, s, 0, 0, -q*zn, 0};
	pers.t = elem;
	
	r = matrix_mult(pers, a);
	
	
	return r;
}

Matrix ortho(Matrix a, double d)
{
	//double f = 1.0/tan(alpha/2.0);
	Matrix r, pers;
	r = new_matrix(4, 4);
	pers = new_matrix(4, 4);
	
	double elem[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1/d, 1};
	pers.t = elem;
	
	r = matrix_mult(pers, a);
	
	return r;
}

/*http://www1.icsi.berkeley.edu/~sather/performance/benchmarks/data/matrix.c*/

/*int main()
{
    Matrix b;
	Matrix c;
	Matrix d;
	b = new_matrix(4, 4);
	c = new_matrix(4, 4);
	double data1[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 9, 8, 7, 6, 5, 4};
	double data2[16] = {2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2};
	b.t = data1;
	c.t = data2;
	d = matrix_scalar_mult(b, 1.0/3.0);
	print_matrix(d);
	
	return 0;
}*/