#pragma once

#include "../parser.h"
#include "parser_utils.h"
#include "string_view_arr.h"
#include "string_view_utils.h"

#define FUNCARR_CAP 20

typedef struct {
    StringView name;
    StringViewArr params;
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

Number* evaluateFunction(Parser* parser, Node* funcCallNode);
Number* evaluateFunctionAt(Parser* parser, Token funcName, Number* x);
void initFuncArr(FuncArr* funcArr);
Func* newFunc(StringView* name, Node* val, int paramCount, StringView* param1,
              ...);
Func* searchFuncArr(const FuncArr* funcArr, StringView* funcName);
bool addToFuncArr(FuncArr* funcArr, Func* func);
void deleteFromFuncArr(FuncArr* funcArr, StringView* funcName);
void freeFunc(Func* func);
void reallocFuncArr(FuncArr* funcArr);