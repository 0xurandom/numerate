#include "num_utils.h"

#include <gmp-x86_64.h>
#include <limits.h>
#include <mpc.h>
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

        case NUM_ERROR: {
            initStringView(&num->error);
            break;
        }

        case NUM_BOOL: {
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
void numSetRealSi(Number *num, double val) {
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
                        mpz_set_si(mpc_realref(result), 0);
                    else
                        mpz_set_si(mpc_realref(result), 1);

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

            int result = (mpz_cmp_ui(tempDeno, 1));

            mpz_clear(tempDeno);

            return result;
        }

        case NUM_BOOL: {
            return true;
        }

        case NUM_ERROR: {
            return false;
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

        case NUM_BOOL: {
            return true;
        }

        case NUM_ERROR: {
            return false;
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

void numPrint(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpfr_printf("%R + %Ri", mpc_realref(num->complex),
                        mpc_imagref(num->complex));
            break;
        }

        case NUM_REAL: {
            mpfr_printf("%R", num->real);
            break;
        }

        case NUM_RATIONAL: {
            mpfr_printf("%Z/%Z", mpq_numref(num->rational),
                        mpq_denref(num->rational));
            break;
        }

        case NUM_BOOL: {
            char trueString[] = "true";
            char falseString[] = "false";
            printf("%s", num->boolean ? trueString : falseString);
            break;
        }

        case NUM_ERROR: {
            printStringView(&num->error);
            break;
        }
    }
}

// TODO: decide if num itself will
// need to be freed
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

        case NUM_BOOL: {
            break;
        }

        case NUM_ERROR: {
            freeStringView(&num->error);
            break;
        }
    }
}
