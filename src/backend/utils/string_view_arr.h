#pragma once

#include "string_view_utils.h"

#define STRINGVIEWARR_CAP 7

typedef struct {
    StringView* arr;
    int count;
    int capacity;
} StringViewArr;