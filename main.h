#ifndef MAIN_H
#define  MAIN_H

#include "lexer.h"
#include "parser.h"

void evaluateExpression(Lexer *lexer, Parser *parser, Stack stringView);

#endif
