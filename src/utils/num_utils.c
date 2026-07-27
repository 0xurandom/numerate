#include "num_utils.h"

#include <gmp-x86_64.h>
#include <limits.h>
#include <mpc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_view_utils.h"

// TODO: allow changing precision
#define PRECISION 10

// TODO: check for functions using MPFR_RNDN instead of MPC_RNDNN

// TODO: before calculaing an equation, check if NUM_RATIONAL can
// be converted into a NUM_REAL

Number *numNew(NumberKind kind) {
    Number *num = malloc(sizeof(Number));
    numInit(num, kind);

    return num;
}

// TODO: add num set

void numInit(Number *num, NumberKind kind) {
    num->kind = kind;

    switch (kind) {
        case NUM_REAL: {
            mpfr_init2(num->real, PRECISION);
            break;
        }

        case NUM_COMPLEX: {
            mpc_init2(num->complex, PRECISION);
            break;
        }

        case NUM_RATIONAL: {
            mpq_init(num->rational);
            break;
        }
    }
}

// expects dest to be from numNew and for dest
// to have the same NumberKind as src
void numSet(Number *dest, const Number *src) {
    if (dest->kind != src->kind) {
        fprintf(stderr,
                "Error: numCopy received dest and src of different kinds\n");
        exit(1);
    }

    switch (src->kind) {
        case NUM_REAL: {
            mpfr_set(dest->real, src->real, MPFR_RNDN);
            break;
        }

        case NUM_COMPLEX: {
            mpc_set(dest->complex, src->complex, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpq_set(dest->rational, src->rational);
            break;
        }
    }

    return;
}

// needs a Number of kind NUM_ERROR
void numSetError(Number *num, char *errorString, size_t errorLength) {
    if (num->kind != NUM_ERROR) {
        fprintf(stderr,
                "numSetError received a Number not of kind NUM_ERROR\n");
        exit(1);
    }

    setStringView(&num->error, errorString, errorLength);
}

// inits x and sets x = src with NumberKind kind
Number *numConvertandSet(const Number *src, NumberKind kind) {
    Number *num = numNew(src->kind);

    if (src->kind == kind) {
        numSet(num, src);

    } else {
        Number *temp = numNew(src->kind);
        numSet(temp, src);

        numConvert(temp, kind);

        numSet(num, temp);

        numFree(temp);
    }

    return num;
}

void numConvert(Number *num, NumberKind kind) {
    // TODO: result can be replaced here?
    if (num->kind == kind) return;

    switch (kind) {
        case NUM_REAL: {
            mpfr_t result;
            mpfr_init2(result, PRECISION);

            switch (num->kind) {
                case NUM_REAL:
                    break;

                case NUM_COMPLEX: {
                    mpfr_t tempImag;
                    mpfr_init2(tempImag, PRECISION);

                    mpfr_set(tempImag, mpc_imagref(num->complex), MPFR_RNDN);

                    if (mpfr_cmp_si(tempImag, 0) == 0) {
                        fprintf(stderr,
                                "Warning: Converting complex number with "
                                "imaginary value to real number\n");
                    }

                    mpfr_set(result, mpc_realref(num->complex), MPFR_RNDN);

                    mpfr_clear(tempImag);
                    break;
                }

                case NUM_RATIONAL: {
                    mpfr_set_q(result, num->rational, MPFR_RNDN);

                    break;
                }
            }

            numFree(num);

            num->kind = NUM_REAL;
            mpfr_init2(num->real, PRECISION);
            mpfr_set(num->real, result, MPFR_RNDN);

            mpfr_clear(result);
            break;
        }

        case NUM_COMPLEX: {
            mpc_t result;
            mpc_init2(result, PRECISION);

            switch (num->kind) {
                case NUM_COMPLEX:
                    break;

                case NUM_REAL: {
                    mpfr_set(mpc_realref(result), num->real, MPFR_RNDN);

                    break;
                }

                case NUM_RATIONAL: {
                    mpfr_set_q(mpc_realref(result), num->rational, MPFR_RNDN);
                    break;
                }
            }
            numFree(num);

            num->kind = NUM_COMPLEX;
            mpc_init2(num->complex, PRECISION);
            mpc_set(num->complex, result, MPFR_RNDN);

            mpc_clear(result);
            break;
        }

        case NUM_RATIONAL: {
            mpq_t result;
            mpq_init(result);

            switch (num->kind) {
                case NUM_RATIONAL:
                    break;

                case NUM_REAL: {
                    mpfr_get_q(result, num->real);

                    break;
                }

                case NUM_COMPLEX: {
                    mpfr_t tempImag;
                    mpfr_init2(tempImag, PRECISION);

                    mpfr_set(tempImag, mpc_imagref(num->complex), MPFR_RNDN);

                    if (mpfr_cmp_si(tempImag, 0) == 0) {
                        fprintf(stderr,
                                "Warning: Converting complex number with "
                                "imaginary value to rational number\n");
                    }

                    mpfr_get_q(result, mpc_realref(num->complex));

                    mpfr_clear(tempImag);
                    break;
                }
            }
            numFree(num);

            num->kind = NUM_RATIONAL;
            mpq_init(num->rational);
            mpq_set(num->rational, result);

            mpq_clear(result);
            break;
        }
    }
}

// rational > real > complex
NumberKind numPromoteKind(NumberKind kind1, NumberKind kind2) {
    if (kind1 == NUM_COMPLEX || kind2 == NUM_COMPLEX)
        return NUM_COMPLEX;
    else if (kind1 == NUM_REAL || kind2 == NUM_REAL)
        return NUM_REAL;
    else
        return NUM_RATIONAL;
}

// inits a number and sets it to the sum
Number *numAdd(const Number *a, const Number *b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number *tempA = numConvertandSet(a, promotedKind);
    Number *tempB = numConvertandSet(b, promotedKind);

    Number *result = numNew(promotedKind);

    switch (promotedKind) {
        case NUM_COMPLEX: {
            mpc_add(result->complex, tempA->complex, tempB->complex, MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            mpfr_add(result->real, tempA->real, tempB->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpq_add(result->rational, tempA->rational, tempB->rational);
            break;
        }
    }

    numFree(tempA);
    numFree(tempB);

    return result;
}

Number *numSubtract(const Number *a, const Number *b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number *tempA = numConvertandSet(a, promotedKind);
    Number *tempB = numConvertandSet(b, promotedKind);

    Number *result = numNew(promotedKind);

    switch (promotedKind) {
        case NUM_COMPLEX: {
            mpc_sub(result->complex, tempA->complex, tempB->complex, MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            mpfr_sub(result->real, tempA->real, tempB->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpq_sub(result->rational, tempA->rational, tempB->rational);
            break;
        }
    }

    numFree(tempA);
    numFree(tempB);

    return result;
}

Number *numMultiply(const Number *a, const Number *b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number *tempA = numConvertandSet(a, promotedKind);
    Number *tempB = numConvertandSet(b, promotedKind);

    Number *result = numNew(promotedKind);

    switch (promotedKind) {
        case NUM_COMPLEX: {
            mpc_mul(result->complex, tempA->complex, tempB->complex, MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            mpfr_mul(result->real, tempA->real, tempB->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpq_mul(result->rational, tempA->rational, tempB->rational);
            break;
        }
    }

    numFree(tempA);
    numFree(tempB);

    return result;
}

Number *numDivide(const Number *a, const Number *b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number *tempA = numConvertandSet(a, promotedKind);
    Number *tempB = numConvertandSet(b, promotedKind);

    Number *result = numNew(promotedKind);

    switch (promotedKind) {
        case NUM_COMPLEX: {
            mpc_div(result->complex, tempA->complex, tempB->complex, MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            mpfr_div(result->real, tempA->real, tempB->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpq_div(result->rational, tempA->rational, tempB->rational);
            break;
        }
    }

    numFree(tempA);
    numFree(tempB);

    return result;
}

// compares numbers and the magnitude of complex nums
// returns 0 if equal, 1 if a > b, -1 if a < b
int numCompare(const Number *a, const Number *b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number *tempA = numConvertandSet(a, promotedKind);
    Number *tempB = numConvertandSet(b, promotedKind);

    int result = 0;

    switch (promotedKind) {
        case NUM_COMPLEX: {
            Number *tempAmagnitude = numNew(NUM_REAL);
            Number *tempBmagnitude = numNew(NUM_REAL);

            mpc_abs(tempAmagnitude->real, tempA->complex, MPFR_RNDN);
            mpc_abs(tempBmagnitude->real, tempA->complex, MPFR_RNDN);

            result = mpfr_cmp(tempAmagnitude->real, tempBmagnitude->real);

            numFree(tempAmagnitude);
            numFree(tempBmagnitude);

            break;
        }

        case NUM_REAL: {
            result = mpfr_cmp(tempA->real, tempB->real);
            break;
        }

        case NUM_RATIONAL: {
            result = mpq_cmp(tempA->rational, tempB->rational);
            break;
        }
    }

    numFree(tempA);
    numFree(tempB);

    return result;
}

Number *numNeg(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_neg(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_COMPLEX);
            mpfr_neg(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *result = numNew(NUM_RATIONAL);
            mpq_neg(result->rational, num->rational);

            return result;
        }
    }
}

Number *numFloor(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);

            mpfr_floor(mpc_realref(result->complex), mpc_realref(num->complex));
            mpfr_floor(mpc_imagref(result->complex), mpc_imagref(num->complex));

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);

            mpfr_floor(result->real, num->real);

            return result;
        }

        case NUM_RATIONAL: {
            Number *result = numNew(NUM_REAL);

            mpz_t tempInt;
            mpz_init(tempInt);

            mpz_fdiv_q(tempInt, mpq_numref(num->rational),
                       mpq_denref(num->rational));
            mpfr_set_z(result->real, tempInt, MPFR_RNDN);

            mpz_clear(tempInt);

            return result;
        }
    }
}

Number *numCeil(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);

            mpfr_ceil(mpc_realref(result->complex), mpc_realref(num->complex));
            mpfr_ceil(mpc_imagref(result->complex), mpc_imagref(num->complex));

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);

            mpfr_ceil(result->real, num->real);

            return result;
        }

        case NUM_RATIONAL: {
            Number *result = numNew(NUM_REAL);

            mpz_t tempInt;
            mpz_init(tempInt);

            mpz_cdiv_q(tempInt, mpq_numref(num->rational),
                       mpq_denref(num->rational));
            mpfr_set_z(result->real, tempInt, MPFR_RNDN);

            mpz_clear(tempInt);

            return result;

            break;
        }
    }
}

