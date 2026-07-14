#pragma once

#include <stdbool.h>
#include <stddef.h>

// ordered and non duplicated array
typedef struct {
    double *elements;
    size_t count;
    size_t capacity;
} Set;

bool binarySearch(Set *set, double val, size_t *result);
