#ifndef PARSER_H
#define  PARSER_H

#import "lexer.h"

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


Token evaluate(Token x, Token y, Token op);
void appendToStack(Stack *stack, Token token);
int getPrecedence(TokenKind kind);
TokenKind getStackTop(Stack *stack);
Token popStack(Stack *stack);

#endif
