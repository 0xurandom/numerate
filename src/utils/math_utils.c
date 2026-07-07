#include "math_utils.h"

#include <math.h>

double factorial(double num) { return tgamma(num + 1); }

double signum(double num) {
    if (num > 0)
        return 1;
    else if (num < 0)
        return -1;
    else
        return 0;
}

double subfactorial(double num) {
    if (num == 0) return 1;
    if (num == 1) return 0;

    return round(tgamma(num + 1) / exp(1));
}
