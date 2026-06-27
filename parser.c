#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h>

#include "lexer.h"

Node* parse(Parser* parser, Precedence precedence) {
    // go to next token

    TokenKind prefixKind = parser->prev.kind;

    Node* left = NULL;

    switch (prefixKind) {
        case TOK_NUMBER: {
            double value = parser->prev.num;
            left = newLiteralNode(value);

            break;
        }

        case TOK_MINUS: {
            // unary minus
            Token op = parser->prev;
            Node *operand = parse(parser, PREC_UNARY);
            left = newUnaryNode(op, operand);

            break;
        }

        case TOK_LPAREN: {
            left = parse(parser, PREC_ASSIGNMENT);

            // consume token func
            break;
        }

        default: {
            fprintf(stderr, "Unexpected prefix token\n");
        }
    }
}

Node* newLiteralNode(double num) {
    Node* node = malloc(sizeof(Node));

    node->type = NODE_LITERAL;
    node->data.literal.value = num;

    return node;
}

Node* newUnaryNode(Token op, Node* operand) {
    Node* node = malloc(sizeof(Node));

    node->type = NODE_UNARY;
    node->data.unary.op = op;
    node->data.unary.operand = operand;

    return node;
}

Node* newBinaryNode(Token op, Node* left, Node* right) {
    Node* node = malloc(sizeof(Node));

    node->type = NODE_BINARY;

    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;

    return node;
}

// void evaluateStacks(Parser* parser) {
//     Token y = popStack(&parser->outputStack);
//     Token x = popStack(&parser->outputStack);
//
//     Token op = popStack(&parser->operatorStack);
//
//     Token result = {
//         .kind = NUMBER,
//     };
//
//     switch (op.kind) {
//         case PLUS: {
//             result.num = x.num + y.num;
//             break;
//         }
//         case MINUS: {
//             result.num = x.num - y.num;
//             break;
//         }
//         case ASTERISK: {
//             result.num = x.num * y.num;
//             break;
//         }
//         case SLASH: {
//             result.num = x.num / y.num;
//             break;
//         }
//
//         default: {
//             fprintf(stderr, "Unknown TokenKind used as operator\n");
//             exit(1);
//         }
//     };
//
//     appendToStack(&parser->outputStack, result);
//     return;
// }
//
// TokenKind getStackTop(Stack* stack) {
//     if (stack->count == 0)
//         return -1;
//     else
//         return stack->arr[stack->count - 1].kind;
// }
//
// int getPrecedence(TokenKind kind) {
//     switch (kind) {
//         case ASTERISK:
//         case SLASH:
//             return 20;
//
//         case PLUS:
//         case MINUS:
//             return 10;
//
//         case LPAREN:
//             return 30;
//         default:
//             return 0;
//     }
// }
//
// Token popStack(Stack* stack) {
//     if (stack->count == 0) {
//         fprintf(stderr, "No tokens left in stack to pop\n");
//         exit(1);
//     }
//
//     stack->count--;
//     return stack->arr[stack->count];
// }
//
// void appendToStack(Stack* stack, Token token) {
//     if (stack->count + 1 == stack->capacity) {
//         fprintf(stderr, "Too many tokens in stack to append\n");
//         exit(1);
//     }
//
//     stack->arr[stack->count] = token;
//     stack->count++;
// }
//
// char* lookupTokenKind(TokenKind kind) {
//     switch (kind) {
//         case NUMBER:
//             return "number";
//         case PLUS:
//             return "+";
//         case MINUS:
//             return "-";
//         case ASTERISK:
//             return "*";
//         case SLASH:
//             return "/";
//         case CARET:
//             return "^";
//         case PERCENT:
//             return "%";
//         case LPAREN:
//             return "(";
//         case RPAREN:
//             return ")";
//     }
// }
