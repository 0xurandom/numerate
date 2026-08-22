#include "limit_utils.h"

#include <string.h>

#include "../parser.h"
#include "function_utils.h"
#include "num_ops.h"
#include "num_utils.h"

Number *calculateLimit(Parser *parser, Node *funcDefNode, const Number *x) {
    if (funcDefNode->funcDef.params->count != 1) {
        char error[] = "Cannot calculate limits for multivariable functions";
        Number *result = numNew(NUM_ERROR);
        numSetError(result, error, strlen(error));
        return result;
    }

    Number *epsilon = numNew(NUM_REAL);
    mpfr_set_d(epsilon->real, 0.0001, MPFR_RNDN);

    Number *delta = numNew(NUM_REAL);
    mpfr_set_d(delta->real, 0.0001, MPFR_RNDN);

    Number *xPlusEpsi = numAdd(x, epsilon);
    Number *xMinusEpsi = numSubtract(x, epsilon);

    Number *lLimit =
        evaluateFunctionAt(parser, funcDefNode->funcDef.name, xMinusEpsi);
    Number *rLimit =
        evaluateFunctionAt(parser, funcDefNode->funcDef.name, xPlusEpsi);

    Number *diff = numSubtract(rLimit, lLimit);
    Number *absDiff = numAbs(diff);

    Number *result = NULL;

    if (numCompare(absDiff, delta) <= 0) {
        Number *numer = numAdd(rLimit, lLimit);
        Number *denom = numNew(NUM_REAL);
        numSetRealSd(denom, 2);

        result = numDivide(numer, denom);

        numFrees(numer, denom, (Number *)NULL);
    } else {
        char error[] = "Limit does not exist";
        result = numNew(NUM_ERROR);
        numSetError(result, error, strlen(error));
    }

    numFrees(epsilon, delta, xPlusEpsi, xMinusEpsi, lLimit, rLimit, diff,
             absDiff, (Number *)NULL);

    return result;
}
