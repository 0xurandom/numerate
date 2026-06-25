#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h>

#include "lexer.h"

void parse(Parser *parser) {
    switch (parser->token.kind) {
        case NUMBER: {
            appendToStack(&parser->outputStack, parser->token);
            break;
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
                evaluateStacks(parser);
                appendToStack(&parser->operatorStack, parser->token);
            }
        }
    }
}

void evaluateStacks(Parser *parser) {
    Token y = popStack(&parser->outputStack);
    Token x = popStack(&parser->outputStack);

    Token op = popStack(&parser->operatorStack);

    Token result = {
        .kind = NUMBER,
    };

    switch (op.kind) {
        case PLUS: {
            result.num = x.num + y.num;
            break;
        }
        case MINUS: {
            result.num = x.num - y.num;
            break;
        }
        case ASTERISK: {
            result.num = x.num * y.num;
            break;
        }
        case SLASH: {
            result.num = x.num / y.num;
            break;
        }

        default: {
            fprintf(stderr, "Unknown TokenKind used as operator\n");
            exit(1);
        }
    };

    appendToStack(&parser->outputStack, result);
    return;
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

Token popStack(Stack *stack) {
    if (stack->count == 0) {
        fprintf(stderr, "No tokens left in stack to pop\n");
        exit(1);
    }

    stack->count--;
    return stack->arr[stack->count];
}

void appendToStack(Stack *stack, Token token) {
    if (stack->count + 1 == stack->capacity) {
        fprintf(stderr, "Too many tokens in stack to append\n");
        exit(1);
    }

    stack->arr[stack->count] = token;
    stack->count++;
}

char *lookupTokenKind(TokenKind kind) {
    switch (kind) {
        case NUMBER:
            return "number";
        case PLUS:
            return "+";
        case MINUS:
            return "-";
        case ASTERISK:
            return "*";
        case SLASH:
            return "/";
    }
}
