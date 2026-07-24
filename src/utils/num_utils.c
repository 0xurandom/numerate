#include "num_utils.h"

#include <gmp-x86_64.h>
#include <limits.h>
#include <mpc.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: allow changing precision
#define PRECISION 10

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

Number *numPow(const Number *base, const Number *exp) {
    // exp is an integer, keep base's kind the same
    if (numIsInteger(exp)) {
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

// requires numIsInteger to be true
// and imaginary part of complex
// num to be zero
long numToLong(const Number *num) {
    long numLong;

    switch (num->kind) {
        case NUM_COMPLEX: {
            // TODO: check for limits

            if (mpfr_cmp_si(mpc_realref(num->complex), LONG_MAX) > 0) {
                fprintf(
                    stderr,
                    "Error: Number is greater than LONG_MAX, cannot convert "
                    "to long\n");
                exit(1);
            }

            numLong = mpfr_get_si(mpc_realref(num->complex), MPFR_RNDN);

            break;
        }

        case NUM_REAL: {
            if (mpfr_cmp_si(num->real, LONG_MAX) > 0) {
                fprintf(
                    stderr,
                    "Error: Number is greater than LONG_MAX, cannot convert "
                    "to long\n");
                exit(1);
            }

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

            if (mpfr_cmp_si(num->real, LONG_MAX) > 0) {
                fprintf(
                    stderr,
                    "Error: Number is greater than LONG_MAX, cannot convert "
                    "to long\n");
                exit(1);
            }

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
    }
}
