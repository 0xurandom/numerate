#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char *arr;
    size_t length;
    size_t capacity;
} StringView;

void initStringView(StringView *view);
void initStringViewWithString(StringView *view, const char *string,
                              size_t stringLength);
StringView *newStringView(char *string, size_t length);
void setStringView(StringView *view, char *string, size_t length);
void appendToStringView(StringView *view, char c);
char *getCstring(StringView *view);
bool compareViews(StringView *view1, StringView *view2);
void reallocStringView(StringView *view);
void freeStringView(StringView *view);
