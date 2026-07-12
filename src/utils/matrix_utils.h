#pragma once

typedef struct {
    unsigned short rows;
    unsigned short columns;
    double **arr;
} Matrix;

void initMatrix(unsigned short rows, unsigned short columns);
int addMatrices(Matrix *mat1, Matrix *mat2, Matrix *result);
