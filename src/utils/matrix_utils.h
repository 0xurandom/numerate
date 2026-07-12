#pragma once

typedef struct {
    unsigned short rows;
    unsigned short columns;
    double **arr;
} Matrix;

void initMatrix(Matrix *matrix, unsigned short rows, unsigned short columns);
int addMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
int subtractMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
int multiplyMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
void getMatrixTranspose(Matrix *matrix, Matrix *result);
