#include "api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/function_utils.h"
#include "utils/hashmap_utils.h"
#include "utils/num_utils.h"
#include "utils/parser_utils.h"
#include "variable_store.h"

Calc *init_calc() {
    Calc *calc = malloc(sizeof(Calc));

    HashMap *varStore = malloc(sizeof(HashMap));
    initVarStore(varStore);

    FuncArr *funcArr = malloc(sizeof(FuncArr));
    initFuncArr(funcArr);

    Env *env = malloc(sizeof(Env));
    env->varStore = varStore;
    env->funcArr = funcArr;
    env->parent = NULL;

    calc->parser = (Parser){.lexer = &calc->lexer, .env = env};

    return calc;
}

char *eval_calc(Calc *calc, const char *input) {
    char *inputCopy = strdup(input);
    const Unit *unit = NULL;

    Number *result =
        evaluateString(&calc->lexer, &calc->parser, inputCopy, &unit);
    char *buffer = malloc(256 * sizeof(char));

    free(inputCopy);

    return buffer;
}

void calc_free_result(char *result) { free(result); }

void calcDestroy(Calc *calc) {
    freeHashmap(calc->parser.env->varStore);
    free(calc->parser.env->varStore);

    freeFuncArr(calc->parser.env->funcArr);
    free(calc->parser.env->funcArr);

    free(calc->parser.env);
    free(calc);
}

void reset_calc(Calc *calc) {
    freeHashmap(calc->parser.env->varStore);
    free(calc->parser.env->varStore);

    HashMap *varStore = malloc(sizeof(HashMap));
    initVarStore(varStore);
    calc->parser.env->varStore = varStore;
}
