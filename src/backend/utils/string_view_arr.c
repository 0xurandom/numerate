#include "string_view_arr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_view_utils.h"

// pass 0 as stringLength if unknown
StringViewArr *newStringViewArr(int paramCount) {
    int cap = (paramCount == 0) ? STRINGVIEWARR_CAP : paramCount;

    StringViewArr *svArr = malloc(sizeof(StringViewArr));
    svArr->arr = malloc(cap * sizeof(StringView));
    svArr->capacity = cap;
    svArr->count = 0;

    return svArr;
}

// pass 0 as stringLength if unknown
void initStringViewArr(StringViewArr *svArr, int paramCount) {
    int cap = (paramCount == 0) ? STRINGVIEWARR_CAP : paramCount;

    svArr->arr = malloc(cap * sizeof(StringView));
    svArr->capacity = cap;
    svArr->count = 0;

    return;
}

// pass 0 as stringLength if unknown
void addStringToStringViewArr(StringViewArr *svArr, StringView *sv) {
    if (svArr->count + 1 > svArr->capacity) reallocStringViewArr(svArr);

    svArr->arr[svArr->count] = sv;
    svArr->count++;

    return;
}

void freeStringViewArr(StringViewArr *svArr) {
    (void)svArr;
    return;
    if (svArr == NULL) return;

    for (int i = 0; i < svArr->count; i++) {
        if (svArr->arr[i] != NULL) {
            freeStringView(svArr->arr[i]);
            free(svArr->arr[i]);
        }
    }

    free(svArr->arr);

    svArr->arr = NULL;
    svArr->count = 0;
    svArr->capacity = 0;

    return;
}

void reallocStringViewArr(StringViewArr *svArr) {
    svArr->arr =
        realloc(svArr->arr, svArr->capacity * 2 * sizeof(StringView *));
    if (svArr->arr == NULL) {
        fprintf(stderr, "Error: Could not allocate String View Arr\n");
        exit(1);
    }

    svArr->capacity *= 2;
}
