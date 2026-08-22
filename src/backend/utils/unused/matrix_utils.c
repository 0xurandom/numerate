#include "matrix_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "num_ops.h"
#include "num_utils.h"

Matrix* newMatrix(unsigned short rows, unsigned short columns) {
    Matrix* matrix = malloc(sizeof(Matrix));
    initMatrix(matrix, rows, columns);
    return matrix;
}

void initMatrix(Matrix* matrix, unsigned short rows, unsigned short columns) {
    if (rows == 0 || columns == 0) {
        fprintf(stderr, "Error: Cannot allocate matrix with order 0\n");
        exit(1);
    }

    matrix->rows = rows;
    matrix->columns = columns;

    matrix->arr = malloc(rows * sizeof(Number*));

    for (size_t i = 0; i < rows; i++) {
        matrix->arr[i] = malloc(columns * sizeof(Number));
        for (size_t j = 0; j < columns; j++) {
            numInit(&matrix->arr[i][j], NUM_REAL);
        }
    }
}

void initIdentityMatrix(Matrix* matrix, unsigned short order) {
    initMatrix(matrix, order, order);

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->columns; j++) {
            if (i == j)
                numSetRealSd(&matrix->arr[i][j], 1);
            else
                numSetRealSd(&matrix->arr[i][j], 0);
        }
    }
}

// func returns 0 if successful
int addMatrices(Matrix* mat1, Matrix* mat2, Matrix* result) {
    if ((mat1->rows != mat2->rows) || (mat1->columns != mat2->columns))
        return 1;

    initMatrix(result, mat1->rows, mat2->columns);

    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->columns; j++) {
            Number* tempResult = numAdd(&mat1->arr[i][j], &mat2->arr[i][j]);
            result->arr[i][j].kind = tempResult->kind;
            numSet(&result->arr[i][j], tempResult);
            numFree(tempResult);
        }
    }

    return 0;
}

int subtractMatrices(Matrix* mat1, Matrix* mat2, Matrix* result) {
    if ((mat1->rows != mat2->rows) || (mat1->columns != mat2->columns))
        return 1;

    initMatrix(result, mat1->rows, mat2->columns);

    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->columns; j++) {
            Number* tempResult =
                numSubtract(&mat1->arr[i][j], &mat2->arr[i][j]);
            result->arr[i][j].kind = tempResult->kind;
            numSet(&result->arr[i][j], tempResult);
            numFree(tempResult);
        }
    }

    return 0;
}

// mxn nxp
int multiplyMatrices(Matrix* mat1, Matrix* mat2, Matrix* result) {
    if (mat1->columns != mat2->rows) return 1;

    initMatrix(result, mat1->rows, mat2->columns);

    Number* product = numNew(NUM_BOOL);

    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->columns; j++) {
            Number* sum = numNew(NUM_REAL);
            numSetRealSd(sum, 0);

            for (size_t k = 0; k < mat1->columns; k++) {
                // multiply ith row and jth col
                numMultiplyInto(product, &mat1->arr[i][k], &mat2->arr[k][j]);
                Number* newSum = numAdd(sum, product);
                numFree(sum);
                sum = newSum;
            }
            numSet(&result->arr[i][j], sum);
            numFree(sum);
        }
    }

    numFree(product);

    return 0;
}

void getMatrixTranspose(Matrix* matrix, Matrix* result) {
    initMatrix(result, matrix->columns, matrix->rows);

    for (size_t i = 0; i < matrix->columns; i++) {
        for (size_t j = 0; j < matrix->rows; j++) {
            result->arr[i][j] = matrix->arr[j][i];
        }
    }
}

Number* getDeterminant(Matrix* matrix) {
    if (matrix->rows != matrix->columns) {
        Number* result = numNew(NUM_ERROR);
        char error[] = "Determinant is undefined for non square matrices";
        numSetError(result, error, strlen(error));
        return result;
    }

    if (matrix->rows == 1) {
        Number* determinant = numNew(matrix->arr[0][0].kind);
        numSet(determinant, &matrix->arr[0][0]);

        return determinant;
    } else if (matrix->rows == 2) {
        Number* ad = numMultiply(&matrix->arr[0][0], &matrix->arr[1][1]);
        Number* bc = numMultiply(&matrix->arr[0][1], &matrix->arr[1][0]);
        Number* determinant = numSubtract(ad, bc);

        numFree(ad);
        numFree(bc);

        return determinant;
    } else if (matrix->rows == 3) {
        Number *a = &matrix->arr[0][0], *b = &matrix->arr[0][1],
               *c = &matrix->arr[0][2];
        Number *d = &matrix->arr[1][0], *e = &matrix->arr[1][1],
               *f = &matrix->arr[1][2];
        Number *g = &matrix->arr[2][0], *h = &matrix->arr[2][1],
               *i = &matrix->arr[2][2];

        Number* ei = numMultiply(e, i);
        Number* fh = numMultiply(f, h);
        Number* t1sub = numSubtract(ei, fh);
        Number* t1 = numMultiply(a, t1sub);

        Number* di = numMultiply(d, i);
        Number* fg = numMultiply(f, g);
        Number* t2sub = numSubtract(di, fg);
        Number* t2 = numMultiply(b, t2sub);

        Number* dh = numMultiply(d, h);
        Number* eg = numMultiply(e, g);
        Number* t3sub = numSubtract(dh, eg);
        Number* t3 = numMultiply(c, t3sub);

        Number* sub = numSubtract(t1, t2);
        Number* determinant = numAdd(sub, t3);

        numFrees(ei, fh, t1sub, t1, di, fg, t2sub, t2, dh, eg, t3sub, t3, sub,
                 (Number*)NULL);

        return determinant;
    }

    Number* determinant = numNew(NUM_REAL);
    mpfr_set_ui(determinant->real, 0, MPFR_RNDN);

    for (unsigned short column = 0; column < matrix->rows; column++) {
        Number* cofactor = getCofactor(matrix, 0, column);
        Number* term = numMultiply(&matrix->arr[0][column], cofactor);

        Number* nthTerm = numAdd(determinant, term);
        numFree(determinant);
        determinant = nthTerm;

        numFree(term);
        numFree(cofactor);
    }

    return determinant;
}

Matrix* getMinorMatrix(Matrix* matrix, unsigned short row,
                       unsigned short column) {
    Matrix* minor = getSubmatrix(matrix, row, column);

    Number* minorDet = getDeterminant(minor);
    freeMatrix(minor);

    return minorDet;
}

Number* getCofactor(Matrix* matrix, unsigned short row, unsigned short column) {
    Number* minorDet = getMinorMatrix(matrix, row, column);

    if ((row + column) % 2 != 0) {
        Number* negativeDet = numNeg(minorDet);
        numFree(minorDet);
        return negativeDet;
    }

    return minorDet;
}

Matrix* getSubmatrix(Matrix* matrix, unsigned short row,
                     unsigned short column) {
    Matrix* subMat = newMatrix(matrix->rows - 1, matrix->columns - 1);

    int subI = 0;
    for (int i = 0; i < matrix->rows; i++) {
        if (i == row) continue;
        int subJ = 0;

        for (int j = 0; j < matrix->columns; j++) {
            if (j == column) continue;

            numSet(&subMat->arr[subI][subJ], &matrix->arr[i][j]);

            subJ++;
        }
        subI++;
    }

    return subMat;
}

void freeMatrix(Matrix* matrix) {
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->columns; j++) {
            numClear(&matrix->arr[i][j]);
        }
        free(matrix->arr[i]);
    }

    matrix->rows = 0;
    matrix->columns = 0;

    free(matrix->arr);
    free(matrix);
}
