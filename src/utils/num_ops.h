#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "num_utils.h"

Number *numAdd(const Number *a, const Number *b);
Number *numSubtract(const Number *a, const Number *b);
Number *numMultiply(const Number *a, const Number *b);
Number *numDivide(const Number *a, const Number *b);

int numCompare(const Number *a, const Number *b);
Number *numNeg(const Number *num);
Number *numFloor(const Number *num);
Number *numCeil(const Number *num);
Number *numFact(const Number *num);

Number *numSin(const Number *num);
Number *numSinh(const Number *num);
Number *numCos(const Number *num);
Number *numCosh(const Number *num);
Number *numTan(const Number *num);
Number *numTanh(const Number *num);
Number *numCosec(const Number *num);
Number *numCosech(const Number *num);
Number *numSec(const Number *num);
Number *numSech(const Number *num);
Number *numCot(const Number *num);
Number *numCoth(const Number *num);

int numSgnSi(const Number *num);
Number *numSgn(const Number *num);
bool numIsUnity(const Number *num);
Number *numAbs(const Number *num);
Number *numPow(const Number *base, const Number *exp);
Number *numExp(const Number *num);
Number *numLn(const Number *num);
Number *numLog(const Number *num);
Number *numNpr(const Number *n, const Number *r);
Number *numNcr(const Number *n, const Number *r);
