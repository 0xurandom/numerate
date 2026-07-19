#pragma once

#include <complex.h>
#include <math.h>
#include <stddef.h>

typedef struct {
    double real;
    double imag;
} ComplexNum;

double complex ccbrt(double complex num);
void initComplexNum(ComplexNum *num);
void addComplexNums(ComplexNum *num1, ComplexNum *num2, ComplexNum *result);
void subtractComplexNums(ComplexNum *num1, ComplexNum *num2,
                         ComplexNum *result);
void multiplyComplexNums(ComplexNum *num1, ComplexNum *num2,
                         ComplexNum *result);
void raiseComplexNum(ComplexNum *num, size_t power, ComplexNum *result);
void divideComplexNums(ComplexNum *num1, ComplexNum *num2, ComplexNum *result);
void conjugateComplexNum(ComplexNum *num);
double getArgument(ComplexNum *num);
double getMagnitude(ComplexNum *num);
void rotateComplexNum(ComplexNum *num, double rad);
