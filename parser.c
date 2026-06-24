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
                Token y = popStack(&parser->outputStack);
                Token x = popStack(&parser->operatorStack);

                Token op = popStack(&parser->operatorStack);

                Token result = evaluate(x, y, op);

                appendToStack(&parser->outputStack, result);
            }
        }
    }
}

Token evaluate(Token x, Token y, Token op) {
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
            printf("unknown operator\n");
            exit(1);
        }
    }

    return result;
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
    if (stack->count == 0) exit(0);

    stack->count--;
    return stack->arr[stack->count];
}

void appendToStack(Stack *stack, Token token) {
    // TODO: check if token exceeds capacity
    stack->arr[stack->count] = token;
    stack->count++;
}
