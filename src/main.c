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
    Parser parser = {
        .lexer = &lexer,
    };

    char testString1[] = "2*8";
    char testString2[] = "7 * 9";

    evaluateString(&lexer, &parser, testString1);
    evaluateString(&lexer, &parser, testString2);

    while (true) {
        evaluateInput(&lexer, &parser, stringView);
    }

    return 0;
}

void evaluateInput(Lexer *lexer, Parser *parser, Stack stringView) {
    printf("> ");
    // TODO: change scanf to fgets

    scanf("%99[^\n]%*c", stringView.arr);
    printf("'%s'\n", stringView.arr);

    evaluateString(lexer, parser, stringView.arr);
}

void evaluateString(Lexer *lexer, Parser *parser, char *str) {
    printf("\n\nevaluating: %s\n", str);
    lexer->string = str;
    lexer->cursor = 0;
    lexer->length = strlen(str);

    parser->cur = tokenise(lexer);

    Node *tree = parse(parser, PREC_ASSIGNMENT);

    Node *result = simplifyTree(tree);

    printf("= %.2f\n\n", result->data.literal.value);
}
