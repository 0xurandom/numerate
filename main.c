#include "main.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

#define INPUT_STRING_CAP 10

int main() {
    Stack stringView = {
        .arr = malloc(INPUT_STRING_CAP * sizeof(char)),
        .capacity = INPUT_STRING_CAP,
    };

    checkAllocation(stringView.arr);

    Lexer lexer;
    Parser parser;

    while (true) {
        evaluateInput(&lexer, &parser, stringView);
    }

    return 0;
}

void evaluateInput(Lexer *lexer, Parser *parser, Stack stringView) {
    printf("> ");
    // TODO: change scanf to fgets
    scanf("%s", stringView.arr);

    stringView.count = strlen(stringView.arr);

    lexer->string = stringView.arr;
    lexer->cursor = 0;
    lexer->length = stringView.count;

    parser->lexer = lexer;
    parser->cur = tokenise(lexer);

    Node *tree = parse(parser, PREC_ASSIGNMENT);

    Node *result = simplifyTree(tree);

    printf("%f\n", result->data.literal.value);
}
