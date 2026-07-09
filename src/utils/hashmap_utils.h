#pragma once

#include "ll_utils.h"

typedef struct HashMap {
    LL_Node **arr;
    size_t count;
    size_t capacity;
} HashMap;

HashMap newHashmap();
void insertVar(HashMap *hashMap, StringView *stringView, double value);
int lookupVar(HashMap *hashMap, StringView *stringView, double *result);
void deleteVar(HashMap *hashMap, StringView *stringView);
void freeHashmap(HashMap *hashMap);
size_t djb2(char *str, size_t str_length);
