#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

#define INPUT_STRING_CAP 10

void evaluateInput(Lexer *lexer, Parser *parser, Stack stringView);

int main() {
    Stack stringView = {
        .arr = malloc(INPUT_STRING_CAP * sizeof(char)),
        .capacity = INPUT_STRING_CAP,
    };

    checkAllocation(stringView.arr);

    Lexer lexer;
    Parser parser = {
        .lexer = &lexer,
    };

    while (true) {
        evaluateInput(&lexer, &parser, stringView);
    }

    return 0;
}

void evaluateInput(Lexer *lexer, Parser *parser, Stack stringView) {
    printf("> ");
    // TODO: change scanf to fgets

    scanf("%99[^\n]%*c", stringView.arr);
    lexString(stringView.arr);

    // printf("= %.2f\n\n", evaluateString(lexer, parser, stringView.arr));
}
