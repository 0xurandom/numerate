#include "string_view_arr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// pass 0 as stringLength if unknown
StringViewArr* newStringViewArr(int paramCount) {
    int cap = (paramCount == 0) ? STRINGVIEWARR_CAP : paramCount;

    StringViewArr* svArr = malloc(sizeof(StringViewArr));
    svArr->arr = malloc(cap * sizeof(StringView));
    svArr->capacity = cap;
    svArr->count = 0;
}

// pass 0 as stringLength if unknown
void initStringViewArr(StringViewArr* svArr, int paramCount) {
    int cap = (paramCount == 0) ? STRINGVIEWARR_CAP : paramCount;

    svArr->arr = malloc(cap * sizeof(StringView));
    svArr->capacity = cap;
    svArr->count = 0;

    return;
}

// pass 0 as stringLength if unknown
void addStringToStringViewArr(StringViewArr* svArr, StringView* sv) {
    if (svArr->count + 1 > svArr->capacity) reallocStringViewArr(svArr);

    svArr->arr[svArr->count] = sv;
    svArr->count++;

    return;
}

void freeStringViewArr(StringViewArr* svArr) {
    for (int i = 0; i < svArr->count; i++) {
        freeStringView(svArr->arr[i]);
    }

    free(svArr->arr);
    free(svArr);
    svArr = NULL;

    return;
}

void reallocStringViewArr(StringViewArr* svArr) {
    svArr->arr = realloc(svArr->arr, svArr->capacity * 2);
    if (svArr->arr == NULL) {
        fprintf(stderr, "Error: Could not allocate String View Arr\n");
        exit(1);
    }

    svArr->capacity *= 2;
}