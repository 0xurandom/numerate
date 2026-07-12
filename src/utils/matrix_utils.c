#include "matrix_utils.h"

#include <stdio.h>
#include <stdlib.h>

void initMatrix(Matrix *matrix, unsigned short rows, unsigned short columns) {
    if (rows == 0 || columns == 0) {
        fprintf(stderr, "Error: Cannot allocate matrix with order 0\n");
        exit(1);
    }

    matrix->arr = malloc(rows * sizeof(double *));

    for (size_t i = 0; i < columns; i++) {
        matrix->arr[i] = malloc(sizeof(double));
    }
}

// func returns 0 if successful
int addMatrices(Matrix *mat1, Matrix *mat2, Matrix *result) {
    if ((mat1->rows != mat2->rows) || (mat1->columns != mat2->columns))
        return 1;

    initMatrix(result, mat1->rows, mat2->columns);

    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->columns; j++) {
            result->arr[i][j] = mat1->arr[i][j] + mat2->arr[i][j];
        }
    }

    return 0;
}

int subtractMatrices(Matrix *mat1, Matrix *mat2, Matrix *result) {
    if ((mat1->rows != mat2->rows) || (mat1->columns != mat2->columns))
        return 1;

    initMatrix(result, mat1->rows, mat2->columns);

    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->columns; j++) {
            result->arr[i][j] = mat1->arr[i][j] - mat2->arr[i][j];
        }
    }

    return 0;
}

// mxn nxp
int multiplyMatrices(Matrix *mat1, Matrix *mat2, Matrix *result) {
    if (mat1->columns != mat2->rows) return 1;

    initMatrix(result, mat1->rows, mat2->columns);

    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->columns; j++) {
            double sum = 0;
            // TODO
            for (size_t k = 0; k < mat1->columns; k++) {
                // multiply ith row and jth col
                sum += mat1->arr[i][k] * mat2->arr[k][j];
            }
            result->arr[i][j] = sum;
        }
    }

    return 0;
}

void getMatrixTranspose(Matrix *matrix, Matrix *result) {
    initMatrix(matrix, matrix->columns, matrix->rows);

    for (size_t i = 0; i < matrix->columns; i++) {
        for (size_t j = 0; j < matrix->rows; j++) {
            result->arr[i][j] = matrix->arr[j][i];
        }
    }
}

double getDeterminant(Matrix *matrix) {}

void getCofactor(Matrix *matrix, unsigned short row, unsigned short column,
                 Matrix *result) {}
