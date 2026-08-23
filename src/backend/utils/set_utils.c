#include "./set_utils.h"

#include <mpc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "num_ops.h"
#include "num_utils.h"

#define DEFAULT_SET_CAP 10

Set *newSet() {
    Set *set = malloc(sizeof(Set));

    set->elements = malloc(DEFAULT_SET_CAP * sizeof(Number));
    if (set->elements == NULL) {
        fprintf(stderr, "Error: Could not allocate memory to set\n");
        exit(1);
    }

    set->capacity = DEFAULT_SET_CAP;
    for (size_t i = 0; i < set->capacity; i++) {
        numInit(&set->elements[i], NUM_REAL);
    }

    set->count = 0;

    return set;
}

Set *allocateSet(unsigned long mem) {
    Set *set = malloc(sizeof(Set));

    set->capacity = mem;
    set->elements = malloc(mem * sizeof(Number));

    if (set->elements == NULL) {
        fprintf(stderr, "Error: Could not allocate memory to set\n");
        exit(1);
    }

    for (size_t i = 0; i < set->capacity; i++) {
        numInit(&set->elements[i], NUM_REAL);
    }

    set->count = 0;

    return set;
}

bool isElement(const Set *set, const Number *val) {
    size_t index;
    return binarySearch(set, val, &index);
}

void insertElement(Set *set, const Number *val) {
    size_t index;

    if (binarySearch(set, val, &index)) return;

    if (set->count + 1 > set->capacity) {
        reallocSet(set);
    }

    for (size_t i = set->count; i > index; i--) {
        numSet(&set->elements[i], &set->elements[i - 1]);
    }

    numSet(&set->elements[index], val);
    set->count++;
}

// returns true if element was successfully removed,
// false if element was not in the set
bool removeElement(Set *set, const Number *val) {
    size_t index;

    if (binarySearch(set, val, &index) == false) return false;

    for (size_t i = index; i < set->count - 1; i++) {
        numSet(&set->elements[i], &set->elements[i + 1]);
    }

    set->count--;

    return true;
}

Set *getUnion(const Set *set1, const Set *set2) {
    Set *result = allocateSet(set1->count + set2->count);

    size_t i = 0;
    size_t j = 0;

    while (i < set1->count && j < set2->count) {
        int compResult = numCompare(&set1->elements[i], &set2->elements[i]);
        if (compResult == -1) {
            appendToSet(result, &set1->elements[i]);
            i++;

        } else if (compResult == 1) {
            appendToSet(result, &set2->elements[i]);
            j++;

        } else {
            appendToSet(result, &set1->elements[i]);
            i++;
            j++;
        }
    }

    while (i < set1->count) {
        appendToSet(result, &set1->elements[i]);
        i++;
    }

    while (j < set2->count) {
        appendToSet(result, &set2->elements[j]);
        j++;
    }

    return result;
}

Set *getIntersection(const Set *set1, const Set *set2) {
    size_t maxCap = (set1->count > set2->count) ? set1->count : set2->count;

    Set *result = allocateSet(maxCap);

    size_t i = 0;
    size_t j = 0;

    while (i < set1->count && j < set2->count) {
        int compResult = numCompare(&set1->elements[i], &set2->elements[i]);
        if (compResult == -1) {
            i++;

        } else if (compResult == 1) {
            j++;

        } else {
            appendToSet(result, &set1->elements[i]);

            i++;
            j++;
        }
    }

    return result;
}

bool isSubset(const Set *subset, const Set *superset) {
    if (subset->count == 0) return true;
    if (subset->count > superset->count) return false;

    size_t i = 0;
    size_t j = 0;

    while (i < subset->count && j < superset->count) {
        int compResult =
            numCompare(&subset->elements[i], &superset->elements[j]);

        if (compResult == -1) {
            return false;
        } else if (compResult == 1) {
            j++;
        } else {
            i++;
            j++;
        }
    }

    return (i == subset->count);
}

bool isSuperset(const Set *superset, const Set *subset) {
    return isSubset(subset, superset);
}

Set *getSymmetricDifference(const Set *set1, const Set *set2) {
    size_t maxCap = (set1->count > set2->count) ? set1->count : set2->count;

    Set *result = allocateSet(maxCap);

    size_t i = 0;
    size_t j = 0;

    while (i < set1->count && j < set2->count) {
        int compResult = numCompare(&set1->elements[i], &set2->elements[j]);
        if (compResult == -1) {
            appendToSet(result, &set1->elements[i]);
            i++;

        } else if (compResult == 1) {
            appendToSet(result, &set2->elements[j]);
            j++;

        } else {
            i++;
            j++;
        }
    }

    while (i < set1->count) {
        appendToSet(result, &set1->elements[i]);
        i++;
    }

    while (j < set2->count) {
        appendToSet(result, &set2->elements[j]);
        j++;
    }

    return result;
}

Set *subtractSets(const Set *set1, const Set *set2) {
    size_t maxCap = set1->count;
    Set *result = allocateSet(maxCap);

    size_t i = 0;
    size_t j = 0;

    while (i < set1->count && j < set2->count) {
        int compResult = numCompare(&set1->elements[i], &set2->elements[j]);
        if (compResult == -1) {
            appendToSet(result, &set1->elements[i]);
            i++;

        } else if (compResult == 1) {
            j++;

        } else {
            i++;
            j++;
        }
    }

    while (i < set1->count) {
        appendToSet(result, &set1->elements[i]);
        i++;
    }

    return result;
}

// only use if it is known that the
// element will be added to the end
void appendToSet(Set *set, const Number *element) {
    if (set->count + 1 > set->capacity) {
        reallocSet(set);
    }

    numSet(&set->elements[set->count], element);
    set->count++;
}

// returns true is val is found
// and sets result to index where it is/should be
bool binarySearch(const Set *set, const Number *val, size_t *result) {
    size_t low = 0;
    size_t high = set->count - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (numCompare(&set->elements[mid], val) == 0) {
            *result = mid;

            return true;
        }

        if (numCompare(&set->elements[mid], val) < 0)
            low = mid + 1;
        else
            high = mid - 1;
    }

    *result = low;
    return false;
}

void reallocSet(Set *set) {
    set->elements = realloc(set->elements, 2 * set->capacity * sizeof(Number));

    if (set->elements == NULL) {
        fprintf(stderr, "Could not reallocate set\n");
        exit(1);
    }

    set->capacity *= 2;
}

void freeSet(Set *set) {
    for (size_t i = 0; i < set->capacity; i++) {
        numFree(&set->elements[i]);
    }

    free(set->elements);

    set->count = 0;
    set->capacity = 0;
    free(set);
}
