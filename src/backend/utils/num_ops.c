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
#include "string_view_utils.h"

// inits out to the required kind
// preferably use NUM_BOOL
void numAddInto(Number* out, const Number* a, const Number* b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number* tempA = numConvertandSet(a, promotedKind);
    Number* tempB = numConvertandSet(b, promotedKind);

    numClear(out);
    numInit(out, promotedKind);

    switch (promotedKind) {
        case NUM_COMPLEX: {
            mpc_add(out->complex, tempA->complex, tempB->complex, MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            mpfr_add(out->real, tempA->real, tempB->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpq_add(out->rational, tempA->rational, tempB->rational);
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

    return;
}

// inits a number and sets it to the sum
Number* numAdd(const Number* a, const Number* b) {
    Number* result = numNew(NUM_BOOL);
    numAddInto(result, a, b);
    return result;
}

void numSubtractInto(Number* out, const Number* a, const Number* b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number* tempA = numConvertandSet(a, promotedKind);
    Number* tempB = numConvertandSet(b, promotedKind);

    numClear(out);
    numInit(out, promotedKind);

    switch (promotedKind) {
        case NUM_COMPLEX: {
            mpc_sub(out->complex, tempA->complex, tempB->complex, MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            mpfr_sub(out->real, tempA->real, tempB->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpq_sub(out->rational, tempA->rational, tempB->rational);
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

    return;
}

Number* numSubtract(const Number* a, const Number* b) {
    Number* result = numNew(NUM_BOOL);
    numSubtractInto(result, a, b);
    return result;
}

void numMultiplyInto(Number* out, const Number* a, const Number* b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number* tempA = numConvertandSet(a, promotedKind);
    Number* tempB = numConvertandSet(b, promotedKind);

    numClear(out);
    numInit(out, promotedKind);

    switch (promotedKind) {
        case NUM_COMPLEX: {
            mpc_mul(out->complex, tempA->complex, tempB->complex, MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            mpfr_mul(out->real, tempA->real, tempB->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpq_mul(out->rational, tempA->rational, tempB->rational);
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

    return;
}

Number* numMultiply(const Number* a, const Number* b) {
    Number* result = numNew(NUM_BOOL);
    numMultiplyInto(result, a, b);
    return result;
}

void numDivideInto(Number* out, const Number* a, const Number* b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number* tempA = numConvertandSet(a, promotedKind);
    Number* tempB = numConvertandSet(b, promotedKind);

    numClear(out);
    numInit(out, promotedKind);

    switch (promotedKind) {
        case NUM_COMPLEX: {
            mpc_div(out->complex, tempA->complex, tempB->complex, MPFR_RNDN);
            break;
        }

        case NUM_REAL: {
            mpfr_div(out->real, tempA->real, tempB->real, MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpq_div(out->rational, tempA->rational, tempB->rational);
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

    return;
}

Number* numDivide(const Number* a, const Number* b) {
    Number* result = numNew(NUM_BOOL);
    numDivideInto(result, a, b);
    return result;
}

// compares numbers and the magnitude of complex nums
// returns 0 if equal, 1 if a > b, -1 if a < b
int numCompare(const Number* a, const Number* b) {
    NumberKind promotedKind = numPromoteKind(a->kind, b->kind);

    Number* tempA = numConvertandSet(a, promotedKind);
    Number* tempB = numConvertandSet(b, promotedKind);

    int result = 0;

    switch (promotedKind) {
        case NUM_COMPLEX: {
            Number* tempAmagnitude = numNew(NUM_REAL);
            Number* tempBmagnitude = numNew(NUM_REAL);

            mpc_abs(tempAmagnitude->real, tempA->complex, MPFR_RNDN);
            mpc_abs(tempBmagnitude->real, tempB->complex, MPFR_RNDN);

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

int numCompareSi(const Number* a, long b) {
    int result;

    switch (a->kind) {
        case NUM_COMPLEX: {
            Number* realA = numNew(NUM_REAL);
            mpc_abs(realA->real, a->complex, MPFR_RNDN);
            result = mpfr_cmp_si(realA->real, b);
            numFree(realA);

            return result;
        }

        case NUM_REAL: {
            result = mpfr_cmp_si(a->real, b);
            return result;
        }

        case NUM_RATIONAL: {
            Number* RealA = numConvertandSet(a, NUM_REAL);
            result = mpfr_cmp_si(RealA->real, b);
            numFree(RealA);

            return result;
        }

        case NUM_BOOL: {
            if ((int)a->boolean == b)
                result = 0;
            else if ((int)a->boolean > b)
                result = 1;
            else
                result = -1;

            return result;
        }

        case NUM_ERROR: {
            fprintf(stderr, "Error: numCompareSi received a NUM_ERROR\n");
            exit(1);
        }

        default: {
            exit(1);
        }
    }
}

void numNegInto(Number* out, const Number* num) {
    numClear(out);
    switch (num->kind) {
        case NUM_COMPLEX: {
            numInit(out, NUM_COMPLEX);
            mpc_neg(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numInit(out, NUM_REAL);
            mpfr_neg(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            numInit(out, NUM_RATIONAL);
            mpq_neg(out->rational, num->rational);

            return;
        }

        case NUM_BOOL: {
            numInit(out, NUM_BOOL);
            out->boolean = !(num->boolean);

            return;
        }

        case NUM_ERROR: {
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numNeg(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numNegInto(result, num);
    return result;
}

void numFloorInto(Number* out, const Number* num) {
    numClear(out);
    switch (num->kind) {
        case NUM_COMPLEX: {
            numInit(out, NUM_COMPLEX);

            mpfr_floor(mpc_realref(out->complex), mpc_realref(num->complex));
            mpfr_floor(mpc_imagref(out->complex), mpc_imagref(num->complex));

            return;
        }

        case NUM_REAL: {
            numInit(out, NUM_REAL);

            mpfr_floor(out->real, num->real);

            return;
        }

        case NUM_RATIONAL: {
            numInit(out, NUM_REAL);

            mpz_t tempInt;
            mpz_init(tempInt);

            mpz_fdiv_q(tempInt, mpq_numref(num->rational),
                       mpq_denref(num->rational));
            mpfr_set_z(out->real, tempInt, MPFR_RNDN);

            mpz_clear(tempInt);

            return;
        }

        case NUM_BOOL: {
            // TODO: change this to NUM_BOOL?
            numInit(out, NUM_REAL);
            mpfr_set_si(out->real, (num->boolean == 0) ? 0 : 1, MPFR_RNDN);

            return;
        }

        case NUM_ERROR: {
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numFloor(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numFloorInto(result, num);
    return result;
}

void numCeilInto(Number* out, const Number* num) {
    numClear(out);
    switch (num->kind) {
        case NUM_COMPLEX: {
            numInit(out, NUM_COMPLEX);

            mpfr_ceil(mpc_realref(out->complex), mpc_realref(num->complex));
            mpfr_ceil(mpc_imagref(out->complex), mpc_imagref(num->complex));

            return;
        }

        case NUM_REAL: {
            numInit(out, NUM_REAL);

            mpfr_ceil(out->real, num->real);

            return;
        }

        case NUM_RATIONAL: {
            numInit(out, NUM_REAL);

            mpz_t tempInt;
            mpz_init(tempInt);

            mpz_cdiv_q(tempInt, mpq_numref(num->rational),
                       mpq_denref(num->rational));
            mpfr_set_z(out->real, tempInt, MPFR_RNDN);

            mpz_clear(tempInt);

            return;
        }

        case NUM_BOOL: {
            numInit(out, NUM_REAL);
            mpfr_set_si(out->real, (num->boolean == 0) ? 0 : 1, MPFR_RNDN);

            return;
        }

        case NUM_ERROR: {
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numCeil(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numCeilInto(result, num);
    return result;
}

// TODO: this needs gamma function for complex nums
Number* numFact(const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            break;
        }

        case NUM_REAL: {
            if (!numCanBeLong(num)) {
                Number* result = numNew(NUM_ERROR);

                char errorString[] = "Factorial is too large to compute";
                size_t errorLength = strlen(errorString);

                numSetError(result, errorString, errorLength);

                return result;
            }

            Number* result = numNew(NUM_REAL);
            mpfr_t numPlusOne;
            mpfr_init2(numPlusOne, MPFR_RNDN);
            mpfr_add_ui(numPlusOne, num->real, 1, MPFR_RNDN);

            if (mpfr_integer_p(numPlusOne) && (mpfr_sgn(numPlusOne) <= 0)) {
                Number* result = numNew(NUM_ERROR);

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

            if (numIsInteger(num) && numSgnSi(num) >= 0) {
                mpz_t tempNumer;
                mpz_init(tempNumer);

                mpz_set(tempNumer, mpq_numref(num->rational));

                if (!mpz_fits_ulong_p(mpq_numref(num->rational))) {
                    Number* result = numNew(NUM_ERROR);

                    char errorString[] = "Factorial is too large to compute";
                    size_t errorLength = strlen(errorString);

                    numSetError(result, errorString, errorLength);

                    return result;
                }

                // unsigned long numerLong = mpz_get_ui(tempNumer);
                mpz_clear(tempNumer);
            }

            Number* result = numNew(NUM_REAL);

            return result;
        }

        case NUM_BOOL: {
            Number* result = numNew(NUM_ERROR);
            mpfr_set_si(result->real, 1, MPFR_RNDN);

            return result;
        }

        case NUM_ERROR: {
            Number* result = numNew(NUM_ERROR);
            numSet(result, num);

            return result;
        }

        default: {
            exit(1);
        }
    }
}

void numSubfactInto(Number* out, const Number* num) {
    numClear(out);

    if (num->kind == NUM_ERROR) {
        numInit(out, NUM_ERROR);
        numSet(out, num);

        return;
    }

    int sgn = numSgnSi(num);

    if (sgn < 0) {
        numInit(out, NUM_ERROR);

        char error[] = "Subfactorial is undefined for negative numbers";
        numSetError(out, error, strlen(error));

        return;
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

    Number* zPlusOne = numNew(NUM_COMPLEX);
    mpc_add_ui(zPlusOne->complex, z, 1, MPC_RNDNN);

    Number* gammaZplusOne = complexGamma(zPlusOne);

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

    numInit(out, num->kind);

    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_set(out->complex, gammaZplusOne->complex, MPC_RNDNN);
            break;
        }

        case NUM_REAL: {
            mpfr_set(out->real, mpc_realref(gammaZplusOne->complex), MPFR_RNDN);
            break;
        }

        case NUM_RATIONAL: {
            mpz_t tempInt;
            mpz_init(tempInt);
            mpfr_get_z(tempInt, mpc_realref(gammaZplusOne->complex), MPFR_RNDN);
            mpq_set_z(out->rational, tempInt);
            mpz_clear(tempInt);

            break;
        }

        case NUM_BOOL: {
            out->boolean =
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

    return;
}

Number* numSubfact(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numSubfactInto(result, num);
    return result;
}

// TODO
void numGammaInto(Number* out, const Number* num) {
    numClear(out);
    if (numSgnSi(num) == 0) {
        numInit(out, NUM_ERROR);

        const char error[] = "Gamma is undefined for zero";
        numSetError(out, error, strlen(error));

        return;
    }

    switch (num->kind) {
        case NUM_COMPLEX: {
            Number* result = complexGamma(num);

            return;
        }

        case NUM_REAL: {
            Number* result = numNew(NUM_REAL);
            mpfr_gamma(result->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            Number* result = numGamma(realNum);

            numFree(realNum);
            return;
        }

        case NUM_BOOL: {
            // bool == 0 will not reach this case
            Number* result = numNew(NUM_REAL);
            numSetRealSd(result, 1);

            return;
        }

        case NUM_ERROR: {
            Number* result = numNew(NUM_ERROR);
            numSet(result, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numGamma(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numGammaInto(result, num);
    return result;
}

static Number* complexGamma(const Number* num) {
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
static Number* SpougeApprox(const Number* num) {
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

    for (int k = 1; k < a; k++) {
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

    for (int k = 1; k < a; k++) {
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

    Number* result = numNew(NUM_COMPLEX);

    mpc_set(result->complex, sum, MPC_RNDNN);

    mpc_clear(sum);

    return result;
}

static Number* eulerReflection(const Number* num) {
    Number* oneMinusZ = numNew(NUM_COMPLEX);
    mpc_ui_sub(oneMinusZ->complex, 1, num->complex, MPC_RNDNN);

    Number* gammaOneMinusZ = SpougeApprox(oneMinusZ);

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

    Number* result = numNew(NUM_COMPLEX);
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

void numSinInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);

            numInit(out, NUM_COMPLEX);
            mpc_sin(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numClear(out);

            numInit(out, NUM_REAL);
            mpfr_sin(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numSinInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numSinInto(out, realNum);

            numFree(realNum);
            return;
        }

        case NUM_ERROR: {
            numClear(out);

            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numSin(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numSinInto(result, num);
    return result;
}

void numSinhInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_sinh(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_sin(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numSinInto(realNum, out);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numSinInto(out, realNum);

            numFree(realNum);
            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numSinh(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numSinhInto(result, num);
    return result;
}

void numCosInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_cos(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_cos(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCosInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCosInto(out, realNum);

            numFree(realNum);
            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numCos(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numCosInto(result, num);
    return result;
}

void numCoshInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_cosh(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_cosh(out->real, num->real, MPC_RNDNN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCoshInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCoshInto(out, realNum);

            numFree(realNum);
            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numCosh(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numCoshInto(result, num);
    return result;
}

void numTanInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_tan(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_tan(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numTanInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numTanInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numTan(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numTanInto(result, num);
    return result;
}

void numTanhInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_tanh(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_tanh(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numTanhInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numTanhInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numTanh(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numTanhInto(result, num);
    return result;
}

void numCosecInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempSin;
            mpc_init2(tempSin, PRECISION);
            mpc_sin(tempSin, num->complex, MPC_RNDNN);

            if (mpc_cmp_si_si(tempSin, 0, 0) == 0) {
                numClear(out);
                numInit(out, NUM_ERROR);
                char error[] = "Cosec is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }

            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_ui_div(out->complex, 1, tempSin, MPC_RNDNN);
            mpc_clear(tempSin);

            return;
        }

        case NUM_REAL: {
            numClear(out);
            mpfr_t tempSin;
            mpfr_init2(tempSin, PRECISION);
            mpfr_sin(tempSin, num->real, MPFR_RNDN);

            if (mpfr_cmp_si(tempSin, 0) == 0) {
                numInit(out, NUM_ERROR);
                char error[] = "Cosec is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }

            numInit(out, NUM_REAL);
            mpfr_ui_div(out->real, 1, tempSin, MPFR_RNDN);
            mpfr_clear(tempSin);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCosecInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCosecInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numCosec(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numCosecInto(result, num);
    return result;
}

void numCosechInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempSinh;
            mpc_init2(tempSinh, PRECISION);
            mpc_sinh(tempSinh, num->complex, MPC_RNDNN);

            numClear(out);

            if (mpc_cmp_si_si(tempSinh, 0, 0) == 0) {
                numInit(out, NUM_ERROR);
                char error[] = "Cosech is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }
            numInit(out, NUM_COMPLEX);
            mpc_ui_div(out->complex, 1, tempSinh, MPC_RNDNN);
            mpc_clear(tempSinh);

            return;
        }

        case NUM_REAL: {
            mpfr_t tempSinh;
            mpfr_init2(tempSinh, PRECISION);
            mpfr_sinh(tempSinh, num->real, MPFR_RNDN);

            numClear(out);

            if (mpfr_cmp_si(tempSinh, 0) == 0) {
                numInit(out, NUM_ERROR);
                char error[] = "Cosec is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }
            numInit(out, NUM_REAL);
            mpfr_ui_div(out->real, 1, tempSinh, MPFR_RNDN);
            mpfr_clear(tempSinh);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCosechInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCosechInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numCosech(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numCosechInto(result, num);
    return result;
}

void numSecInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempCos;
            mpc_init2(tempCos, PRECISION);
            mpc_cos(tempCos, num->complex, MPC_RNDNN);

            numClear(out);

            if (mpc_cmp_si_si(tempCos, 0, 0) == 0) {
                numInit(out, NUM_ERROR);
                char error[] = "Sec is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }

            numInit(out, NUM_COMPLEX);
            mpc_ui_div(out->complex, 1, tempCos, MPC_RNDNN);
            mpc_clear(tempCos);

            return;
        }

        case NUM_REAL: {
            mpfr_t tempCos;
            mpfr_init2(tempCos, PRECISION);
            mpfr_cos(tempCos, num->real, MPFR_RNDN);

            numClear(out);

            if (mpfr_cmp_si(tempCos, 0) == 0) {
                numInit(out, NUM_ERROR);
                char error[] = "Sec is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }

            numInit(out, NUM_REAL);
            mpfr_ui_div(out->real, 1, tempCos, MPFR_RNDN);
            mpfr_clear(tempCos);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numSecInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numSecInto(out, realNum);

            numFree(realNum);
            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numSec(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numSecInto(result, num);
    return result;
}

void numSechInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempCosh;
            mpc_init2(tempCosh, PRECISION);
            mpc_cosh(tempCosh, num->complex, MPC_RNDNN);

            numClear(out);

            if (mpc_cmp_si_si(tempCosh, 0, 0) == 0) {
                numInit(out, NUM_ERROR);
                char error[] = "Sech is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }

            numInit(out, NUM_COMPLEX);
            mpc_ui_div(out->complex, 1, tempCosh, MPC_RNDNN);
            mpc_clear(tempCosh);

            return;
        }

        case NUM_REAL: {
            mpfr_t tempCosh;
            mpfr_init2(tempCosh, PRECISION);
            mpfr_cosh(tempCosh, num->real, MPFR_RNDN);

            numClear(out);

            if (mpfr_cmp_si(tempCosh, 0) == 0) {
                numInit(out, NUM_ERROR);
                char error[] = "Sech is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }

            numInit(out, NUM_REAL);
            mpfr_ui_div(out->real, 1, tempCosh, MPFR_RNDN);
            mpfr_clear(tempCosh);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numSechInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numSechInto(out, realNum);

            numFree(realNum);
            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numSech(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numSechInto(result, num);
    return result;
}

void numCotInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempTan;
            mpc_init2(tempTan, PRECISION);
            mpc_tan(tempTan, num->complex, MPC_RNDNN);

            numClear(out);

            if (mpc_cmp_si_si(tempTan, 0, 0) == 0) {
                numInit(out, NUM_ERROR);
                char error[] = "Cot is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }

            numInit(out, NUM_COMPLEX);
            mpc_ui_div(out->complex, 1, tempTan, MPC_RNDNN);
            mpc_clear(tempTan);

            return;
        }

        case NUM_REAL: {
            mpfr_t tempTan;
            mpfr_init2(tempTan, PRECISION);
            mpfr_tan(tempTan, num->real, MPFR_RNDN);

            numClear(out);

            if (mpfr_cmp_si(tempTan, 0) == 0) {
                numInit(out, NUM_ERROR);
                char error[] = "Cot is undefined for this value";
                numSetError(out, error, strlen(error));
                return;
            }

            numInit(out, NUM_REAL);
            mpfr_ui_div(out->real, 1, tempTan, MPFR_RNDN);
            mpfr_clear(tempTan);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCotInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCotInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numCot(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numCotInto(result, num);
    return result;
}

void numCothInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            mpc_t tempTanh;
            mpc_init2(tempTanh, PRECISION);
            mpc_tanh(tempTanh, num->complex, MPC_RNDNN);

            if (mpc_cmp_si_si(tempTanh, 0, 0) == 0) {
                Number* result = numNew(NUM_ERROR);
                char error[] = "Coth is undefined for this value";
                numSetError(result, error, strlen(error));
                return;
            }

            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_ui_div(out->complex, 1, tempTanh, MPC_RNDNN);
            mpc_clear(tempTanh);
            return;
        }

        case NUM_REAL: {
            mpfr_t tempTanh;
            mpfr_init2(tempTanh, PRECISION);
            mpfr_tanh(tempTanh, num->real, MPFR_RNDN);

            if (mpfr_cmp_si(tempTanh, 0) == 0) {
                Number* result = numNew(NUM_ERROR);
                char error[] = "Coth is undefined for this value";
                numSetError(result, error, strlen(error));
                return;
            }

            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_ui_div(out->real, 1, tempTanh, MPFR_RNDN);
            mpfr_clear(tempTanh);
            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCothInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCothInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numCoth(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numCothInto(result, num);
    return result;
}

int numSgnSi(const Number* num) {
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

        default: {
            exit(1);
        }
    }
}

void numSgnInto(Number* out, const Number* num) {
    numClear(out);

    switch (num->kind) {
        case NUM_COMPLEX: {
            if (mpc_cmp_si_si(num->complex, 0, 0) == 0) {
                numInit(out, NUM_REAL);
                mpfr_set_si(out->real, 0, MPFR_RNDN);

                return;
            }

            numInit(out, NUM_COMPLEX);

            mpfr_t tempMagnitude;
            mpfr_init2(tempMagnitude, PRECISION);

            mpc_abs(tempMagnitude, num->complex, MPFR_RNDN);
            mpc_div_fr(out->complex, num->complex, tempMagnitude, MPC_RNDNN);

            mpfr_clear(tempMagnitude);

            return;
        }

        case NUM_REAL: {
            numInit(out, NUM_REAL);
            int sgnInt = mpfr_sgn(num->real);
            mpfr_set_ui(out->real, sgnInt, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            numInit(out, NUM_REAL);
            int sgnInt = mpq_sgn(num->rational);
            mpfr_set_ui(out->real, sgnInt, MPFR_RNDN);

            return;
        }

        case NUM_BOOL: {
            numInit(out, NUM_REAL);
            mpfr_set_ui(out->real, (num->boolean == 0) ? 0 : 1, MPFR_RNDN);

            return;
        }

        case NUM_ERROR: {
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numSgn(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numSgnInto(result, num);
    return result;
}

// checks if real part of num is 1
// returns false if num has complex part
bool numIsUnity(const Number* num) {
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

        default: {
            exit(1);
        }
    }
}

void numAbsInto(Number* out, const Number* num) {
    numClear(out);
    switch (num->kind) {
        case NUM_COMPLEX: {
            numInit(out, NUM_REAL);
            mpc_abs(out->real, num->complex, MPFR_RNDN);

            return;
        }

        case NUM_REAL: {
            numInit(out, NUM_REAL);
            mpfr_abs(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            numInit(out, NUM_RATIONAL);
            mpq_abs(out->rational, num->rational);

            return;
        }

        case NUM_BOOL: {
            numInit(out, NUM_REAL);
            numSetRealSd(out, (num->boolean == 0 ? 0 : 1));

            return;
        }

        case NUM_ERROR: {
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

// returns modulus of complex num
// and abs for others
Number* numAbs(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numAbsInto(result, num);
    return result;
}

void numPowInto(Number* out, const Number* base, const Number* exp) {
    if (base->kind == NUM_ERROR || exp->kind == NUM_ERROR) {
        numClear(out);
        numInit(out, NUM_ERROR);
        numSet(out, (base->kind == NUM_ERROR) ? base : exp);

        return;
    }

    if (base->kind == NUM_BOOL || exp->kind == NUM_BOOL) {
        NumberKind promotedKind = numPromoteKind(base->kind, exp->kind);

        Number* promotedBase = numConvertandSet(base, promotedKind);
        Number* promotedExp = numConvertandSet(exp, promotedKind);

        numPowInto(out, promotedBase, promotedExp);

        numFree(promotedBase);
        numFree(promotedExp);

        return;
    }

    // exp is an integer, keep base's kind the same
    if (numIsInteger(exp)) {
        // TODO: add exception to fllowing error for zero, one and inf
        if (!numCanBeLong(exp)) {
            numClear(out);
            numInit(out, NUM_ERROR);

            char errorString[] = "Exponent is too long to compute";
            size_t errorLength = strlen(errorString);

            numSetError(out, errorString, errorLength);

            return;
        }

        long expLong = numToLong(exp);

        switch (base->kind) {
            case NUM_COMPLEX: {
                if (mpc_cmp_si(base->complex, 0) == 0 && expLong < 0) {
                    numClear(out);
                    numInit(out, NUM_ERROR);

                    char errorString[] = "Cannot raise zero to a -ve power";
                    size_t errorLength = strlen(errorString);

                    numSetError(out, errorString, errorLength);

                    return;
                }
                numClear(out);
                numInit(out, NUM_COMPLEX);
                mpc_pow_si(out->complex, base->complex, expLong, MPC_RNDNN);

                return;
            }

            case NUM_REAL: {
                if (mpfr_zero_p(base->real) && expLong < 0) {
                    numClear(out);
                    numInit(out, NUM_ERROR);

                    char errorString[] = "Cannot raise zero to a -ve power";
                    size_t errorLength = strlen(errorString);

                    numSetError(out, errorString, errorLength);

                    return;
                }
                numClear(out);
                numInit(out, NUM_REAL);
                mpfr_pow_si(out->real, base->real, expLong, MPFR_RNDN);

                return;
            }

            case NUM_RATIONAL: {
                if (mpq_sgn(base->rational) == 0 && expLong < 0) {
                    numClear(out);
                    numInit(out, NUM_ERROR);

                    char errorString[] = "Cannot raise zero to a -ve power";
                    size_t errorLength = strlen(errorString);

                    numSetError(out, errorString, errorLength);

                    return;
                }
                numClear(out);
                numInit(out, NUM_RATIONAL);

                if (expLong >= 0) {
                    mpz_t numer, denom;
                    mpz_init(numer);
                    mpz_init(denom);

                    mpz_pow_ui(numer, mpq_numref(base->rational), expLong);
                    mpz_pow_ui(denom, mpq_denref(base->rational), expLong);

                    mpq_set_num(out->rational, numer);
                    mpq_set_den(out->rational, denom);

                    mpq_canonicalize(out->rational);

                    mpz_clear(numer);
                    mpz_clear(denom);

                } else {
                    mpq_t invertedNum;
                    mpq_init(invertedNum);

                    if (numSgn(base) == 0) {
                        numClear(out);
                        numInit(out, NUM_ERROR);

                        char errorString[] = "Cannot raise zero to a -ve power";
                        size_t errorLength = strlen(errorString);

                        numSetError(out, errorString, errorLength);

                        return;
                    }

                    mpq_inv(invertedNum, base->rational);

                    mpz_t numer, denom;
                    mpz_init(numer);
                    mpz_init(denom);

                    mpz_pow_ui(numer, mpq_numref(invertedNum), -expLong);
                    mpz_pow_ui(denom, mpq_denref(invertedNum), -expLong);

                    mpq_set_num(out->rational, numer);
                    mpq_set_den(out->rational, denom);

                    mpq_canonicalize(out->rational);

                    mpz_clear(numer);
                    mpz_clear(denom);

                    mpq_clear(invertedNum);
                }

                return;
            }

            case NUM_BOOL: {
                // a num of kind NUM_ERROR will
                //  not reach this case block
                return;
            }

            case NUM_ERROR: {
                // a num of kind NUM_ERROR will
                //  not reach this case block
                return;
            }
        }
    }

    // base is complex, result will be complex
    if (base->kind == NUM_COMPLEX) {
        Number* complexExp = numConvertandSet(exp, NUM_COMPLEX);
        numClear(out);
        numInit(out, NUM_COMPLEX);

        mpc_pow(out->complex, base->complex, complexExp->complex, MPC_RNDNN);

        numFree(complexExp);

        return;
    }

    // base is less than zero, result can be complex
    if (numSgnSi(base) < 0) {
        Number* complexBase = numConvertandSet(base, NUM_COMPLEX);
        Number* complexExp = numConvertandSet(exp, NUM_COMPLEX);

        numClear(out);
        numInit(out, NUM_COMPLEX);

        mpc_pow(out->complex, complexBase->complex, complexExp->complex,
                MPC_RNDNN);

        numFree(complexBase);
        numFree(complexExp);

        return;
    } else {
        Number* realBase = numConvertandSet(base, NUM_REAL);
        Number* realExp = numConvertandSet(exp, NUM_REAL);

        numClear(out);
        numInit(out, NUM_REAL);

        mpfr_pow(out->real, realBase->real, realExp->real, MPFR_RNDN);

        numFree(realBase);
        numFree(realExp);
        return;
    }
}

Number* numPow(const Number* base, const Number* exp) {
    Number* result = numNew(NUM_BOOL);
    numPowInto(result, base, exp);
    return result;
}

void numSqrtInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_sqrt(out->complex, num->complex, MPC_RNDNN);
            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_sqrt(out->real, num->real, MPFR_RNDN);
            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numSqrtInto(out, realNum);
            numFree(realNum);
            return;
        }

        case NUM_BOOL: {
            numClear(out);
            numInit(out, NUM_REAL);
            numSetRealSd(out, num->boolean == 0 ? 0 : 1);
            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);
            return;
        }

        default:
            exit(1);
    }
}

Number* numSqrt(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numSqrtInto(result, num);
    return result;
}

void numCbrtInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            Number* half = numNew(NUM_REAL);
            numSetRealSd(half, 0.5);
            numPowInto(out, num, half);
            numClear(half);
            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_cbrt(out->real, num->real, MPFR_RNDN);
            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numCbrtInto(out, realNum);
            numFree(realNum);
            return;
        }

        case NUM_BOOL: {
            numClear(out);
            numInit(out, NUM_REAL);
            numSetRealSd(out, num->boolean == 0 ? 0 : 1);
            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);
            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numCbrt(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numCbrtInto(result, num);
    return result;
}

void numExpInto(Number* out, const Number* num) {
    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_exp(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_exp(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numExpInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_set_ui(out->real, (num->boolean == 0) ? 1 : M_E, MPFR_RNDN);

            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numExp(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numExpInto(result, num);
    return result;
}

void numLnInto(Number* out, const Number* num) {
    if (numSgnSi(num) == 0) {
        numClear(out);
        numInit(out, NUM_ERROR);

        char error[] = "Log is undefined for 0";
        numSetError(out, error, strlen(error));

        return;
    }

    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_log(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_log(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numLnInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            // a bool == 0 will not reach this case
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_set_si(out->real, 0, MPFR_RNDN);

            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numLn(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numLnInto(result, num);
    return result;
}

void numLogInto(Number* out, const Number* num) {
    if (numSgnSi(num) == 0) {
        numClear(out);
        numInit(out, NUM_ERROR);

        char error[] = "Log is undefined for 0";
        numSetError(out, error, strlen(error));

        return;
    }

    switch (num->kind) {
        case NUM_COMPLEX: {
            numClear(out);
            numInit(out, NUM_COMPLEX);
            mpc_log10(out->complex, num->complex, MPC_RNDNN);

            return;
        }

        case NUM_REAL: {
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_log10(out->real, num->real, MPFR_RNDN);

            return;
        }

        case NUM_RATIONAL: {
            Number* realNum = numConvertandSet(num, NUM_REAL);
            numLogInto(out, realNum);
            numFree(realNum);

            return;
        }

        case NUM_BOOL: {
            // a bool == 0 will not reach this case
            numClear(out);
            numInit(out, NUM_REAL);
            mpfr_set_ui(out->real, 0, MPFR_RNDN);

            return;
        }

        case NUM_ERROR: {
            numClear(out);
            numInit(out, NUM_ERROR);
            numSet(out, num);

            return;
        }

        default: {
            exit(1);
        }
    }
}

Number* numLog(const Number* num) {
    Number* result = numNew(NUM_BOOL);
    numLogInto(result, num);
    return result;
}

void numNprInto(Number* out, const Number* n, const Number* r) {
    // TODO: fact is undefined for complex nums
    if (n->kind == NUM_COMPLEX || r->kind == NUM_COMPLEX) {
    }

    Number* realN = numConvertandSet(n, NUM_REAL);
    Number* realR = numConvertandSet(r, NUM_REAL);
    Number* nMinusR = numSubtract(realN, realR);

    numFree(realR);

    Number* nFact = numFact(realN);
    Number* nMinusrFact = numFact(nMinusR);

    numFree(realN);
    numFree(nMinusR);

    numDivideInto(out, nFact, nMinusrFact);

    numFree(nFact);
    numFree(nMinusrFact);

    return;
}

Number* numNpr(const Number* n, const Number* r) {
    Number* result = numNew(NUM_BOOL);
    numNprInto(result, n, r);
    return result;
}

void numNcrInto(Number* out, const Number* n, const Number* r) {
    // TODO: fact is undefined for complex nums
    if (n->kind == NUM_COMPLEX || r->kind == NUM_COMPLEX) {
    }

    Number* realN = numConvertandSet(n, NUM_REAL);
    Number* realR = numConvertandSet(r, NUM_REAL);
    Number* nMinusR = numSubtract(realN, realR);

    Number* nFact = numFact(realN);
    Number* rFact = numFact(realR);
    Number* nMinusrFact = numFact(nMinusR);

    numFree(realN);
    numFree(realR);
    numFree(nMinusR);

    Number* denom = numMultiply(rFact, nMinusrFact);
    numDivideInto(out, nFact, denom);

    numFree(nFact);
    numFree(rFact);
    numFree(nMinusrFact);
    numFree(denom);

    return;
}

Number* numNcr(const Number* n, const Number* r) {
    Number* result = numNew(NUM_BOOL);
    numNcrInto(result, n, r);
    return result;
}

static void numShiftRightSiInto(Number* out, const Number* num,
                                unsigned long bits) {
    numClear(out);
    switch (num->kind) {
        case NUM_COMPLEX: {
            numInit(out, NUM_COMPLEX);
            mpc_div_2ui(out->complex, num->complex, bits, MPC_RNDNN);
            return;
        }

        case NUM_REAL: {
            numInit(out, NUM_REAL);
            mpfr_div_2ui(out->real, num->real, bits, MPFR_RNDN);
            return;
        }

        case NUM_RATIONAL: {
            numInit(out, NUM_RATIONAL);
            mpq_div_2exp(out->rational, num->rational, bits);
            return;
        }

        case NUM_BOOL: {
            numInit(out, NUM_REAL);
            mpfr_t temp;
            mpfr_init2(temp, PRECISION);

            mpfr_set_ui(temp, num->boolean == 0 ? 0 : 1, MPFR_RNDN);
            mpfr_div_2exp(out->real, temp, bits, MPFR_RNDN);

            mpfr_clear(temp);
            return;
        }

        case NUM_ERROR: {
            // NUM_ERROR does not reach this case
            return;
        }

        default: {
            exit(1);
        }
    }
}

// must be called from numShiftRight or numShiftLeft
static Number* numShiftRightSi(const Number* num, unsigned long bits) {
    Number* result = numNew(NUM_BOOL);
    numShiftRightSiInto(result, num, bits);
    return result;
}

static void numShiftLeftSiInto(Number* out, const Number* num,
                               unsigned long bits) {
    numClear(out);
    switch (num->kind) {
        case NUM_COMPLEX: {
            numInit(out, NUM_COMPLEX);
            mpc_mul_2ui(out->complex, num->complex, bits, MPC_RNDNN);
            return;
        }

        case NUM_REAL: {
            numInit(out, NUM_REAL);
            mpfr_mul_2ui(out->real, num->real, bits, MPC_RNDNN);
            return;
        }

        case NUM_RATIONAL: {
            numInit(out, NUM_RATIONAL);
            mpq_mul_2exp(out->rational, num->rational, bits);
            return;
        }

        case NUM_BOOL: {
            numInit(out, NUM_BOOL);
            mpfr_t temp;
            mpfr_init2(temp, PRECISION);

            mpfr_set_ui(temp, num->boolean == 0 ? 0 : 1, MPFR_RNDN);
            mpfr_mul_2exp(out->real, temp, bits, MPFR_RNDN);

            mpfr_clear(temp);
            return;
        }

        case NUM_ERROR: {
            // NUM_ERROR does not reach this case
            return;
        }

        default: {
            exit(1);
        }
    }
}

// must be called from numShiftRight or numShiftLeft
static Number* numShiftLeftSi(const Number* num, unsigned long bits) {
    Number* result = numNew(NUM_BOOL);
    numShiftLeftSiInto(result, num, bits);
    return result;
}

void numShiftRightInto(Number* out, const Number* num, const Number* bits) {
    numClear(out);
    if (!numIsInteger(bits)) {
        numInit(out, NUM_ERROR);
        char error[] = "Number of bits to be shifted must be an integer";
        numSetError(out, error, strlen(error));
        return;
    }

    if (num->kind == NUM_ERROR) {
        numInit(out, NUM_ERROR);
        setStringView(&out->error, num->error.arr, num->error.length);
        return;
    }

    unsigned long bitsUlong = 0;
    int unsignedLongresult = numCanBeUnsignedLong(bits);

    if (unsignedLongresult == 0) {
        bitsUlong = numToUnsignedLong(bits);
    } else if (unsignedLongresult == 1) {
        // bit shift left
        numShiftLeftSiInto(out, num, bitsUlong);
        return;
    } else {
        numInit(out, NUM_ERROR);
        // TODO: change LONG_MAX depending upon the system
        char error[] =
            "Maximum number of bits that can be shifted is ULONG_MAX";
        numSetError(out, error, strlen(error));
        return;
    }

    numShiftRightSiInto(out, num, bitsUlong);
    return;
}

Number* numShiftRight(const Number* num, const Number* bits) {
    Number* result = numNew(NUM_BOOL);
    numShiftRightInto(result, num, bits);
    return result;
}

void numShiftLeftInto(Number* out, const Number* num, const Number* bits) {
    numClear(out);
    if (!numIsInteger(bits)) {
        numInit(out, NUM_ERROR);
        char error[] = "Number of bits to be shifted must be an integer";
        numSetError(out, error, strlen(error));
        return;
    }

    if (num->kind == NUM_ERROR) {
        numInit(out, NUM_ERROR);
        setStringView(&out->error, num->error.arr, num->error.length);
        return;
    }

    unsigned long bitsUlong = 0;
    int unsignedLongresult = numCanBeUnsignedLong(bits);

    if (unsignedLongresult == 0) {
        bitsUlong = numToUnsignedLong(bits);
    } else if (unsignedLongresult == 1) {
        numShiftRightSiInto(out, num, bitsUlong);
        return;
    } else {
        numInit(out, NUM_ERROR);
        // TODO: change LONG_MAX depending upon the system
        char error[] =
            "Maximum number of bits that can be shifted is ULONG_MAX";
        numSetError(out, error, strlen(error));
        return;
    }

    numShiftLeftSiInto(out, num, bitsUlong);
    return;
}

Number* numShiftLeft(const Number* num, const Number* bits) {
    Number* result = numNew(NUM_BOOL);
    numShiftLeftInto(result, num, bits);
    return result;
}

void numAndInto(Number* out, const Number* a, const Number* b) {
    numClear(out);

    if ((a->kind == NUM_BOOL) && (b->kind == NUM_BOOL)) {
        numInit(out, NUM_BOOL);
        out->boolean = a->boolean && b->boolean;
        return;
    }

    if ((!numIsInteger(a)) || (!numIsInteger(b))) {
        char error[] = "Bitwise operands must be integers";
        numInit(out, NUM_ERROR);
        numSetError(out, error, strlen(error));
        return;
    }

    mpz_t intA, intB, result;
    mpz_init2(intA, PRECISION);
    mpz_init2(intB, PRECISION);
    mpz_init2(result, PRECISION);

    numToInt(intA, a);
    numToInt(intB, b);

    mpz_and(result, intA, intB);

    mpz_clear(intA);
    mpz_clear(intB);
    mpz_clear(result);

    numInit(out, NUM_REAL);
    mpfr_set_z(out->real, result, MPFR_RNDN);
    return;
}

Number* numAnd(const Number* a, const Number* b) {
    Number* result = numNew(NUM_BOOL);
    numAndInto(result, a, b);
    return result;
}

void numOrInto(Number* out, const Number* a, const Number* b) {
    numClear(out);

    if ((a->kind == NUM_BOOL) || (b->kind == NUM_BOOL)) {
        numInit(out, NUM_BOOL);
        out->boolean = a->boolean || b->boolean;
        return;
    }

    if ((!numIsInteger(a)) || (!numIsInteger(b))) {
        char error[] = "Bitwise operands must be integers";
        numInit(out, NUM_ERROR);
        numSetError(out, error, strlen(error));
        return;
    }

    mpz_t intA, intB, result;
    mpz_init2(intA, PRECISION);
    mpz_init2(intB, PRECISION);
    mpz_init2(result, PRECISION);

    numToInt(intA, a);
    numToInt(intB, b);

    mpz_eor(result, intA, intB);

    mpz_clear(intA);
    mpz_clear(intB);
    mpz_clear(result);

    numInit(out, NUM_REAL);
    mpfr_set_z(out->real, result, MPFR_RNDN);
    return;
}

Number* numOr(const Number* a, const Number* b) {
    Number* result = numNew(NUM_BOOL);
    numOrInto(result, a, b);
    return result;
}