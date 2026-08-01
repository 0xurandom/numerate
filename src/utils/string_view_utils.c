#include "string_view_utils.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "num_utils.h"

#define DEFAULT_STRING_CAPACITY 10

// TODO: add malloc null checks
// and const parameters

void initStringView(StringView *view) {
    view->arr = malloc(DEFAULT_STRING_CAPACITY * sizeof(char));
    view->length = 0;
    view->capacity = DEFAULT_STRING_CAPACITY;
}

void initStringViewWithString(StringView *view, const char *string,
                              size_t stringLength) {
    if (stringLength > DEFAULT_STRING_CAPACITY) {
        view->arr = malloc(stringLength * sizeof(char));
        view->capacity = stringLength;
        view->length = stringLength;

    } else {
        view->arr = malloc(DEFAULT_STRING_CAPACITY * sizeof(char));
        view->capacity = DEFAULT_STRING_CAPACITY;
        view->length = stringLength;
    }

    if (view->arr == NULL) {
        fprintf(stderr, "Error: Could not allocate string view\n");
        exit(1);
    }

    memcpy(view->arr, string, stringLength);

    return;
}

// copies length number of bytes from string
// and allocates a new string view
StringView *newStringView(char *string, size_t length) {
    StringView *view = malloc(sizeof(StringView));

    if (length > DEFAULT_STRING_CAPACITY) {
        view->arr = malloc(length * sizeof(char));
        view->capacity = length;
    } else {
        view->arr = malloc(DEFAULT_STRING_CAPACITY * sizeof(char));
        view->capacity = DEFAULT_STRING_CAPACITY;
    }
    memcpy(view->arr, string, length);

    view->length = length;

    return view;
}

// change the string of an existing string view
void setStringView(StringView *view, const char *string, size_t length) {
    if (length > view->capacity) {
        view->arr = realloc(view->arr, length * sizeof(char));
        view->capacity = length;
    }

    memcpy(view->arr, string, length);

    view->length = length;
}

void copyStringView(StringView *dest, const StringView *src) {
    dest->capacity = src->capacity;
    dest->length = src->length;

    dest->arr = malloc(dest->capacity * sizeof(char));
    memcpy(dest->arr, src->arr, dest->capacity);

    return;
}

void appendToStringView(StringView *view, char c) {
    if (view->length + 1 > view->capacity) reallocStringView(view);

    view->arr[view->length] = c;
}

// get a c string from the string view
// this string must be freed manually
char *getCstring(StringView *view) {
    char *cstring = malloc((view->length + 1) * sizeof(char));
    memcpy(cstring, view->arr, view->length);
    cstring[view->length] = '\0';

    return cstring;
}

bool compareViews(const StringView *view1, const StringView *view2) {
    if (view1->length != view2->length) return false;

    if (strncmp(view1->arr, view2->arr, view1->length) == 0)
        return true;
    else
        return false;
}

// double the capacity of StringView
void reallocStringView(StringView *view) {
    view->arr = realloc(view->arr, view->capacity * 2);
    view->capacity *= 2;
}

void freeStringView(StringView *view) {
    free(view->arr);
    view->arr = NULL;
    view->capacity = 0;
    view->length = 0;
}
