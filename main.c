#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

#define OUTPUT_STACK_CAP 10
#define OPERATOR_STACK_CAP 10

#define INPUT_STRING_CAP 10

int main() {
    StringView stringView = {
        .string = malloc(INPUT_STRING_CAP * sizeof(char)),
        .cap = INPUT_STRING_CAP,
    };

    if (stringView.string == NULL) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    Lexer lexer;
    Parser parser;

    while (1 == 1) {
        evaluateExpression(&lexer, &parser, stringView);
    }

    return 0;
}

void evaluateExpression(Lexer *lexer, Parser *parser, StringView stringView) {
    printf("> ");
    scanf("%s", stringView.string);

    stringView.len = strlen(stringView.string);

    lexer->string = stringView.string;
    lexer->cursor = 0;
    lexer->length = stringView.len;

    parser->lexer = lexer;
    parser->cur = tokenise(lexer);

    Node *result = parse(parser, PREC_ASSIGNMENT);

    printf("%f\n", result->data.literal.value);
}
