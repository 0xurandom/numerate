#include "string_view_arr.h"

#include <stdio.h>
#include <string.h>

StringViewArr* newStringViewArr() {
    StringViewArr* svArr = malloc(sizeof(StringViewArr));
    svArr->arr = malloc(STRINGVIEWARR_CAP * sizeof(StringView));
    svArr->capacity = STRINGVIEWARR_CAP;
    svArr->count = 0;
}

// pass 0 as stringLength if unknown
void addStringToStringView(StringViewArr* svArr, const char* string,
                           size_t passedStrLen) {
    if (svArr->count + 1 > svArr->capacity) reallocStringViewArr(svArr);

    int StrLen = (passedStrLen == 0) ? strlen(string) : passedStrLen;
    // change to stringView**
    memcpy(&svArr->arr[svArr->count], string, StrLen * sizeof(char));
    svArr->arr[svArr->count].length = StrLen;
}

void reallocStringViewArr(StringView* svArr) {
    svArr->arr = realloc(svArr->arr, svArr->capacity * 2);
    if (svArr->arr == NULL) {
        fprintf(stderr, "Error: Could not allocate String View Arr\n");
        exit(1);
    }

    svArr->capacity *= 2;
}