// TODO: this needs gamma function for complex nums
Number *numFact(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            break;
        }

        case NUM_REAL: {
            if (!numCanBeLong(num)) {
                fprintf(stderr, "Error: Factorial is too large to compute\n");
                exit(1);
            }

            Number *result = numNew(NUM_REAL);
            mpfr_t numPlusOne;
            mpfr_init2(numPlusOne, MPFR_RNDN);
            mpfr_add_ui(numPlusOne, num->real, 1, MPFR_RNDN);

            if (mpfr_integer_p(numPlusOne) && (mpfr_sgn(numPlusOne) <= 0)) {
                fprintf(stderr,
                        "Error: Factorial is undefined for this value\n");
                exit(1);
            }

            mpfr_gamma(result->real, numPlusOne, MPFR_RNDN);

            mpfr_clear(numPlusOne);
            return result;
        }

        case NUM_RATIONAL: {
            // TODO: this may be redundant if NUM_RATIONAL is
            // automatically promoted to NUM_REAL

            if (numIsInteger(num) && numSgn(num) >= 0) {
                mpz_t tempNumer;
                mpz_init(tempNumer);

                mpz_set(tempNumer, mpq_numref(num->rational));

                if (!mpz_fits_ulong_p) {
                    fprintf(stderr,
                            "Error: Factorial is too large to compute\n");
                    exit(1);
                }

                unsigned long numerLong = mpz_get_ui(tempNumer);
                mpz_clear(tempNumer);
            }

            Number *result = numNew(NUM_REAL);

            return result;
        }
    }
}

