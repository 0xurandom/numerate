#include "num_utils.h"

#include <gmp-x86_64.h>
#include <mpc.h>
#include <stdio.h>

// TODO: allow changing precision
#define PRECISION 10

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

// TODO
// inits x and x = y
void numSet(Number *x, const Number *y) { numInit(x, y->kind); }

void numConvert(Number *num, NumberKind kind) {
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

            numClear(num);

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
            numClear(num);

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
            numClear(num);

            num->kind = NUM_RATIONAL;
            mpq_init(num->rational);
            mpq_set(num->rational, result);

            mpq_clear(result);
            break;
        }
    }
}

void numClear(Number *num) {
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
