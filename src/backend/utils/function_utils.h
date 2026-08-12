#pragma once

#include "parser_utils.h"
#include "string_view_utils.h"

#define FUNCARR_CAP 20

typedef struct {
    StringView name;
    StringView* params;
    int paramCount;
    Node* val;
} Func;

typedef struct Env Env;

struct Env {
    HashMap* varStore;
    Env* parent;
};

typedef struct {
    Func** funcs;
    int count;
    int capacity;
} FuncArr;
