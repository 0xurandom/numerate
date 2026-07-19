#include "complex_utils.h"

void getSqrtofComplex(double complex num, double complex* result1,
                      double complex* result2) {
    double magnitude = cabs(num);
    double argument = carg(num);

    *result1 = csqrt(num);
    *result2 = -(*result1);
}

double complex ccbrt(double complex num) { return cexp(clog(num) / 3); }

void getCbrtofComplex(double complex num, double complex* result1,
                      double complex* result2, double complex* result3) {
    double magnitude = cabs(num);
    double argument = carg(num);

    *result1 = (cbrt(magnitude)) * cos((argument / 3)) +
               (cbrt(magnitude)) * sin((argument / 3)) * I;

    *result2 = (cbrt(magnitude)) * cos(((argument + (2 * M_PI)) / 3)) +
               (cbrt(magnitude)) * sin(((argument + (2 * M_PI)) / 3)) * I;

    *result3 = (cbrt(magnitude)) * cos(((argument + (4 * M_PI)) / 3)) +
               (cbrt(magnitude)) * sin(((argument + (4 * M_PI)) / 3)) * I;
}
