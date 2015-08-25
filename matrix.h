#ifndef _MATRIX_H
#define _MATRIX_H
typedef struct {
    int rows;
	int cols;
	double *t;
}Matrix;

Matrix new_matrix(int rows, int cols);
void init_identity_matrix(Matrix *t);
void print_matrix(Matrix t);
Matrix matrix_mult(Matrix a, Matrix b);
Matrix matrix_add(Matrix a, Matrix b);
Matrix matrix_scalar_mult(Matrix a, double b);
Matrix tran(Matrix a, double x, double y, double z);
Matrix scal(Matrix a, double sx, double sy, double sz);
Matrix rot(Matrix a, double theta, int axis);
Matrix pers(Matrix a, double alpha, double zn, double zf);
Matrix ortho(Matrix a, double d);
#endif