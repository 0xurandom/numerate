#pragma once

#include <stdbool.h>
#include <stddef.h>

// ordered and non duplicated array
typedef struct {
    double *elements;
    size_t count;
    size_t capacity;
} Set;

void initSet(Set *set);
bool isElement(Set *set, double val);
void insertElement(Set *set, double val);
bool removeElement(Set *set, double val);
void getUnion(Set *set1, Set *set2, Set *result);
void subtractSets(Set *set1, Set *set2, Set *result);
void appendToSet(Set *set, double element);
bool binarySearch(Set *set, double val, size_t *result);
void appendToSet(Set *set, double element);
