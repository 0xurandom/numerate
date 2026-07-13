#include "math_utils.h"

#include <math.h>
#include <stdio.h>

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

double twosComplement(double num) {
    double integer_part;
    double fractional_part = modf(num, &integer_part);

    if (fractional_part != 0) {
        fprintf(stderr,
                "Error: cannot get two's complement of a fractional number\n");
    }

    // TODO: this may overflow
    return (double)(~(int)num + 1);
}
