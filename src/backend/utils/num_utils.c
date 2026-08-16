#include "num_utils.h"

#include <gmp-x86_64.h>
#include <limits.h>
#include <mpc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "num_ops.h"
#include "string_view_utils.h"

// TODO: check for functions using MPFR_RNDN instead of MPC_RNDNN

// TODO: before calculaing an equation, check if NUM_RATIONAL can
// be converted into a NUM_REAL

Number *numNew(NumberKind kind) {
    Number *num = malloc(sizeof(Number));
    numInit(num, kind);

    return num;
}

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

        case NUM_BOOL: {
            num->boolean = 0;
            break;
        }

        case NUM_ERROR: {
            initStringView(&num->error);
            break;
        }
    }
}

// expects dest to be from numNew and for dest
// to have the same NumberKind as src
void numSet(Number *dest, const Number *src) {
    if (dest->kind != src->kind) {
        // TODO: set dest to error
        fprintf(stderr,
                "Error: numSet received dest(kind: %d) and src(kind: %d)) of "
                "different kinds\n",
                dest->kind, src->kind);
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

        case NUM_ERROR: {
            copyStringView(&dest->error, &src->error);
            break;
        }

        case NUM_BOOL: {
            if (numSgnSi(src) == 0)
                dest->boolean = 0;
            else
                dest->boolean = 1;
        }
    }

    return;
}

void numSetBool(Number *num, bool boolean) {
    if (num->kind != NUM_BOOL) {
        fprintf(stderr, "numSetBool received a Number not of kind NUM_ERROR\n");
        exit(1);
    }

    num->boolean = boolean;

    return;
}

// needs a NUM_REAL as input
void numSetRealSd(Number *num, double val) {
    if (num->kind != NUM_REAL) {
        fprintf(stderr,
                "Error: numSetRealSi received a number not of kind NUM_REAL\n");
        exit(1);
    }
    mpfr_set_d(num->real, val, MPFR_RNDN);

    return;
}

// needs a Number of kind NUM_ERROR
void numSetError(Number *num, const char *errorString, size_t errorLength) {
    if (num->kind != NUM_ERROR) {
        fprintf(stderr,
                "numSetError received a Number not of kind NUM_ERROR\n");
        exit(1);
    }

    setStringView(&num->error, errorString, errorLength);
}

// inits x and sets x = src with NumberKind kind
Number *numConvertandSet(const Number *src, NumberKind kind) {
    Number *num = numNew(kind);

    if (src->kind == kind) {
        numSet(num, src);

    } else {
        Number *temp = numNew(src->kind);
        numSet(temp, src);

        temp = numConvert(temp, kind);

        numSet(num, temp);

        numFree(temp);
    }

    return num;
}

// frees input Number and returns a new Number
Number *numConvert(Number *num, NumberKind kind) {
    // TODO: result can be replaced here?
    if (num->kind == kind || num->kind == NUM_ERROR || kind == NUM_ERROR) {
        return num;
    }

    switch (kind) {
        case NUM_REAL: {
            mpfr_t result;
            mpfr_init2(result, PRECISION);

            switch (num->kind) {
                case NUM_REAL:
                    return num;

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

                case NUM_BOOL: {
                    if (numSgnSi(num) == 0)
                        mpfr_set_si(result, 0, MPFR_RNDN);
                    else
                        mpfr_set_si(result, 1, MPFR_RNDN);

                    break;
                }

                case NUM_ERROR: {
                    break;
                }
            }

            numFree(num);

            Number *resultNum = numNew(NUM_REAL);
            mpfr_set(resultNum->real, result, MPFR_RNDN);
            mpfr_clear(result);

            return resultNum;
        }

        case NUM_COMPLEX: {
            mpc_t result;
            mpc_init2(result, PRECISION);
            mpfr_set_ui(mpc_imagref(result), 0, MPFR_RNDN);

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

                case NUM_BOOL: {
                    if (numSgnSi(num) == 0)
                        mpfr_set_si(mpc_realref(result), 0, MPFR_RNDN);
                    else
                        mpfr_set_si(mpc_realref(result), 1, MPFR_RNDN);

                    break;
                }

                case NUM_ERROR: {
                    break;
                }
            }
            numFree(num);

            Number *resultNum = numNew(NUM_COMPLEX);
            mpc_set(resultNum->complex, result, MPFR_RNDN);
            mpc_clear(result);

            return resultNum;
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

                case NUM_BOOL: {
                    if (numSgnSi(num) == 0)
                        mpq_set_si(result, 0, 1);
                    else
                        mpq_set_si(result, 1, 1);

                    break;
                }

                case NUM_ERROR: {
                    break;
                }
            }
            numFree(num);

            Number *resultNum = numNew(NUM_RATIONAL);

            mpq_set(resultNum->rational, result);
            mpq_clear(result);

            return resultNum;
        }

        case NUM_BOOL: {
            int boolean = numSgnSi(num);
            Number *resultNum = numNew(NUM_BOOL);

            numSetBool(resultNum, boolean);
            return resultNum;
        }

        case NUM_ERROR: {
            return num;
        }

        default: {
            exit(1);
        }
    }
}

