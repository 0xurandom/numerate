#include "string_view_arr.h"

StringViewArr* newStringViewArr() {
    StringViewArr* svArr = malloc(sizeof(StringViewArr));
    svArr->arr = malloc(STRINGVIEWARR_CAP * sizeof(StringView));
}