#include "complex_utils.h"

#include <math.h>
#include <stdlib.h>

// TODO: chnage result from parameter
// to making a new complex num

// TODO: use complex.h

ComplexNum *newComplexNum() {
    ComplexNum *num = malloc(sizeof(ComplexNum));
    initComplexNum(num);

    return num;
}

void initComplexNum(ComplexNum *num) {
    num->real = 0;
    num->imag = 0;
}

void getComplexSqrt(double num, ComplexNum *result) {
    result->real = 0;
    result->imag = sqrt(num);
}

void getComplexCbrts(double num, ComplexNum *result1, ComplexNum *result2) {
    // TODO
}

void getCbrtofComplex(ComplexNum *num, ComplexNum *result1, ComplexNum *result2,
                      ComplexNum *result3) {
    double magnitude = getMagnitude(num);
    double argument = getArgument(num);

    result1->real = (cbrt(magnitude)) * cos((argument / 3));
    result1->imag = (cbrt(magnitude)) * sin((argument / 3));

    result2->real = (cbrt(magnitude)) * cos(((argument + (2 * M_PI)) / 3));
    result2->imag = (cbrt(magnitude)) * sin(((argument + (2 * M_PI)) / 3));

    result3->real = (cbrt(magnitude)) * cos(((argument + (4 * M_PI)) / 3));
    result3->imag = (cbrt(magnitude)) * sin(((argument + (4 * M_PI)) / 3));
}

void addComplexNums(ComplexNum *num1, ComplexNum *num2, ComplexNum *result) {
    result->real = num1->real + num2->real;
    result->imag = num1->imag + num2->imag;
}

void subtractComplexNums(ComplexNum *num1, ComplexNum *num2,
                         ComplexNum *result) {
    result->real = num1->real - num2->real;
    result->imag = num1->imag - num2->imag;
}

void multiplyComplexNums(ComplexNum *num1, ComplexNum *num2,
                         ComplexNum *result) {
    result->real = (num1->real * num2->real) - (num1->imag * num2->imag);
    result->imag = (num1->real * num2->imag) + (num1->imag * num2->real);
}

void raiseComplexNum(ComplexNum *num, size_t power, ComplexNum *result) {
    ComplexNum originalNum = {
        .real = num->real,
        .imag = num->imag,
    };

    for (size_t i = 0; i < power; i++) {
        multiplyComplexNums(num, &originalNum, num);
    }
}

void divideComplexNums(ComplexNum *num1, ComplexNum *num2, ComplexNum *result) {
    result->real = ((num1->real * num2->real) + (num1->imag * num2->imag)) /
                   (pow(num2->real, 2) + pow(num2->imag, 2));
    result->imag = ((num1->imag * num2->real) + (num1->real * num2->imag)) /
                   (pow(num2->real, 2) + pow(num2->imag, 2));
}

void conjugateComplexNum(ComplexNum *num) { num->imag = -num->imag; }

double getArgument(ComplexNum *num) { return atan(num->imag / num->real); }

double getMagnitude(ComplexNum *num) {
    return (pow(num->real, 2) + pow(num->imag, 2));
}

void rotateComplexNum(ComplexNum *num, double rad) {
    ComplexNum temp = {
        .real = num->real,
        .imag = num->imag,
    };

    num->real = (temp.real * cos(rad)) - (temp.imag * sin(rad));
    num->imag = (temp.real * sin(rad)) + (temp.imag * cos(rad));
}
