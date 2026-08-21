#pragma once

#include <gmp-x86_64.h>
#include <mpc.h>
#include <mpfr.h>
#include <stdbool.h>

#include "string_view_utils.h"

typedef struct Unit Unit;

// TODO: allow changing precision
#define PRECISION 10

// as rationals
typedef enum {
    NUM_REAL,
    NUM_COMPLEX,
    NUM_RATIONAL,
    NUM_BOOL,
    NUM_ERROR,
} NumberKind;

// TODO: consider bin and hex nums
typedef struct {
    NumberKind kind;

    union {
        mpfr_t real;       // NUM_REAL
        mpc_t complex;     // NUM_COMPLEX
        mpq_t rational;    // NUM_RATIONAL
        bool boolean;      // NUM_BOOL
        StringView error;  // NUM_ERROR
    };

} Number;

Number *numNew(NumberKind kind);
void numInit(Number *num, NumberKind kind);

void numSet(Number *dest, const Number *src);
void numSetBool(Number *num, bool boolean);
void numSetRealSd(Number *num, double val);
void numSetError(Number *num, const char *errorString, size_t errorLength);

Number *numConvertandSet(const Number *src, NumberKind kind);
Number *numConvert(Number *num, NumberKind kind);

NumberKind numPromoteKind(NumberKind kind1, NumberKind kind2);

Number *rationalStringToNum(const char *numer, const char *denom);
bool numIsInteger(const Number *num);
void numToInt(mpz_t result, const Number *num);
bool numCanBeLong(const Number *num);
int numCanBeUnsignedLong(const Number *num);
bool numCompCanBeReal(const Number *num);
bool numRatCanBeReal(const Number *num, Number *result);

long numToLong(const Number *num);
unsigned long numToUnsignedLong(const Number *num);

void numPrint(const Number *num);
void numToStringApi(char *out, size_t outSize, const Number *num,
                    const Unit **unit);
void numClear(Number *num);
void numFree(Number *num);
void numFrees(Number *num, ...);