// rational > real > complex
NumberKind numPromoteKind(NumberKind kind1, NumberKind kind2) {
    if (kind1 == NUM_COMPLEX || kind2 == NUM_COMPLEX)
        return NUM_COMPLEX;
    else if (kind1 == NUM_REAL || kind2 == NUM_REAL || kind1 == NUM_BOOL ||
             kind2 == NUM_BOOL)
        return NUM_REAL;
    else
        return NUM_RATIONAL;
}

// checks if number is an integer
// and if the complex part is zero
bool numIsInteger(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            return (mpfr_integer_p(mpc_realref(num->complex))) &&
                   (mpfr_zero_p(mpc_imagref(num->complex)));
        }

        case NUM_REAL: {
            return mpfr_integer_p(num->real);
        }

        case NUM_RATIONAL: {
            mpz_t tempDeno;
            mpz_init(tempDeno);

            mpq_get_den(tempDeno, num->rational);

            int result = (mpz_cmp_ui(tempDeno, 1) == 0);

            mpz_clear(tempDeno);

            return result;
        }

        case NUM_BOOL: {
            return true;
        }

        case NUM_ERROR: {
            return false;
        }

        default: {
            exit(1);
        }
    }
}

// requires numIsInteger to be true
void numToInt(mpz_t result, const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpfr_get_z(result, mpc_realref(num->complex), MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            mpfr_get_z(result, num->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpz_set(result, mpq_numref(num->rational));
            break;
        }

        case NUM_BOOL: {
            mpz_set_ui(result, (num->boolean == 0) ? 0 : 1);
            break;
        }

        case NUM_ERROR: {
            // NUM_ERROR will not reach this case
        }
    }
}

bool numCanBeLong(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            if (mpfr_cmp_ui(mpc_imagref(num->complex), 0) != 0) return false;

            if (mpfr_cmp_si(mpc_realref(num->complex), LONG_MAX) > 0 ||
                mpfr_cmp_si(mpc_realref(num->complex), LONG_MIN) < 0)
                return false;
            else
                return true;
        }

        case NUM_REAL: {
            if (mpfr_cmp_si(num->real, LONG_MAX) > 0 ||
                mpfr_cmp_si(num->real, LONG_MIN) < 0)
                return false;
            else
                return true;
        }

        case NUM_RATIONAL: {
            if (mpz_cmp_ui(mpq_denref(num->rational), 1) != 0) return false;

            if (mpq_cmp_si(num->rational, LONG_MAX, 1) > 0 ||
                mpq_cmp_si(num->rational, LONG_MIN, 1) < 0)
                return false;
            else
                return true;
        }

        case NUM_BOOL: {
            return true;
        }

        case NUM_ERROR: {
            return false;
        }

        default: {
            exit(1);
        }
    }
}

