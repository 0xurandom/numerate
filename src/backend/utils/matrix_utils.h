#pragma once

#include "num_utils.h"

typedef struct {
    unsigned short rows;
    unsigned short columns;
    Number **arr;
} Matrix;

void initMatrix(Matrix *matrix, unsigned short rows, unsigned short columns);
void initIdentityMatrix(Matrix *matrix, unsigned short order);
int addMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
int subtractMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
int multiplyMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
void getMatrixTranspose(Matrix *matrix, Matrix *result);
Number *getDeterminant(Matrix *matrix);
Matrix *getMinorMatrix(Matrix *matrix, unsigned short row,
                       unsigned short column);
Matrix *getCofactor(Matrix *matrix, unsigned short row, unsigned short column);
void freeMatrix(Matrix *matrix);
