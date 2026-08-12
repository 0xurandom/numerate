#include "equation_utils.h"

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "set_utils.h"

// TODO: use newton rhapson for equation solving

Polynomial* newPolynomial() {}

// ax^2 + bx + c = 0;
Set* solveQuadratic(Polynomial* quadratic) {
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

        Set* set = newSet();
        insertElement(set, x);
        insertElement(set, y);

        return set;

    } else if (determinant == 0) {
        double x = (-b) / (2 * a);

        Set* set = newSet();
        insertElement(set, x);

        return set;

    } else {
        double complex complexDeterminant = determinant + (0 * I);
        double complex x = (-b + csqrt(complexDeterminant)) / (2 * a);
        double complex y = (-b - csqrt(complexDeterminant)) / (2 * a);

        // TODO
    }
}

// ax^3 + bx^2 + cx + d = 0
Set* getCubicRoots(Polynomial* cubic) {
    if (cubic->degree != 3) {
        fprintf(stderr, "Error: getCubicRoots received a non cubic\n");
        exit(1);
    }

    double a = cubic->coeffs[0];
    double b = cubic->coeffs[1];
    double c = cubic->coeffs[2];
    double d = cubic->coeffs[3];

    double d0 = pow(b, 2) - (3 * a * c);
    double d1 = (2 * pow(b, 3)) - (9 * a * b * c) + (27 * pow(a, 2) * d);

    double DSquared = pow(d1, 2) - (4 * pow(d0, 3));

    double complex C1 = 0, C2 = 0, C3 = 0;
    double complex x = 0, y = 0, z = 0;

    double unityCbrt = (-1 + sqrt(3) * I) / 2;

    // D is complex hence numerator of c is complex
    if (DSquared < 0) {
        double complex DsquaredComplex = DSquared + (0 * I);
        double complex D = csqrt(DsquaredComplex);

        C1 = ccbrt((d1 + D) / 2);
        C2 = unityCbrt * C1;
        C3 = cpow(unityCbrt, 2) * C1;

    } else {
        double D = sqrt(DSquared);

        C1 = ccbrt(((double complex)(d1 + D)) / 2);
        C2 = unityCbrt * C1;
        C3 = cpow(unityCbrt, 2) * C1;
    }

    if ((d0 == 0) && (d1 == 0)) {
        x = (-1 / (3 * a)) * (b + C1);
        y = (-1 / (3 * a)) * (b + C2);
        z = (-1 / (3 * a)) * (b + C3);
    } else {
        x = (-1 / (3 * a)) * (b + C1 + (d0 / C1));
        y = (-1 / (3 * a)) * (b + C2 + (d0 / C2));
        z = (-1 / (3 * a)) * (b + C3 + (d0 / C3));
    }

    // TODO: sets dont support double complexes
}
