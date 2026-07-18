#include "equation_utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: use newton rhapson for equation solving

Polynomial *newPolynomial() {}

// ax^2 + bx + c = 0;
Set *solveQuadratic(Polynomial *quadratic) {
    if (quadratic->degree != 2) {
        fprintf(stderr, "Error: solveQuadratic received a non quadratic\n");
        exit(1);
    }

    double a = quadratic->coeffs[0];
    double b = quadratic->coeffs[1];
    double c = quadratic->coeffs[2];

    double determinant = pow(b, 2) - (4 * a * c);

    if (determinant > 0) {
        double x = (-b + sqrt(determinant)) / (2 * a);
        double y = (-b - sqrt(determinant)) / (2 * a);

        Set *set = newSet();
        insertElement(set, x);
        insertElement(set, y);

        return set;

    } else if (determinant == 0) {
        double x = (-b) / (2 * a);

        Set *set = newSet();
        insertElement(set, x);

        return set;

    } else {
        // has complex root
        // TODO: use better number type for sets
        // which can be a complex num
    }
}
