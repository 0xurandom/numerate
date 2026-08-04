#include "num_ops.h"

#include <gmp-x86_64.h>
#include <iso646.h>
#include <math.h>
#include <mpc.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "num_utils.h"

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

        // numPromoteKind does not return these values
        case NUM_BOOL: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_BOOL\n");
            break;
        }

        case NUM_ERROR: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_ERROR\n");
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

        // numPromoteKind does not return these values
        case NUM_BOOL: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_BOOL\n");
            break;
        }

        case NUM_ERROR: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_ERROR\n");
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

        // numPromoteKind does not return these values
        case NUM_BOOL: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_BOOL\n");
            break;
        }

        case NUM_ERROR: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_ERROR\n");
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

        // numPromoteKind does not return these values
        case NUM_BOOL: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_BOOL\n");
            break;
        }

        case NUM_ERROR: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_ERROR\n");
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

        // numPromoteKind does not return these values
        case NUM_BOOL: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_BOOL\n");
            break;
        }

        case NUM_ERROR: {
            fprintf(stderr, "Warning: numPromoteKind returned NUM_ERROR\n");
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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numNeg(realNum);
            numFree(realNum);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *result = numNew(NUM_REAL);
            mpfr_set_si(result->real, (num->boolean == 0) ? 0 : 1, MPFR_RNDN);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *result = numNew(NUM_REAL);
            mpfr_set_si(result->real, (num->boolean == 0) ? 0 : 1, MPFR_RNDN);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
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
                Number *result = numNew(NUM_ERROR);

                char errorString[] = "Factorial is too large to compute";
                size_t errorLength = strlen(errorString);

                numSetError(result, errorString, errorLength);

                return result;
            }

            Number *result = numNew(NUM_REAL);
            mpfr_t numPlusOne;
            mpfr_init2(numPlusOne, MPFR_RNDN);
            mpfr_add_ui(numPlusOne, num->real, 1, MPFR_RNDN);

            if (mpfr_integer_p(numPlusOne) && (mpfr_sgn(numPlusOne) <= 0)) {
                Number *result = numNew(NUM_ERROR);

                char errorString[] = "Factorial is undefined for this value";
                size_t errorLength = strlen(errorString);

                numSetError(result, errorString, errorLength);

                return result;
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
                    Number *result = numNew(NUM_ERROR);

                    char errorString[] = "Factorial is too large to compute";
                    size_t errorLength = strlen(errorString);

                    numSetError(result, errorString, errorLength);

                    return result;
                }

                unsigned long numerLong = mpz_get_ui(tempNumer);
                mpz_clear(tempNumer);
            }

            Number *result = numNew(NUM_REAL);

            return result;
        }

        case NUM_BOOL: {
            Number *result = numNew(NUM_ERROR);
            mpfr_set_si(result->real, 1, MPFR_RNDN);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
        }
    }
}

Number *numSubfact(const Number *num) {
    if (num->kind == NUM_ERROR) {
        Number *result = numNew(NUM_ERROR);
        numSet(result, num);

        return result;
    }

    int sgn = numSgnSi(num);

    if (sgn < 0) {
        Number *result = numNew(NUM_ERROR);

        char error[] = "Subfactorial is undefined for negative numbers";
        numSetError(result, error, strlen(error));

        return result;
    }

    mpc_t z;
    mpc_init2(z, PRECISION);

    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_set(z, num->complex, MPC_RNDNN);
            break;
        }

        case NUM_REAL: {
            mpc_set_fr(z, num->real, MPC_RNDNN);
            break;
        }

        case NUM_RATIONAL: {
            mpfr_t tempReal;
            mpfr_init2(tempReal, PRECISION);

            mpfr_set_q(tempReal, num->rational, MPFR_RNDN);
            mpc_set_fr(z, tempReal, MPC_RNDNN);

            mpfr_clear(tempReal);
            break;
        }

        case NUM_BOOL: {
            // NUM_BOOL does not reach this case
            break;
        }

        case NUM_ERROR: {
            // NUM_ERROR does not reach this case
            break;
        }
    }

    Number *zPlusOne = numNew(NUM_COMPLEX);
    mpc_add_ui(zPlusOne->complex, z, 1, MPC_RNDNN);

    Number *gammaZplusOne = complexGamma(zPlusOne);

    mpfr_t eConst;
    mpfr_init2(eConst, PRECISION);
    mpfr_set_ui(eConst, 1, MPFR_RNDN);
    mpfr_exp(eConst, eConst, MPFR_RNDN);

    mpc_div_fr(gammaZplusOne->complex, gammaZplusOne->complex, eConst,
               MPC_RNDNN);

    mpfr_round(mpc_realref(gammaZplusOne->complex),
               mpc_realref(gammaZplusOne->complex));
    mpfr_round(mpc_imagref(gammaZplusOne->complex),
               mpc_imagref(gammaZplusOne->complex));

    Number *result = numNew(num->kind);

    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_set(result->complex, gammaZplusOne->complex, MPC_RNDNN);
            break;
        }

        case NUM_REAL: {
            mpfr_set(result->real, mpc_realref(gammaZplusOne->complex),
                     MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpz_t tempInt;
            mpz_init(tempInt);
            mpfr_get_z(tempInt, mpc_realref(gammaZplusOne->complex), MPFR_RNDN);
            mpq_set_z(result->rational, tempInt);
            mpz_clear(tempInt);

            break;
        }

        case NUM_BOOL: {
            result->boolean =
                (mpfr_cmp_ui(mpc_realref(gammaZplusOne->complex), 0) != 0);

            break;
        }

        case NUM_ERROR: {
            // NUM_ERROR will not reach this case
        }
    }

    mpc_clear(z);
    mpfr_clear(eConst);
    numFree(zPlusOne);
    numFree(gammaZplusOne);

    return result;
}

