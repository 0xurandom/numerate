#pragma once

#include "ll_utils.h"
#include "num_utils.h"

typedef struct HashMap {
    LL_Node **arr;
    size_t count;
    size_t capacity;
} HashMap;

void initHashmap(HashMap *map);
void insertVar(HashMap *hashMap, const StringView *stringView,
               const Number *value);
int lookupVar(HashMap *hashMap, const StringView *stringView, Number *result);
void deleteVar(HashMap *hashMap, const StringView *stringView);
void freeHashmap(HashMap *hashMap);
size_t djb2(char *str, size_t str_length);
