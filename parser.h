#ifndef PARSER_H
#define  PARSER_H

#include "lexer.h"

typedef struct {
    Token *arr;
    size_t count;
    size_t capacity;
} Stack;

typedef struct {
    Stack operatorStack;
    Stack outputStack;
    Token token;
} Parser;

void parse(Parser *parser);
void evaluateStacks(Parser *parser);
TokenKind getStackTop(Stack *stack);
int getPrecedence(TokenKind kind);
Token popStack(Stack *stack);
void appendToStack(Stack *stack, Token token);
char *lookupTokenKind(TokenKind kind);

#endif
