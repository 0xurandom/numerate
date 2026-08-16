#pragma once

#include "../lexer.h"
#include "hashmap_utils.h"
#include "string_view_arr.h"
#include "string_view_utils.h"

#define FUNCARR_CAP 20

typedef struct Node Node;
typedef struct Parser Parser;

typedef struct {
    StringView name;
    StringViewArr params;
    Node *val;
} Func;

typedef struct {
    Func **funcs;
    int count;
    int capacity;
} FuncArr;

typedef struct Env Env;

struct Env {
    HashMap *varStore;
    FuncArr *funcArr;
    Env *parent;
};

Number *evaluateFunction(Parser *parser, Node *funcCallNode);
Number *evaluateFunctionAt(Parser *parser, Token funcName, const Number *x);
void initFuncArr(FuncArr *funcArr);
Func *newFunc(const StringView *name, Node *val, int paramCount,
              StringView *param1, ...);
Func *newFuncWithArr(const StringView *name, Node *val, StringViewArr *params);
Func *searchFuncArr(const FuncArr *funcArr, StringView *funcName);
bool addToFuncArr(FuncArr *funcArr, Func *func);
void deleteFromFuncArr(FuncArr *funcArr, StringView *funcName);
void freeFunc(Func *func);
void reallocFuncArr(FuncArr *funcArr);
