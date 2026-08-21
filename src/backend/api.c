#include "api.h"

#include <stdlib.h>

#include "utils/function_utils.h"
#include "utils/hashmap_utils.h"
#include "variable_store.h"

Calc *initCalc() {
    Calc *calc = malloc(sizeof(Calc));

    HashMap varStore;
    initVarStore(&varStore);

    FuncArr funcArr;
    initFuncArr(&funcArr);

    Env env = {.varStore = &varStore, .funcArr = &funcArr, .parent = NULL};
    calc->parser = (Parser){.lexer = &calc->lexer, .env = &env};

    return calc;
}

void resetCalc(Calc *calc) {
    freeHashmap(calc->parser.env->varStore);
    HashMap varStore;
    initVarStore(&varStore);
    calc->parser.env->varStore = &varStore;
}
