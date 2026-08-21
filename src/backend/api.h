#pragma once

#include "lexer.h"
#include "parser.h"

typedef struct {
    Lexer lexer;
    Parser parser;
} Calc;
