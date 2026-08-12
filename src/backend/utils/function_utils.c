#include "function_utils.h"

Number* evaluateFunction(Parser* parser, StringView* func) {
    Env localEnv = {.parent = parser->env};
    initVarStore(localEnv.varStore);

    // for (int i = 0; i <)
}

// Func* newFunc(const char* name, , Node * val)

void initFuncArr(FuncArr* funcArr) {
    funcArr->funcs = malloc(FUNCARR_CAP * sizeof(Func));
    funcArr->capacity = FUNCARR_CAP;
    funcArr->count = 0;

    for (int i = 0; i < FUNCARR_CAP; i++) {
        funcArr->funcs[i] = NULL;
    }
}

Func* searchFuncArr(const FuncArr* funcArr, StringView* funcName) {
    for (int i = 0; i < funcArr->count; i++) {
        if (compareViews(funcName, &funcArr->funcs[i]->name))
            return &funcArr->funcs[i];
    }

    return NULL;
}

bool addToFuncArr(FuncArr* funcArr, Func* func) {
    if (searchFuncArr(funcArr, &func->name) != NULL) return false;

    if (funcArr->count + 1 > funcArr->capacity) resizeFuncArr(funcArr);

    funcArr->funcs[funcArr->count] = func;
    return true;
}

void deleteFromFuncArr(FuncArr* funcArr) {}

void reallocFuncArr(FuncArr* funcArr) {
    funcArr->funcs = realloc(funcArr->funcs, 2 * funcArr->capacity);
    funcArr->capacity *= 2;
}