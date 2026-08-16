#pragma once

#include "../lexer.h"

void lexString(char* string);
void checkAllocation(void* ptr);
double parseDec(Lexer* lexer);
double parseHex(Lexer* lexer);
double parseBin(Lexer* lexer);
void parseNum(Lexer* lexer, Token* token);
int hexToInt(char c);
char peekNext(Lexer* lexer);
Token newToken(TokenKind kind);
