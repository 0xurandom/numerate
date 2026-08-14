#include "function_utils.h"

#include <stdarg.h>
#include <stdio.h>

Number* evaluateFunction(Parser* parser, Node* funcCallNode) {
    Func* func = searchFuncArr(parser->env->varStore, func);

    if (funcCallNode->funcCall.argCount != func->params.count) {
        const char error[] = "Function call has invalid number of arguments";
        Number* result = numNew(NUM_ERROR);
        numSetError(result, error, strlen(error));
        return result;
    }

    for (int i = 0; i < funcCallNode->funcCall.argCount; i++) {
        Node* node = funcCallNode->funcCall.args[i];
        Node* evaluatedNode = simplifyTree(parser, node);

        if (!canBeNodeLiteral(evaluatedNode)) {
            const char error[] = "Unable to evaluate function arguments";
            Number* result = numNew(NUM_ERROR);
            numSetError(result, error, strlen(error));
            // TODO: free this and prev nodes
            return result;
        }

        funcCallNode->funcCall.args[i] = evaluatedNode;

        if (node != evaluatedNode) freeNode(node);
    }

    Env localEnv = {.parent = parser->env};
    initVarStore(localEnv.varStore);

    for (int i = 0; i < func->params.count; i++) {
        insertVar(localEnv.varStore, func->params.arr[i],
                  &funcCallNode->funcCall.args[i]->literal.value);
    }
}

void initFuncArr(FuncArr* funcArr) {
    funcArr->funcs = malloc(FUNCARR_CAP * sizeof(Func));
    funcArr->capacity = FUNCARR_CAP;
    funcArr->count = 0;

    for (int i = 0; i < FUNCARR_CAP; i++) {
        funcArr->funcs[i] = NULL;
    }

    return;
}

Func* newFunc(StringView* name, Node* val, int paramCount, StringView* param1,
              ...) {
    Func* func = malloc(sizeof(Func));

    if (func == NULL) {
        fprintf(stderr, "Error: Could not allocate Func\n");
        exit(1);
    }

    copyStringView(&func->name, name);
    initStringViewArr(&func->params, paramCount);

    va_list args;
    va_start(args, param1);
    for (int i = 0; i < paramCount; i++) {
        StringView* param = va_arg(args, StringView*);
        addStringToStringViewArr(&func->params, param);
    }
    va_end(args);

    return func;
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

void deleteFromFuncArr(FuncArr* funcArr, StringView* funcName) {
    int i;

    for (i = 0; i < funcArr->count; i++) {
        if (compareViews(funcName, &funcArr->funcs[i]->name)) {
            freeFunc(&funcArr->funcs[i]);
            funcArr->funcs[i] = funcArr->funcs[funcArr->count - 1];
            funcArr->count--;
        }
    }

    return;
}

void freeFunc(Func* func) {
    freeNode(func->val);

    free(func);
    func = NULL;

    return;
}

void reallocFuncArr(FuncArr* funcArr) {
    funcArr->funcs = realloc(funcArr->funcs, 2 * funcArr->capacity);
    funcArr->capacity *= 2;

    return;
}