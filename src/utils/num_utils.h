#pragma once

#include <gmp-x86_64.h>
#include <mpc.h>
#include <mpfr.h>
#include <stdbool.h>

typedef enum {
    NUM_REAL,
    NUM_COMPLEX,
    NUM_RATIONAL,
} NumberKind;

// TODO: consider bin and hex nums
// consider storing integers as mpz_t
typedef struct {
    NumberKind kind;

    union {
        mpfr_t real;
        mpc_t complex;
        mpq_t rational;
    };

} Number;

Number *numNew(NumberKind kind);
void numInit(Number *num, NumberKind kind);
void numSet(Number *dest, const Number *src);
Number *numConvertandSet(const Number *src, NumberKind kind);
void numConvert(Number *num, NumberKind kind);
NumberKind numPromoteKind(NumberKind kind1, NumberKind kind2);

Number *numAdd(const Number *a, const Number *b);
Number *numSubtract(const Number *a, const Number *b);
Number *numMultiply(const Number *a, const Number *b);
Number *numDivide(const Number *a, const Number *b);
int numCompare(const Number *a, const Number *b);
Number *numPow(const Number *base, const Number *exp);

bool numIsInteger(const Number *num);

long numToLong(const Number *num);

void numFree(Number *num);
