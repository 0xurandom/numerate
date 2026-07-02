#include "math_utils.h"

#include <math.h>

double factorial(double num) { return tgamma(num + 1); }

// TODO: implement subfactorial
// in parser
double subfactorial(double num) {
    // TODO: implement subfactorial
    if (num == 0) return 1;
    if (num == 1) return 0;

    return round(tgamma(num + 1) / exp(1));
}