Number *numGamma(const Number *num) {
    if (numSgnSi(num) == 0) {
        Number *result = numNew(NUM_ERROR);

        const char error[] = "Gamma is undefined for zero";
        numSetError(result, error, strlen(error));

        return result;
    }

    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = complexGamma(num);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_gamma(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numGamma(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_BOOL: {
            // bool == 0 will not reach this case
            Number *result = numNew(NUM_REAL);
            numSetRealSi(result, 1);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
        }
    }
}

static Number *complexGamma(const Number *num) {
    if (num->kind != NUM_COMPLEX) {
        fprintf(stderr, "Error: complexGamma received a non NUM_COMPLEX\n");
        exit(1);
    }

    if (mpfr_cmp_d(mpc_realref(num->complex), 0.5) < 0)
        return eulerReflection(num);
    else
        return SpougeApprox(num);
}

// needs a complex num and for the real
// part of the number to be greater than 0.5
static Number *SpougeApprox(const Number *num) {
    // TODO: instead of using sum as the base result, use result->complex

    mpc_t double_pi_sqrt;
    mpc_init2(double_pi_sqrt, PRECISION);
    mpfr_const_pi(mpc_realref(double_pi_sqrt), MPFR_RNDN);
    mpfr_set_si(mpc_imagref(double_pi_sqrt), 0, MPFR_RNDN);
    mpc_mul_si(double_pi_sqrt, double_pi_sqrt, 2, MPC_RNDNN);
    mpc_sqrt(double_pi_sqrt, double_pi_sqrt, MPC_RNDNN);

    mpfr_prec_t prec = mpc_get_prec(num->complex);

    int a = (int)ceil(0.3772 * (double)prec) + 3;

    mpfr_t c_k[a];
    int sign = 1;
    mpfr_t k_fact;
    mpfr_init2(k_fact, PRECISION);
    mpfr_set_si(k_fact, 1, MPFR_RNDN);

    mpfr_t tempAminusK, tempExpAminusK, tempKminushalf, tempPowAminusK;
    mpfr_inits2(PRECISION, tempAminusK, tempExpAminusK, tempKminushalf,
                tempPowAminusK, (mpfr_ptr)NULL);

    for (size_t k = 1; k < a; k++) {
        mpfr_init2(c_k[k], PRECISION);
        mpfr_set_si(c_k[k], sign, MPFR_RNDN);
        mpfr_set_si(tempAminusK, a - k, MPFR_RNDN);
        mpfr_exp(tempExpAminusK, tempAminusK, MPFR_RNDN);
        mpfr_set_d(tempKminushalf, k - 0.5, MPFR_RNDN);
        mpfr_pow(tempPowAminusK, tempAminusK, tempKminushalf, MPFR_RNDN);

        mpfr_mul(c_k[k], c_k[k], tempPowAminusK, MPFR_RNDN);
        mpfr_mul(c_k[k], c_k[k], tempExpAminusK, MPFR_RNDN);

        mpfr_div(c_k[k], c_k[k], k_fact, MPFR_RNDN);

        mpfr_mul_ui(k_fact, k_fact, k, MPFR_RNDN);

        sign = -sign;
    }

    mpfr_clears(tempAminusK, tempExpAminusK, tempKminushalf, tempPowAminusK,
                (mpfr_ptr)NULL);

    mpc_t sum;
    mpc_init2(sum, PRECISION);
    mpc_set_ui_ui(sum, 0, 0, MPC_RNDNN);

    mpc_t tempC_k, tempZplusK;
    mpc_init2(tempC_k, PRECISION);
    mpc_init2(tempZplusK, PRECISION);

    for (size_t k = 1; k < a; k++) {
        mpc_set_si_si(tempC_k, 0, 0, MPC_RNDNN);
        mpfr_set(mpc_realref(tempC_k), c_k[k], MPFR_RNDN);

        mpc_set_ui_ui(tempZplusK, k, 0, MPC_RNDNN);
        mpc_add(tempZplusK, tempZplusK, num->complex, MPC_RNDNN);

        mpc_div(tempC_k, tempC_k, tempZplusK, MPC_RNDNN);

        mpc_add(sum, sum, tempC_k, MPC_RNDNN);

        mpfr_clear(c_k[k]);
    }

    mpc_clear(tempZplusK);
    mpc_clear(tempC_k);

    mpc_add(sum, sum, double_pi_sqrt, MPC_RNDNN);
    mpc_clear(double_pi_sqrt);

    mpc_t tempZPlusA;
    mpc_init2(tempZPlusA, PRECISION);

    mpc_t tempBase;
    mpc_init2(tempBase, PRECISION);
    mpc_set(tempBase, num->complex, MPC_RNDNN);
    mpc_add_si(tempBase, tempBase, a, MPC_RNDNN);

    mpc_t tempExp;
    mpc_init2(tempExp, PRECISION);
    mpc_set_d(tempExp, 0.5, MPC_RNDNN);
    mpc_add(tempExp, tempExp, num->complex, MPC_RNDNN);

    mpc_pow(tempZPlusA, tempBase, tempExp, MPC_RNDNN);

    mpc_clear(tempExp);

    mpc_t tempEtoZplusA;
    mpc_init2(tempEtoZplusA, PRECISION);

    mpc_neg(tempBase, tempBase, MPC_RNDNN);
    mpc_exp(tempEtoZplusA, tempBase, MPC_RNDNN);

    mpc_mul(sum, sum, tempZPlusA, MPC_RNDNN);
    mpc_mul(sum, sum, tempEtoZplusA, MPC_RNDNN);
    mpc_div(sum, sum, num->complex, MPC_RNDNN);

    mpc_clear(tempBase);
    mpc_clear(tempEtoZplusA);
    mpc_clear(tempZPlusA);
    mpfr_clear(k_fact);

    Number *result = numNew(NUM_COMPLEX);

    mpc_set(result->complex, sum, MPC_RNDNN);

    mpc_clear(sum);

    return result;
}

static Number *eulerReflection(const Number *num) {
    Number *oneMinusZ = numNew(NUM_COMPLEX);
    mpc_ui_sub(oneMinusZ->complex, 1, num->complex, MPC_RNDNN);

    Number *gammaOneMinusZ = SpougeApprox(oneMinusZ);

    mpfr_t pi;
    mpfr_init2(pi, PRECISION);
    mpfr_const_pi(pi, MPFR_RNDN);

    mpc_t piTimesZ;
    mpc_init2(piTimesZ, PRECISION);
    mpc_mul_fr(piTimesZ, num->complex, pi, MPC_RNDNN);

    mpc_t sinPiZ;
    mpc_init2(sinPiZ, PRECISION);
    mpc_sin(sinPiZ, piTimesZ, MPC_RNDNN);

    mpc_t denom;
    mpc_init2(denom, PRECISION);
    mpc_mul(denom, sinPiZ, gammaOneMinusZ->complex, MPC_RNDNN);

    Number *result = numNew(NUM_COMPLEX);
    mpc_set_fr(result->complex, pi, MPC_RNDNN);
    mpc_div(result->complex, result->complex, denom, MPC_RNDNN);

    mpc_clear(denom);
    mpc_clear(sinPiZ);
    mpc_clear(piTimesZ);
    mpfr_clear(pi);
    numFree(gammaOneMinusZ);
    numFree(oneMinusZ);

    return result;
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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numSin(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numSinh(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCos(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCosh(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numTan(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numTanh(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCosec(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCosech(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numSec(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numSech(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCot(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

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

        case NUM_BOOL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numCoth(realNum);

            numFree(realNum);
            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
        }
    }
}

int numSgnSi(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            if (mpc_cmp_si_si(num->complex, 0, 0)) {
                return 0;
            } else {
                // complex value for non zero nums
                fprintf(stderr, "numSgnSi received a nonzero complex number\n");
                exit(1);
            }
        }

        case NUM_REAL: {
            return mpfr_sgn(num->real);
        }

        case NUM_RATIONAL: {
            return mpq_sgn(num->rational);
        }

        case NUM_BOOL: {
            return (num->boolean == 1) ? 1 : 0;
        }

        case NUM_ERROR: {
            fprintf(stderr, "Warning numSgnSi returning 0 for NUM_ERROR\n");
            return 0;
        }
    }
}

Number *numSgn(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            if (mpc_cmp_si_si(num->complex, 0, 0) == 0) {
                Number *result = numNew(NUM_REAL);
                mpfr_set_si(result->real, 0, MPFR_RNDN);

                return result;
            }

            Number *result = numNew(NUM_COMPLEX);

            mpfr_t tempMagnitude;
            mpfr_init2(tempMagnitude, PRECISION);

            mpc_abs(tempMagnitude, num->complex, MPFR_RNDN);
            mpc_div_fr(result->complex, num->complex, tempMagnitude, MPC_RNDNN);

            mpfr_clear(tempMagnitude);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            int sgnInt = mpfr_sgn(num->real);
            mpfr_set_ui(result->real, sgnInt, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *result = numNew(NUM_REAL);
            int sgnInt = mpq_sgn(num->rational);
            mpfr_set_ui(result->real, sgnInt, MPFR_RNDN);

            return result;
        }

        case NUM_BOOL: {
            Number *result = numNew(NUM_REAL);
            mpfr_set_ui(result->real, (num->boolean == 0) ? 0 : 1, MPFR_RNDN);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
        }
    }
}

// checks if real part of num is 1
// returns false if num has complex part
bool numIsUnity(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            if (!mpfr_zero_p(mpc_imagref(num->complex))) return false;

            if (mpfr_cmp_si(mpc_realref(num->complex), 1))
                return true;
            else
                return false;
        }

        case NUM_REAL: {
            if (mpfr_cmp_si(num->real, 1))
                return true;
            else
                return false;
        }

        case NUM_RATIONAL: {
            if (mpz_cmp_si(mpq_numref(num->rational), 1) &&
                mpz_cmp_si(mpq_denref(num->rational), 1))
                return true;
            else
                return false;
        }

        case NUM_BOOL: {
            if (num->boolean == 1)
                return true;
            else
                return false;
        }

        case NUM_ERROR: {
            return false;
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

        case NUM_BOOL: {
            Number *result = numConvertandSet(num, NUM_REAL);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
        }
    }
}

Number *numPow(const Number *base, const Number *exp) {
    if (base->kind == NUM_ERROR || exp->kind == NUM_ERROR) {
        Number *result = numNew(NUM_ERROR);
        numSet(result, (base->kind == NUM_ERROR) ? base : exp);

        return result;
    }

    if (base->kind == NUM_BOOL || exp->kind == NUM_BOOL) {
        NumberKind promotedKind = numPromoteKind(base->kind, exp->kind);

        Number *promotedBase = numConvertandSet(base, promotedKind);
        Number *promotedExp = numConvertandSet(exp, promotedKind);

        Number *result = numPow(promotedBase, promotedExp);

        numFree(promotedBase);
        numFree(promotedExp);

        return result;
    }

    // exp is an integer, keep base's kind the same
    if (numIsInteger(exp)) {
        // TODO: add exception to fllowing error for zero, one and inf
        if (!numCanBeLong(exp)) {
            Number *result = numNew(NUM_ERROR);

            char errorString[] = "Exponent is too long to compute";
            size_t errorLength = strlen(errorString);

            numSetError(result, errorString, errorLength);

            return result;
        }

        long expLong = numToLong(exp);

        switch (base->kind) {
            case NUM_COMPLEX: {
                if (mpc_cmp_si(base->complex, 0) == 0 && expLong < 0) {
                    Number *result = numNew(NUM_ERROR);

                    char errorString[] = "Cannot raise zero to a -ve power";
                    size_t errorLength = strlen(errorString);

                    numSetError(result, errorString, errorLength);

                    return result;
                }

                Number *result = numNew(NUM_COMPLEX);
                mpc_pow_si(result->complex, base->complex, expLong, MPC_RNDNN);

                return result;
            }

            case NUM_REAL: {
                if (mpfr_zero_p(base->real) && expLong < 0) {
                    Number *result = numNew(NUM_ERROR);

                    char errorString[] = "Cannot raise zero to a -ve power";
                    size_t errorLength = strlen(errorString);

                    numSetError(result, errorString, errorLength);

                    return result;
                }

                Number *result = numNew(NUM_REAL);
                mpfr_pow_si(result->real, base->real, expLong, MPFR_RNDN);

                return result;
            }

            case NUM_RATIONAL: {
                if (mpq_sgn(base->rational) == 0 && expLong < 0) {
                    Number *result = numNew(NUM_ERROR);

                    char errorString[] = "Cannot raise zero to a -ve power";
                    size_t errorLength = strlen(errorString);

                    numSetError(result, errorString, errorLength);

                    return result;
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
                        Number *result = numNew(NUM_ERROR);

                        char errorString[] = "Cannot raise zero to a -ve power";
                        size_t errorLength = strlen(errorString);

                        numSetError(result, errorString, errorLength);

                        return result;
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

            case NUM_BOOL: {
                // a num of kind NUM_ERROR will
                //  not reach this case block
                return NULL;
            }

            case NUM_ERROR: {
                // a num of kind NUM_ERROR will
                //  not reach this case block
                return NULL;
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

Number *numExp(const Number *num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_exp(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_exp(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numExp(realNum);
            numFree(realNum);

            return result;
        }

        case NUM_BOOL: {
            Number *result = numNew(NUM_REAL);
            mpfr_set_ui(result->real, (num->boolean == 0) ? 1 : M_E, MPFR_RNDN);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
        }
    }
}

Number *numLn(const Number *num) {
    if (numSgnSi(num) == 0) {
        Number *num = numNew(NUM_ERROR);

        char error[] = "Log is undefined for 0";
        numSetError(num, error, strlen(error));

        return num;
    }

    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_log(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_log(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numLn(realNum);
            numFree(realNum);

            return result;
        }

        case NUM_BOOL: {
            // a bool == 0 will not reach this case
            Number *result = numNew(NUM_REAL);
            mpfr_set_si(result->real, 0, MPFR_RNDN);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
        }
    }
}

Number *numLog(const Number *num) {
    if (numSgnSi(num) == 0) {
        Number *num = numNew(NUM_ERROR);

        char error[] = "Log is undefined for 0";
        numSetError(num, error, strlen(error));

        return num;
    }

    switch (num->kind) {
        case NUM_COMPLEX: {
            Number *result = numNew(NUM_COMPLEX);
            mpc_log10(result->complex, num->complex, MPC_RNDNN);

            return result;
        }

        case NUM_REAL: {
            Number *result = numNew(NUM_REAL);
            mpfr_log10(result->real, num->real, MPFR_RNDN);

            return result;
        }

        case NUM_RATIONAL: {
            Number *realNum = numConvertandSet(num, NUM_REAL);
            Number *result = numLog(realNum);
            numFree(realNum);

            return result;
        }

        case NUM_BOOL: {
            // a bool == 0 will not reach this case
            Number *result = numNew(NUM_REAL);
            mpfr_set_ui(result->real, 0, MPFR_RNDN);

            return result;
        }

        case NUM_ERROR: {
            Number *result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
        }
    }
}

Number *numNpr(const Number *n, const Number *r) {
    // TODO: fact is undefined for complex nums
    if (n->kind == NUM_COMPLEX || r->kind == NUM_COMPLEX) {
    }

    Number *realN = numConvertandSet(n, NUM_REAL);
    Number *realR = numConvertandSet(r, NUM_REAL);
    Number *nMinusR = numSubtract(realN, realR);

    numFree(realR);

    Number *nFact = numFact(realN);
    Number *nMinusrFact = numFact(nMinusR);

    numFree(realN);
    numFree(nMinusR);

    Number *result = numDivide(nFact, nMinusrFact);

    numFree(nFact);
    numFree(nMinusrFact);

    return result;
}

Number *numNcr(const Number *n, const Number *r) {
    // TODO: fact is undefined for complex nums
    if (n->kind == NUM_COMPLEX || r->kind == NUM_COMPLEX) {
    }

    Number *realN = numConvertandSet(n, NUM_REAL);
    Number *realR = numConvertandSet(r, NUM_REAL);
    Number *nMinusR = numSubtract(realN, realR);

    Number *nFact = numFact(realN);
    Number *rFact = numFact(realR);
    Number *nMinusrFact = numFact(nMinusR);

    numFree(realN);
    numFree(realR);
    numFree(nMinusR);

    Number *denom = numMultiply(rFact, nMinusrFact);
    Number *result = numDivide(nFact, denom);

    numFree(nFact);
    numFree(rFact);
    numFree(nMinusrFact);
    numFree(denom);

    return result;
}
