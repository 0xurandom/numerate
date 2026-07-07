#pragma once

#include <stddef.h>

typedef struct {
    char *arr;
    size_t length;
    size_t capacity;
} StringView;

StringView initStringView(char *string, size_t length);
void setStringView(StringView *view, char *string, size_t length);
char *getCstring(StringView *view);
void freeStringView(StringView *view);
