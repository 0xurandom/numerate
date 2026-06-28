
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

    return 0;
}
