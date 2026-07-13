#pragma once

typedef struct {
    unsigned short rows;
    unsigned short columns;
    double **arr;
} Matrix;

void initMatrix(Matrix *matrix, unsigned short rows, unsigned short columns);
void initIdentityMatrix(Matrix *matrix, unsigned short order);
int addMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
int subtractMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
int multiplyMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
void getMatrixTranspose(Matrix *matrix, Matrix *result);
double getDeterminant(Matrix *matrix);
void getMinorMatrix(Matrix *matrix, unsigned short row, unsigned short column,
                    Matrix *result);
void getCofactor(Matrix *matrix, unsigned short row, unsigned short column,
                 Matrix *result);
void freeMatrix(Matrix *matrix);
