#pragma once
#include <mpfr.h>
#include <stdbool.h>
#include <stddef.h>

// ordered and non duplicated array
typedef struct {
    mpfr_t *elements;
    size_t count;
    size_t capacity;
} Set;

Set *newSet();
Set *allocateSet(unsigned long mem);
bool isElement(Set *set, mpfr_t val);
void insertElement(Set *set, mpfr_t val);
bool removeElement(Set *set, mpfr_t val);
Set *getUnion(Set *set1, Set *set2);
Set *getIntersection(Set *set1, Set *set2);
bool isSubset(Set *subset, Set *superset);
bool isSuperset(Set *superset, Set *subset);
Set *getSymmetricDifference(Set *set1, Set *set2);
Set *subtractSets(Set *set1, Set *set2);
void appendToSet(Set *set, mpfr_t element);
bool binarySearch(Set *set, mpfr_t val, size_t *result);
void reallocSet(Set *set);
void freeSet(Set *set);
