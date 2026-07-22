#pragma once

#include <gmp-x86_64.h>
#include <mpc.h>
#include <mpfr.h>

typedef enum {
    NUM_REAL,
    NUM_COMPLEX,
    NUM_RATIONAL,
} NumberKind;

// TODO: consider bin and hex nums
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
void numFree(Number *num);
