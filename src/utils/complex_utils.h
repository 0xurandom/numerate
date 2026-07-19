#pragma once

#include <complex.h>
#include <math.h>
#include <stddef.h>

void getSqrtofComplex(double complex num, double complex* result1,
                      double complex* result2);

double complex ccbrt(double complex num);

void getCbrtofComplex(double complex num, double complex* result1,
                      double complex* result2, double complex* result3);
