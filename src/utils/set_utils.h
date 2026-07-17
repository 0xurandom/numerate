#pragma once

#include <stdbool.h>
#include <stddef.h>

// ordered and non duplicated array
typedef struct {
    double *elements;
    size_t count;
    size_t capacity;
} Set;

Set *newSet();
Set *allocateSet(unsigned long mem);
bool isElement(Set *set, double val);
void insertElement(Set *set, double val);
bool removeElement(Set *set, double val);
Set *getUnion(Set *set1, Set *set2);
Set *getIntersection(Set *set1, Set *set2);
bool isSubset(Set *subset, Set *superset);
bool isSuperset(Set *superset, Set *subset);
Set *getSymmetricDifference(Set *set1, Set *set2);
Set *subtractSets(Set *set1, Set *set2);
void appendToSet(Set *set, double element);
bool binarySearch(Set *set, double val, size_t *result);
void appendToSet(Set *set, double element);
void reallocSet(Set *set);
void freeSet(Set *set);
