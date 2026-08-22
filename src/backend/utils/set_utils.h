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
bool isElement(const Set *set, const Number *val);
void insertElement(Set *set, const Number *val);
bool removeElement(Set *set, const Number *val);
Set *getUnion(const Set *set1, const Set *set2);
Set *getIntersection(const Set *set1, const Set *set2);
bool isSubset(const Set *subset, const Set *superset);
bool isSuperset(const Set *superset, const Set *subset);
Set *getSymmetricDifference(const Set *set1, const Set *set2);
Set *subtractSets(const Set *set1, const Set *set2);
void appendToSet(Set *set, const Number *element);
bool binarySearch(const Set *set, const Number *val, size_t *result);
void reallocSet(Set *set);
void freeSet(Set *set);
