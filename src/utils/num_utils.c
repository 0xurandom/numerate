#include "num_utils.h"

#include <gmp-x86_64.h>
#include <limits.h>
#include <mpc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
