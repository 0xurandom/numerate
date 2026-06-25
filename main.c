#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

#define OUTPUT_STACK_CAP 10
#define OPERATOR_STACK_CAP 10

int main() {
    char testString[] = "1+2-3*4/5";

    Lexer lexer = {
        .string = testString,
        .cursor = 0,
        .length = strlen(testString),
    };

    Parser parser = {
        .operatorStack =
            {
                .arr = malloc(OPERATOR_STACK_CAP * sizeof(Token)),
                .count = 0,
                .capacity = OPERATOR_STACK_CAP,
            },

        .outputStack =
            {
                .arr = malloc(OUTPUT_STACK_CAP * sizeof(Token)),
                .count = 0,
                .capacity = OUTPUT_STACK_CAP,
            },
    };

    // TODO: here tokenise is run twice?

    while (lexer.cursor != lexer.length) {
        Token token = tokenise(&lexer);

        parser.token = token;
        printf("parsing token of kind %d, val: %f\n", token.kind, token.num);

        parse(&parser);
    }

    while (parser.outputStack.count != 1) {
        evaluateStacks(&parser);
    }
    printf("hi");
    printf("result: %f", parser.outputStack.arr[0].num);

    return 0;
}