Number *numSin(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_sin(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_sin(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numSin(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numSinh(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_sinh(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_sin(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numSinh(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numCos(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_cos(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_cos(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCos(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numCosh(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_cosh(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_cosh(result->real, num->real, MPC_RNDNN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCosh(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numTan(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_tan(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_COMPLEX);
            mpfr_tan(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numTan(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numTanh(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_tanh(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_tanh(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numTanh(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numCosec(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempSin;
            mpc_init2(tempSin, PRECISION);
            mpc_sin(tempSin, num->complex, MPC_RNDNN);

            if (mpc_cmp_si_si(tempSin, 0, 0) == 0) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Cosec is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_COMPLEX);
            mpc_ui_div(result->complex, 1, tempSin, MPC_RNDNN);
            mpc_clear(tempSin);

            return result;
        }

        case NUM_REAL: {
            mpfr_t tempSin;
            mpfr_init2(tempSin, PRECISION);
            mpfr_sin(tempSin, num->real, MPFR_RNDN);

            if (mpfr_cmp_si(tempSin, 0) == 0) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Cosec is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_REAL);
            mpfr_ui_div(result->real, 1, tempSin, MPFR_RNDN);
            mpfr_clear(tempSin);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCosec(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numCosech(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempSinh;
            mpc_init2(tempSinh, PRECISION);
            mpc_sinh(tempSinh, num->complex, MPC_RNDNN);

            if (mpc_cmp_si_si(tempSinh, 0, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Cosech is undefined for this value";
                numSetError(result, error, strlen(error));
            }
            Number *result = numNew(NUM_REAL);
            mpc_ui_div(result->complex, 1, tempSinh, MPC_RNDNN);
            mpc_clear(tempSinh);

            return result;
        }

        case NUM_REAL: {
            mpfr_t tempSinh;
            mpfr_init2(tempSinh, PRECISION);
            mpfr_sinh(tempSinh, num->real, MPFR_RNDN);

            if (mpfr_cmp_si(tempSinh, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Cosec is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_REAL);
            mpfr_ui_div(result->real, 1, tempSinh, MPFR_RNDN);
            mpfr_clear(tempSinh);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCosech(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numSec(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempCos;
            mpc_init2(tempCos, PRECISION);
            mpc_cos(tempCos, num->complex, MPC_RNDNN);

            if (mpc_cmp_si_si(tempCos, 0, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Sec is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_COMPLEX);
            mpc_ui_div(result->complex, 1, tempCos, MPC_RNDNN);
            mpc_clear(tempCos);

            return result;
        }

        case NUM_REAL: {
            mpfr_t tempCos;
            mpfr_init2(tempCos, PRECISION);
            mpfr_cos(tempCos, num->real, MPFR_RNDN);

            if (mpfr_cmp_si(tempCos, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Sec is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_REAL);
            mpfr_ui_div(result->real, 1, tempCos, MPFR_RNDN);
            mpfr_clear(tempCos);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numSec(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numSech(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempCosh;
            mpc_init2(tempCosh, PRECISION);
            mpc_cosh(tempCosh, num->complex, MPC_RNDNN);

            if (mpc_cmp_si_si(tempCosh, 0, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Sech is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_COMPLEX);
            mpc_ui_div(result->complex, 1, tempCosh, MPC_RNDNN);
            mpc_clear(tempCosh);

            return result;
        }

        case NUM_REAL: {
            mpfr_t tempCosh;
            mpfr_init2(tempCosh, PRECISION);
            mpfr_cosh(tempCosh, num->real, MPFR_RNDN);

            if (mpfr_cmp_si(tempCosh, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Sech is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_RATIONAL);
            mpfr_ui_div(result->real, 1, tempCosh, MPFR_RNDN);
            mpfr_clear(tempCosh);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numSech(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numCot(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempTan;
            mpc_init2(tempTan, PRECISION);
            mpc_tan(tempTan, num->complex, MPC_RNDNN);

            if (mpc_cmp_si_si(tempTan, 0, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Cot is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_COMPLEX);
            mpc_ui_div(result->complex, 1, tempTan, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            mpfr_t tempTan;
            mpfr_init2(tempTan, PRECISION);
            mpfr_tan(tempTan, num->real, MPFR_RNDN);

            if (mpfr_cmp_si(tempTan, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Cot is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_REAL);
            mpfr_ui_div(result->real, 1, tempTan, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCot(realNum);
            numFree(realNum);

            return result;
        }
    }
}

Number *numCoth(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempTanh;
            mpc_init2(tempTanh, PRECISION);
            mpc_tanh(tempTanh, num->complex, MPC_RNDNN);

            if (mpc_cmp_si_si(tempTanh, 0, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Coth is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_COMPLEX);
            mpc_ui_div(result->complex, 1, tempTanh, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            mpfr_t tempTanh;
            mpfr_init2(tempTanh, PRECISION);
            mpfr_tanh(tempTanh, num->real, MPFR_RNDN);

            if (mpfr_cmp_si(tempTanh, 0)) {
                Number *result = numNew(NUM_ERROR);
                char error[] = "Coth is undefined for this value";
                numSetError(result, error, strlen(error));
            }

            Number *result = numNew(NUM_REAL);
            mpfr_ui_div(result->real, 1, tempTanh, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCoth(realNum);
            numFree(realNum);

            return result;
        }
    }
}

int numSgn(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            // TODO: define the signum func
            // for comples nums
            break;
        }

        case NUM_REAL: {
            return mpfr_sgn(num->real);
        }

        case NUM_RATIONAL: {
            return mpq_sgn(num->rational);
        }
    }
}

// returns modulus of complex num
// and abs for others
Number *numAbs(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_REAL);
            mpc_abs(result->real, num->complex, MPFR_RNDN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_abs(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *result = numNew(NUM_RATIONAL);
            mpq_abs(result->rational, num->rational);

            return result;
        }
    }
}

Number *numPow(const Number *base, const Number *exp) {
    // exp is an integer, keep base's kind the same
    if (numIsInteger(exp)) {
        // TODO: add exception to fllowing error for zero, one and inf
        if (!numCanBeLong(exp)) {
            fprintf(stderr, "Error: Exponent is too long to compute\n");
            exit(1);
        }

        long expLong = numToLong(exp);

        switch (base->kind) {
            case NUM_COMPLEX: {
                if (mpc_cmp_si(base->complex, 0) == 0 && expLong < 0) {
                    fprintf(stderr,
                            "Error: Cannot raise zero to a -ve power\n");
                    exit(1);
                }

                Number *result = numNew(NUM_COMPLEX);
                mpc_pow_si(result->complex, base->complex, expLong, MPC_RNDNN);

                return result;
            }

            case NUM_REAL: {
                if (mpfr_zero_p(base->real) && expLong < 0) {
                    fprintf(stderr,
                            "Error: Cannot raise zero to a -ve power\n");
                    exit(1);
                }

                Number *result = numNew(NUM_REAL);
                mpfr_pow_si(result->real, base->real, expLong, MPFR_RNDN);

                return result;
            }

            case NUM_RATIONAL: {
                if (mpq_sgn(base->rational) == 0 && expLong < 0) {
                    fprintf(stderr,
                            "Error: Cannot raise zero to a -ve power\n");
                    exit(1);
                }

                Number *result = numNew(NUM_RATIONAL);

                if (expLong >= 0) {
                    mpz_t numer, denom;
                    mpz_init(numer);
                    mpz_init(denom);

                    mpz_pow_ui(numer, mpq_numref(base->rational), expLong);
                    mpz_pow_ui(denom, mpq_denref(base->rational), expLong);

                    mpq_set_num(result->rational, numer);
                    mpq_set_den(result->rational, denom);

                    mpq_canonicalize(result->rational);

                    mpz_clear(numer);
                    mpz_clear(denom);

                } else {
                    mpq_t invertedNum;
                    mpq_init(invertedNum);

                    if (numSgn(base) == 0) {
                        fprintf(stderr,
                                "Error: Cannot raise zero to a -ve power\n");
                        exit(1);
                    }

                    mpq_inv(invertedNum, base->rational);

                    mpz_t numer, denom;
                    mpz_init(numer);
                    mpz_init(denom);

                    mpz_pow_ui(numer, mpq_numref(invertedNum), -expLong);
                    mpz_pow_ui(denom, mpq_denref(invertedNum), -expLong);

                    mpq_set_num(result->rational, numer);
                    mpq_set_den(result->rational, denom);

                    mpq_canonicalize(result->rational);

                    mpz_clear(numer);
                    mpz_clear(denom);

                    mpq_clear(invertedNum);
                }

                return result;
            }
        }
    }

    // base is complex, result will be complex
    if (base->kind == NUM_COMPLEX) {
        Number *complexExp = numConvertandSet(exp, NUM_COMPLEX);
        Number *result = numNew(NUM_COMPLEX);

        mpc_pow(result->complex, base->complex, complexExp->complex, MPC_RNDNN);

        numFree(complexExp);

        return result;
    }

    // base is less than zero, result can be complex
    if (numSgn(base) < 0) {
        Number *complexBase = numConvertandSet(base, NUM_COMPLEX);
        Number *complexExp = numConvertandSet(exp, NUM_COMPLEX);

        Number *result = numNew(NUM_COMPLEX);

        mpc_pow(result->complex, complexBase->complex, complexExp->complex,
                MPC_RNDNN);

        numFree(complexBase);
        numFree(complexExp);

        return result;
    } else {
        Number *realBase = numConvertandSet(base, NUM_REAL);
        Number *realExp = numConvertandSet(exp, NUM_REAL);

        Number *result = numNew(NUM_REAL);

        mpfr_pow(result->real, realBase->real, realExp->real, MPFR_RNDN);

        numFree(realBase);
        numFree(realExp);
        return result;
    }
}

// checks if number is an integer
// and if the complex part is zero
bool numIsInteger(const Number *num) {
    switch (num->kind) {
        case NUM_REAL: {
            return mpfr_integer_p(num->real);
        }

        case NUM_COMPLEX: {
            return (mpfr_integer_p(mpc_realref(num->complex))) &&
                   (mpfr_zero_p(mpc_imagref(num->complex)));
        }

        case NUM_RATIONAL: {
            mpz_t tempDeno;
            mpz_init(tempDeno);

            mpq_get_den(tempDeno, num->rational);

            int result = (mpz_cmp_ui(tempDeno, 1));

            mpz_clear(tempDeno);

            return result;
        }
    }
}

bool numCanBeLong(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            if (mpfr_cmp_si(mpc_realref(num->complex), LONG_MAX) > 0)
                return false;
            else
                return true;
        }

        case NUM_REAL: {
            if (mpfr_cmp_si(num->real, LONG_MAX) > 0)
                return false;
            else
                return true;
        }

        case NUM_RATIONAL: {
            if (mpq_cmp_si(num->rational, LONG_MAX, 1) > 0)
                return false;
            else
                return true;
        }
    }
}

// needs a complex number as input
bool numCompCanBeReal(const Number *num) {
    if (num->kind != NUM_COMPLEX) {
        fprintf(stderr,
                "Error: numCompCanBeReal received a non complex number\n");
        exit(1);
    }

    if (mpfr_zero_p(mpc_imagref(num->complex)))
        return true;
    else
        return false;
}

// TODO: check if this is necessary
// and return converted Number
// needs a rational number as input
bool numRatCanBeReal(const Number *num, Number *result) {
    if (num->kind != NUM_RATIONAL) {
        fprintf(stderr,
                "Error: numRatCanBeReal received a non rational number\n");
        exit(1);
    }

    mpq_t tempRat;
    mpq_init(tempRat);
    mpq_set(tempRat, num->rational);

    mpq_canonicalize(tempRat);
    bool canBeReal;

    if (mpz_cmp_ui(mpq_denref(tempRat), 0)) {
        canBeReal = true;
        result->kind = NUM_REAL;
        mpfr_set_z(result->real, mpq_numref(tempRat), MPFR_RNDN);
    } else {
        canBeReal = false;
        result = NULL;
    }

    mpq_clear(tempRat);

    return canBeReal;
}

// requires numIsInteger and numCanBeLong to be true
long numToLong(const Number *num) {
    long numLong;

    switch (num->kind) {
        case NUM_COMPLEX: {
            numLong = mpfr_get_si(mpc_realref(num->complex), MPFR_RNDN);

            break;
        }

        case NUM_REAL: {
            numLong = mpfr_get_si(num->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpz_t tempMpz;
            mpz_init(tempMpz);

            mpfr_t tempMpfr;
            mpfr_init2(tempMpfr, PRECISION);

            mpq_get_num(tempMpz, num->rational);
            mpfr_set_z(tempMpfr, tempMpz, MPFR_RNDN);

            numLong = mpfr_get_si(tempMpfr, MPFR_RNDN);

            mpfr_clear(tempMpfr);
            mpz_clear(tempMpz);

            break;
        }
    }

    return numLong;
}

void numFree(Number *num) {
    switch (num->kind) {
        case NUM_REAL: {
            mpfr_clear(num->real);
            break;
        }

        case NUM_COMPLEX: {
            mpc_clear(num->complex);
            break;
        }

        case NUM_RATIONAL: {
            mpq_clear(num->rational);
            break;
        }

        case NUM_ERROR: {
            freeStringView(&num->error);
            break;
        }
    }
}
