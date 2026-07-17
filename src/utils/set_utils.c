#include "set_utils.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SET_CAP 10

Set *newSet() {
    Set *set = malloc(sizeof(Set));

    set->elements = malloc(DEFAULT_SET_CAP * sizeof(double));
    if (set->elements == NULL) {
        fprintf(stderr, "Error: Could not allocate memory to set\n");
        exit(1);
    }

    set->count = 0;
    set->capacity = DEFAULT_SET_CAP;

    return set;
}

Set *allocateSet(unsigned long mem) {
    Set *set = malloc(sizeof(Set));

    set->elements = malloc(mem * sizeof(double));

    if (set->elements == NULL) {
        fprintf(stderr, "Errpr: Could not allocate memory to set\n");
        exit(1);
    }

    set->count = 0;
    set->capacity = mem;

    return set;
}

bool isElement(Set *set, double val) { return binarySearch(set, val, NULL); }

void insertElement(Set *set, double val) {
    size_t index;

    if (binarySearch(set, val, &index)) return;

    if (set->count + 1 > set->capacity) {
        reallocSet(set);
    }

    for (size_t i = 0; i < index; i++) {
        set->elements[set->count - i + 1] = set->elements[set->count - i];
    }

    set->elements[index] = val;
}

// returns true if element was successfully removed,
// false if element was not in the set
bool removeElement(Set *set, double val) {
    size_t index;

    bool valExists = binarySearch(set, val, &index);

    if (valExists == false) return false;

    for (size_t i = index; i < set->count - 1; i++) {
        set->elements[i] = set->elements[i + 1];
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
}

// only use if it is known that the
// element will be added to the end
void appendToSet(Set *set, double element) {
    // TODO: check capacity

    set->elements[set->count] = element;
    set->count++;
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

void reallocSet(Set *set) {
    set->elements = realloc(set->elements, 2 * set->capacity);

    if (set->elements == NULL) {
        fprintf(stderr, "Could not reallocate set\n");
        exit(1);
    }

    set->capacity *= 2;
}

void freeSet(Set *set) {
    free(set->elements);
    free(set);

    set->count = 0;
    set->capacity = 0;
}
