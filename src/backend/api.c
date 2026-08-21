#include "api.h"

#include <stdlib.h>
#include <string.h>

#include "utils/function_utils.h"
#include "utils/hashmap_utils.h"
#include "utils/num_utils.h"
#include "utils/parser_utils.h"
#include "variable_store.h"

Calc *init_calc() {
    Calc *calc = malloc(sizeof(Calc));

    HashMap varStore;
    initVarStore(&varStore);

    FuncArr funcArr;
    initFuncArr(&funcArr);

    Env env = {.varStore = &varStore, .funcArr = &funcArr, .parent = NULL};
    calc->parser = (Parser){.lexer = &calc->lexer, .env = &env};

    return calc;
}

char *eval_calc(Calc *calc, const char *input) {
    char *inputCopy = strdup(input);

    const Unit *unit = NULL;
    Number *result =
        evaluateString(&calc->lexer, &calc->parser, inputCopy, &unit);

    char *buffer = malloc(256 * sizeof(char));
    numToStringApi(buffer, sizeof(buffer), result, &unit);

    numFree(result);
    free(inputCopy);

    return buffer;
}

void calc_free_result(char *result) { free(result); }

void calcDestroy(Calc *calc) {
    freeHashmap(calc->parser.env->varStore);
    freeFuncArr(calc->parser.env->funcArr);
    free(calc);
}

void reset_calc(Calc *calc) {
    freeHashmap(calc->parser.env->varStore);
    HashMap varStore;
    initVarStore(&varStore);
    calc->parser.env->varStore = &varStore;
}