// if result == 0, true;
// 1, can be unsigned long but is negative;
// -1, cannot be unsigned long
int numCanBeUnsignedLong(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            if (mpfr_cmp_ui(mpc_imagref(num->complex), 0) != 0) return -1;

            if (mpfr_cmp_ui(mpc_realref(num->complex), ULONG_MAX) > 0)
                return -1;

            if (mpfr_cmp_ui(mpc_realref(num->complex), 0) < 0) return 1;

            return 0;
        }

        case NUM_REAL: {
            if (mpfr_cmp_ui(num->real, ULONG_MAX) > 0) return -1;

            if (mpfr_cmp_ui(num->real, 0) < 0) return 1;

            return 0;
        }

        case NUM_RATIONAL: {
            if (mpz_cmp_ui(mpq_denref(num->rational), 1) != 0) return -1;

            if (mpq_cmp_ui(num->rational, ULONG_MAX, 1) > 0) return -1;

            if ((mpq_sgn(num->rational) < 0)) return 1;

            return 0;
        }

        case NUM_BOOL: {
            return 0;
        }

        case NUM_ERROR: {
            return -1;
        }

        default: {
            exit(1);
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

// needs a rational number as input
// and result to be a NUM_REAL from newNew
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
    bool canBeReal = (mpz_cmp_ui(mpq_denref(tempRat), 1) == 0);

    if (canBeReal) {
        if (result->kind != NUM_REAL) {
            fprintf(stderr,
                    "Error: numRatCanBeReal received a non real number\n");
            exit(1);
        }

        mpfr_set_z(result->real, mpq_numref(tempRat), MPFR_RNDN);
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

        case NUM_BOOL: {
            if (num->boolean == 0)
                numLong = 0;
            else
                numLong = 1;

            break;
        }

        case NUM_ERROR: {
            numLong = 0;

            break;
        }
    }

    return numLong;
}

// requires numIsInteger and numCanBeLong to be true
unsigned long numToUnsignedLong(const Number *num) {
    unsigned long numUnsignedLong;
    switch (num->kind) {
        case NUM_COMPLEX: {
            numUnsignedLong = mpfr_get_ui(mpc_realref(num->complex), MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            numUnsignedLong = mpfr_get_ui(num->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            numUnsignedLong = mpz_get_ui(mpq_numref(num->rational));
            break;
        }

        case NUM_BOOL: {
            numUnsignedLong = num->boolean == 0 ? 0 : 1;
            break;
        }

        case NUM_ERROR: {
            numUnsignedLong = 0;
            break;
        }
    }

    return numUnsignedLong;
}

void numPrint(const Number *num) {
    if (num == NULL) {
        printf("numPrint received a null pointer\n");
        return;
    }

    switch (num->kind) {
        case NUM_COMPLEX: {
            mpfr_printf("complex: %Rg + %Rgi\n", mpc_realref(num->complex),
                        mpc_imagref(num->complex));
            return;
        }

        case NUM_REAL: {
            mpfr_printf("real: %Rg\n", num->real);
            return;
        }

        case NUM_RATIONAL: {
            gmp_printf("rational: %Zd/%Zd\n", mpq_numref(num->rational),
                       mpq_denref(num->rational));
            return;
        }

        case NUM_BOOL: {
            char trueString[] = "true";
            char falseString[] = "false";
            printf("%s\n", num->boolean ? trueString : falseString);
            return;
        }

        case NUM_ERROR: {
            printStringView(&num->error);
            printf("\n");
            return;
        }
    }
}

// clears the kind inside the number
// but not the number itself
void numClear(Number *num) {
    if (num == NULL) return;

    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_clear(num->complex);
            num->kind = -1;
            break;
        }
        case NUM_REAL: {
            mpfr_clear(num->real);
            num->kind = -1;

            break;
        }

        case NUM_RATIONAL: {
            mpq_clear(num->rational);
            num->kind = -1;

            break;
        }

        case NUM_BOOL: {
            break;
        }

        case NUM_ERROR: {
            freeStringView(&num->error);
            num->kind = -1;

            break;
        }

        default:
            break;
    }
}

void numFree(Number *num) {
    if (num == NULL) return;

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

        case NUM_BOOL: {
            break;
        }

        case NUM_ERROR: {
            freeStringView(&num->error);
            break;
        }
    }

    free(num);
}

void numFrees(Number *num, ...) {
    if (num == NULL) return;

    va_list args;
    va_start(args, num);

    Number *nextArg;

    while ((nextArg = va_arg(args, Number *)) != NULL) {
        numFree(nextArg);
    }

    va_end(args);
}
