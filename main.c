#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

#define OUTPUT_STACK_CAP 10
#define OPERATOR_STACK_CAP 10

int main() {
    char testString[] = "1+2-3*4/5";

    printf("calculating: %s\n\n", testString);

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

    while (lexer.cursor != lexer.length) {
        Token token = tokenise(&lexer);

        parser.token = token;
        printf("token kind %s\t", lookupTokenKind(token.kind));

        if (token.kind == NUMBER)
            printf("val: %f\n", token.num);
        else
            printf("\n");

        parse(&parser);
    }

    while (parser.outputStack.count != 1) {
        evaluateStacks(&parser);
    }
    printf("\n\nresult: %f\n\n", parser.outputStack.arr[0].num);

    return 0;
}
