#pragma once

#include "set_utils.h"

typedef struct {
    double *coeffs;

    int degree;  // count
    int capacity;
} Polynomial;

Set *solveQuadratic(Polynomial *quadratic);
