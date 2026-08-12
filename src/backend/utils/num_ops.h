#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "num_utils.h"

void numAddInto(Number* out, const Number* a, const Number* b);
Number* numAdd(const Number* a, const Number* b);
void numSubtractInto(Number* out, const Number* a, const Number* b);
Number* numSubtract(const Number* a, const Number* b);
void numMultiplyInto(Number* out, const Number* a, const Number* b);
Number* numMultiply(const Number* a, const Number* b);
void numDivideInto(Number* out, const Number* a, const Number* b);
Number* numDivide(const Number* a, const Number* b);

int numCompare(const Number* a, const Number* b);
void numNegInto(Number* out, const Number* num);
Number* numNeg(const Number* num);
void numFloorInto(Number* out, const Number* num);
Number* numFloor(const Number* num);
void numCeilInto(Number* out, const Number* num);
Number* numCeil(const Number* num);

Number* numFact(const Number* num);
void numSubfactInto(Number* out, const Number* num);
Number* numSubfact(const Number* num);
void numGammaInto(Number* out, const Number* num);
Number* numGamma(const Number* num);

static Number* complexGamma(const Number* num);
static Number* SpougeApprox(const Number* num);
static Number* eulerReflection(const Number* num);

void numSinInto(Number* out, const Number* num);
Number* numSin(const Number* num);
void numSinhInto(Number* out, const Number* num);
Number* numSinh(const Number* num);
void numCosInto(Number* out, const Number* num);
Number* numCos(const Number* num);
void numCoshInto(Number* out, const Number* num);
Number* numCosh(const Number* num);
void numTanInto(Number* out, const Number* num);
Number* numTan(const Number* num);
void numTanhInto(Number* out, const Number* num);
Number* numTanh(const Number* num);
void numCosecInto(Number* out, const Number* num);
Number* numCosec(const Number* num);
void numCosechInto(Number* out, const Number* num);
Number* numCosech(const Number* num);
void numSecInto(Number* out, const Number* num);
Number* numSec(const Number* num);
void numSechInto(Number* out, const Number* num);
Number* numSech(const Number* num);
void numCotInto(Number* out, const Number* num);
Number* numCot(const Number* num);
void numCothInto(Number* out, const Number* num);
Number* numCoth(const Number* num);

int numSgnSi(const Number* num);
void numSgnInto(Number* out, const Number* num);
Number* numSgn(const Number* num);
bool numIsUnity(const Number* num);
void numAbsInto(Number* out, const Number* num);
Number* numAbs(const Number* num);
void numPowInto(Number* out, const Number* base, const Number* exp);
Number* numPow(const Number* base, const Number* exp);
void numSqrtInto(Number* out, const Number* num);
Number* numSqrt(const Number* num);
void numCbrtInto(Number* out, const Number* num);
Number* numCbrt(const Number* num);
void numExpInto(Number* out, const Number* num);
Number* numExp(const Number* num);
void numLnInto(Number* out, const Number* num);
Number* numLn(const Number* num);
void numLogInto(Number* out, const Number* num);
Number* numLog(const Number* num);

void numNprInto(Number* out, const Number* n, const Number* r);
Number* numNpr(const Number* n, const Number* r);
void numNcrInto(Number* out, const Number* n, const Number* r);
Number* numNcr(const Number* n, const Number* r);

void numShiftRightInto(Number* out, const Number* num, const Number* bits);
Number* numShiftRight(const Number* num, const Number* bits);
void numShiftLeftInto(Number* out, const Number* num, const Number* bits);
Number* numShiftLeft(const Number* num, const Number* bits);
void numAndInto(Number* out, const Number* a, const Number* b);
Number* numAnd(const Number* a, const Number* b);
void numOrInto(Number* out, const Number* a, const Number* b);
Number* numOr(const Number* a, const Number* b);