#ifndef MAIN_H
#define MAIN_H

#include "lexer.h"
#include "parser.h"

void evaluateInput(Lexer *lexer, Parser *parser, Stack stringView);
double evaluateString(Lexer *lexer, Parser *parser, char *str);

#endif
