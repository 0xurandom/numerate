#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "utils/function_utils.h"
#include "utils/hashmap_utils.h"
#include "utils/lexer_utils.h"
#include "utils/num_utils.h"
#include "utils/parser_utils.h"
#include "variable_store.h"

#define INPUT_STRING_CAP 10

void evaluateInput(Lexer* lexer, Parser* parser, StringView stringView);

int main() {
    StringView stringView = {
        .arr = malloc(INPUT_STRING_CAP * sizeof(char)),
        .capacity = INPUT_STRING_CAP,
    };

    checkAllocation(stringView.arr);

    Lexer lexer;
    HashMap varStore;
    initVarStore(&varStore);
    Env env = {.parent = NULL, .varStore = &varStore};
    Parser parser = {.lexer = &lexer, .env = &env};

    while (true) {
        evaluateInput(&lexer, &parser, stringView);
    }

    return 0;
}

void evaluateInput(Lexer* lexer, Parser* parser, StringView stringView) {
    printf("> ");
    // TODO: change scanf to fgets

    scanf("%99[^\n]%*c", stringView.arr);
    lexString(stringView.arr);
    Number* result = evaluateString(lexer, parser, stringView.arr);
    printf("Result: ");
    numPrint(result);
    numFree(result);
}
