#ifndef MAIN_H
#define  MAIN_H

#include "lexer.h"
#include "parser.h"

typedef struct {
    char *string;
    size_t len;
    size_t cap;
} StringView;

void evaluateExpression(Lexer *lexer, Parser *parser, StringView stringView);

#endif
