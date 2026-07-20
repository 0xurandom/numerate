#include "set_utils.h"

#include <mpc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SET_CAP 10

Set *newSet() {
    Set *set = malloc(sizeof(Set));

    set->elements = malloc(DEFAULT_SET_CAP * sizeof(mpfr_t));
    if (set->elements == NULL) {
        fprintf(stderr, "Error: Could not allocate memory to set\n");
        exit(1);
    }

    for (size_t i = 0; i < set->capacity; i++) {
        mpfr_init_set_d(set->elements[i], (double)0, MPFR_RNDN);
    }

    set->count = 0;
    set->capacity = DEFAULT_SET_CAP;

    return set;
}

Set *allocateSet(unsigned long mem) {
    Set *set = malloc(sizeof(Set));

    set->elements = malloc(mem * sizeof(mpfr_t));

    if (set->elements == NULL) {
        fprintf(stderr, "Error: Could not allocate memory to set\n");
        exit(1);
    }

    set->count = 0;
    set->capacity = mem;

    return set;
}

bool isElement(Set *set, mpfr_t val) { return binarySearch(set, val, NULL); }

void insertElement(Set *set, mpfr_t val) {
    size_t index;

    if (binarySearch(set, val, &index)) return;

    if (set->count + 1 > set->capacity) {
        reallocSet(set);
    }

    for (size_t i = 0; i < index; i++) {
        mpfr_set(set->elements[set->count - i + 1],
                 set->elements[set->count - i], MPFR_RNDN);
    }

    mpfr_set(set->elements[index], val, MPFR_RNDN);
}

// returns true if element was successfully removed,
// false if element was not in the set
bool removeElement(Set *set, mpfr_t val) {
    size_t index;

    bool valExists = binarySearch(set, val, &index);

    if (valExists == false) return false;

    for (size_t i = index; i < set->count - 1; i++) {
        mpfr_set(set->elements[i], set->elements[i + 1], MPFR_RNDN);
    }

    set->count--;

    return true;
}

Set *getUnion(Set *set1, Set *set2) {
    size_t maxCap = set1->count > set2->count ? set1->count : set2->count;

    Set *result = allocateSet(maxCap);

    size_t i = 0;
    size_t j = 0;

    while (i < set1->count && j < set2->count) {
        if (set1->elements[i] < set2->elements[j]) {
            appendToSet(result, set1->elements[i]);
            i++;

        } else if (set1->elements[i] > set2->elements[i]) {
            appendToSet(result, set2->elements[i]);
            j++;

        } else {
            appendToSet(result, set1->elements[i]);
            i++;
            j++;
        }
    }

    return result;
}

Set *getIntersection(Set *set1, Set *set2) {
    size_t maxCap = (set1->count > set2->count) ? set1->count : set2->count;

    Set *result = allocateSet(maxCap);

    size_t i = 0;
    size_t j = 0;

    while (i < set1->count && j < set2->count) {
        if (set1->elements[i] < set2->elements[j]) {
            i++;

        } else if (set1->elements[i] > set2->elements[j]) {
            j++;

        } else {
            appendToSet(result, set1->elements[i]);

            i++;
            j++;
        }
    }

    return result;
}

bool isSubset(Set *subset, Set *superset) {
    if (subset->count > superset->count) return false;

    size_t i = 0;
    size_t j = 0;

    while (i < subset->count && j < superset->count) {
        if (subset->elements[i] < superset->elements[j]) {
            return false;
        } else if (subset->elements[i] > superset->elements[j]) {
            j++;
        } else {
            i++;
            j++;
        }
    }

    return (i == subset->count - 1);
}

bool isSuperset(Set *superset, Set *subset) {
    return isSubset(subset, superset);
}

Set *getSymmetricDifference(Set *set1, Set *set2) {
    size_t maxCap = (set1->count > set2->count) ? set1->count : set2->count;

    Set *result = allocateSet(maxCap);

    size_t i = 0;
    size_t j = 0;

    while (i < set1->count && j < set2->count) {
        if (set1->elements[i] < set2->elements[j]) {
            appendToSet(result, set1->elements[i]);
            i++;

        } else if (set1->elements[i] > set2->elements[j]) {
            appendToSet(result, set2->elements[j]);
            j++;

        } else {
            i++;
            j++;
        }
    }

    while (i < set1->count) {
        appendToSet(result, set1->elements[i]);
        i++;
    }

    while (j < set2->count) {
        appendToSet(result, set2->elements[j]);
        j++;
    }

    return result;
}

Set *subtractSets(Set *set1, Set *set2) {
    size_t maxCap = (set1->count > set2->count) ? set1->count : set2->count;

    Set *result = allocateSet(maxCap);

    size_t i = 0;
    size_t j = 0;

    while (i < set1->count && j < set2->count) {
        if (set1->elements[i] < set2->elements[j]) {
            appendToSet(result, set1->elements[i]);
            i++;

        } else if (set1->elements[i] > set2->elements[i]) {
            j++;

        } else {
            i++;
            j++;
        }
    }

    return result;
}

// only use if it is known that the
// element will be added to the end
void appendToSet(Set *set, mpfr_t element) {
    // TODO: check capacity

    mpfr_set(set->elements[set->count], element, MPFR_RNDN);
    set->count++;
}

// returns true is val is found
// and sets result to index where it is/should be
bool binarySearch(Set *set, mpfr_t val, size_t *result) {
    size_t low = 0;
    size_t high = set->count - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (mpfr_cmp(set->elements[mid], val) == 0) {
            *result = mid;

            return true;
        }

        if (mpfr_cmp(set->elements[mid], val) < 0)
            low = mid + 1;
        else
            high = mid - 1;
    }

    *result = low;
    return false;
}

void reallocSet(Set *set) {
    set->elements = realloc(set->elements, 2 * set->capacity);

    if (set->elements == NULL) {
        fprintf(stderr, "Could not reallocate set\n");
        exit(1);
    }

    set->capacity *= 2;
}

void freeSet(Set *set) {
    for (size_t i = 0; i < set->capacity; i++) {
        mpfr_clear(set->elements[i]);
    }

    free(set->elements);
    free(set);

    set->count = 0;
    set->capacity = 0;
}
