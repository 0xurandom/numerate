#include "string_view_utils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_STRING_CAPACITY 10

// init a string view and set it to a string
StringView newStringView(char *string, size_t length) {
    StringView view;

    if (length > DEFAULT_STRING_CAPACITY) {
        view.arr = malloc(length * sizeof(char));
        view.capacity = length;
    } else {
        view.arr = malloc(DEFAULT_STRING_CAPACITY * sizeof(char));
        view.capacity = DEFAULT_STRING_CAPACITY;
    }
    view.length = length;

    return view;
}

// change the string of an existing string view
void setStringView(StringView *view, char *string, size_t length) {
    if (length > view->capacity) {
        view->arr = realloc(view->arr, length * sizeof(char));
        view->capacity = length;
    } else {
        view->arr = string;
    }

    view->length = length;
}

// get a c string from the string view
// this string must be freed manually
char *getCstring(StringView *view) {
    char *cstring = malloc((view->length + 1) * sizeof(char));
    memcpy(cstring, view->arr, view->length);
    cstring[view->length] = '\0';

    return cstring;
}

bool compareViews(StringView *view1, StringView *view2) {
    if (view1->length != view2->length) return false;

    if (strncmp(view1->arr, view2->arr, view1->length) == 0)
        return true;
    else
        return false;
}

void freeStringView(StringView *view) {
    free(view->arr);
    view->arr = NULL;
    view->capacity = 0;
    view->length = 0;
}
