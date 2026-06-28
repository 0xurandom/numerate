#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h>

#include "lexer.h"

Node* parse(Parser* parser, Precedence precedence) {
    // go to next token

    TokenKind prefixKind = parser->prev.kind;

    Node* left = NULL;

    // handle prefixes

    switch (prefixKind) {
        case TOK_NUMBER: {
            double value = parser->prev.num;
            left = newLiteralNode(value);

            break;
        }

        case TOK_MINUS: {
            // unary minus
            Token op = parser->prev;
            Node* operand = parse(parser, PREC_UNARY);
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

    // handle infixes

    // if the next token has a higher precedence
    while (precedence <= getPrecedence(parser->cur.kind)) {
    }
}

void nextToken(Parser* parser) {
    parser->prev = parser->cur;

    parser->cur = tokenise(parser->lexer);
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
