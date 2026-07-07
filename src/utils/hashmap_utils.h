#pragma once

#include "ll_utils.h"

typedef struct HashMap {
    LL_Node **arr;
    size_t count;
    size_t capacity;
} HashMap;

HashMap newHashmap();
size_t djb2(char *str, size_t str_length);
