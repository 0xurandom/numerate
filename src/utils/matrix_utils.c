#include "matrix_utils.h"

#include <stdio.h>
#include <stdlib.h>

void initMatrix(unsigned short rows, unsigned short columns) {
    if (rows == 0 || columns == 0) {
        fprintf(stderr, "Error: Cannot allocate matrix with order 0\n");
        exit(1);
    }

    Matrix matrix = {
        .rows = rows,
        .columns = columns,
    };

    matrix.arr = malloc(rows * sizeof(double *));

    for (size_t i = 0; i < columns; i++) {
        matrix.arr[i] = malloc(sizeof(double));
    }
}

int addMatrices(Matrix *mat1, Matrix *mat2, Matrix *result) {
    if ((mat1->rows != mat2->rows) || (mat1->columns != mat2->columns))
        return 1;

    result->rows = mat1->rows;
    result->columns = mat2->rows;

    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->columns; j++) {
            result->arr[i][j] = mat1->arr[i][j] + mat2->arr[i][j];
        }
    }

    return 0;
}
