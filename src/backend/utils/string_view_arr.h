#pragma once

#include "string_view_utils.h"

#define STRINGVIEWARR_CAP 7

typedef struct {
    StringView** arr;
    int count;
    int capacity;
} StringViewArr;

StringViewArr* newStringViewArr();
void initStringViewArr(StringViewArr* svArr, int paramCount);
void addStringToStringViewArr(StringViewArr* svArr, StringView* sv);
void freeStringViewArr(StringViewArr* svArr);
void reallocStringViewArr(StringView* svArr);