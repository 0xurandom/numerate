#pragma once
#include <mpfr.h>
#include <stdbool.h>
#include <stddef.h>

#include "num_utils.h"

// ordered and non duplicated array
typedef struct {
    Number *elements;
    size_t count;
    size_t capacity;
} Set;

Set *newSet();
Set *allocateSet(unsigned long mem);
bool isElement(Set *set, Number *val);
void insertElement(Set *set, Number *val);
bool removeElement(Set *set, Number *val);
Set *getUnion(Set *set1, Set *set2);
Set *getIntersection(Set *set1, Set *set2);
bool isSubset(Set *subset, Set *superset);
bool isSuperset(Set *superset, Set *subset);
Set *getSymmetricDifference(Set *set1, Set *set2);
Set *subtractSets(Set *set1, Set *set2);
void appendToSet(Set *set, Number *element);
bool binarySearch(Set *set, Number *val, size_t *result);
void reallocSet(Set *set);
void freeSet(Set *set);
