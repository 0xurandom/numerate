#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char* arr;
    size_t length;
    size_t capacity;
} StringView;

void initStringView(StringView* view);
void initStringViewWithString(StringView* view, const char* string,
                              size_t stringLength);
StringView* newStringView(const char* string, size_t length);
void setStringView(StringView* view, const char* string, size_t length);
void copyStringView(StringView* dest, const StringView* src);
void appendToStringView(StringView* view, char c);
void printStringView(const StringView* view);
char* getCstring(const StringView* view);
bool compareViews(const StringView* view1, const StringView* view2);
void reallocStringView(StringView* view);
void freeStringView(StringView* view);
