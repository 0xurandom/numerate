#include "set_utils.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SET_CAP 10

void initSet(Set *set) {
    set->elements = malloc(DEFAULT_SET_CAP * sizeof(double));
    if (set->elements == NULL) {
        fprintf(stderr, "Error: Could not allocate memory to set\n");
        exit(1);
    }

    set->count = 0;
    set->capacity = DEFAULT_SET_CAP;
}

bool isElement(Set *set, double val) { return binarySearch(set, val, NULL); }

void insertElement(Set *set, double val) {
    size_t index;

    if (binarySearch(set, val, &index)) return;

    if (set->count + 1 > set->capacity)
    // realloc
    {
    }

    for (size_t i = 0; i < index; i++) {
        set->elements[set->count - i + 1] = set->elements[set->count - i];
    }

    set->elements[index] = val;
}

// returns true is val is found
// and sets result to index where it is/should be
bool binarySearch(Set *set, double val, size_t *result) {
    size_t low = 0;
    size_t high = set->count - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (set->elements[mid] == val) {
            *result = val;
            return true;
        }

        if (set->elements[mid] < val)
            low = mid + 1;
        else
            high = mid - 1;
    }

    *result = low;
    return false;
}
