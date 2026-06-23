#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

int main() {
    Parser parser;

    return 0;
}

double parse(Parser *parser) {
    switch (parser->token.kind) {
        case NUMBER: {
            appendToStack(&parser->outputStack, parser->token);
        }
        case PLUS:
        case MINUS:
        case ASTERISK:
        case SLASH: {
            // check if the token's operator has higher precedence
            // than the operator on top of the stack

            int tokenPrecedence = getPrecedence(parser->token.kind);
            int stackPrecedence =
                getPrecedence(getStackTop(&parser->operatorStack));

            if (tokenPrecedence > stackPrecedence) {
                appendToStack(&parser->operatorStack, parser->token);
            } else {
            }
        }
    }
}

double evaluate(double x, double y, TokenKind operator) {
    switch (operator) {
        case PLUS:
            return x + y;
        case MINUS:
            return x - y;
        case ASTERISK:
            return x * y;
        case SLASH:
            return x / y;

        default: {
            printf("unknown operator\n");
            exit(1);
        }
    }
}

TokenKind getStackTop(Stack *stack) {
    if (stack->count == 0)
        return -1;
    else
        return stack->arr[stack->count - 1].kind;
}

int getPrecedence(TokenKind kind) {
    switch (kind) {
        case ASTERISK:
        case SLASH:
            return 2;

        case PLUS:
        case MINUS:
            return 1;

        default:
            return 0;
    }
}

void popStack(Stack *stack) { stack->count--; }

void appendToStack(Stack *stack, Token token) {
    // TODO: check if token exceeds capacity
    stack->arr[stack->count] = token;
    stack->count++;
}
