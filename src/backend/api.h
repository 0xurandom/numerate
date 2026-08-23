#pragma once

#include "lexer.h"
#include "parser.h"

typedef struct {
    Lexer lexer;
    Parser parser;
} Calc;

Calc *init_calc();
char *eval_calc(Calc *calc, const char *input);
void calc_free_result(char *result);
void calcDestroy(Calc *calc);
void reset_calc(Calc *calc);
