#pragma once

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer.h"
#include "parser_utils.h"

void lexString(char* string);
void checkAllocation(void* ptr);
double parseDec(Lexer* lexer);
double parseHex(Lexer* lexer);
double parseBin(Lexer* lexer);
Number* parseNum(Lexer* lexer);
int hexToInt(char c);
char peekNext(Lexer* lexer);
