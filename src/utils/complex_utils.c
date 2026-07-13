#include "complex_utils.h"

void initComplexNum(ComplexNum *num) {
    num->real = 0;
    num->imag = 0;
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
