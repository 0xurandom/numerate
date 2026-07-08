#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char *arr;
    size_t length;
    size_t capacity;
} StringView;

StringView newStringView(char *string, size_t length);
void setStringView(StringView *view, char *string, size_t length);
char *getCstring(StringView *view);
bool compareViews(StringView *view1, StringView *view2);
void freeStringView(StringView *view);
