#include "math_utils.h"

#include <math.h>

double factorial(double num) {
    // assuming num is an int
    // TODO: use gamma function
    // to implement decimal factorials
    if (num == 0 || num == 1)
        return 1;
    else
        return num * factorial(num - 1);
}

double subfactorial(double num) {
    // TODO: implement subfactorial
}
