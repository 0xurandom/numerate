#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h>

#include "lexer.h"

Node* parse(Parser* parser, Precedence precedence) {
    nextToken(parser);

    Node* left = NULL;

    // handle prefixes

    switch (parser->prev.kind) {
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
            // parse tokens till the rparen
            left = parse(parser, PREC_ASSIGNMENT);

            // expect rparen
            if (parser->cur.kind == TOK_RPAREN) {
                nextToken(parser);
            } else {
                fprintf(stderr, "Expected ')', got: %s\n",
                        lookupTokenKind(parser->cur.kind));
                return NULL;
            }

            break;
        }

        default: {
            fprintf(stderr, "Unexpected prefix token: %s\n",
                    lookupTokenKind(parser->prev.kind));

            return NULL;
        }
    }

    // handle infixes

    // while the next token has a higher precedence
    while (precedence <= getPrecedence(parser->cur.kind)) {
        // go to next token so the infix operator is prev
        nextToken(parser);
        Token op = parser->prev;

        switch (op.kind) {
            // left associative tokens:
            // add 1 to recursively parse
            // with left associativity
            case TOK_PLUS:
            case TOK_MINUS:
            case TOK_ASTERISK:
            case TOK_SLASH: {
                Node* right = parse(parser, getPrecedence(op.kind) + 1);

                left = newBinaryNode(op, left, right);

                break;
            }

            // right associative tokens
            case TOK_CARET: {
                Node* right = parse(parser, getPrecedence(op.kind));

                left = newBinaryNode(op, left, right);

                break;
            }

            default: {
                fprintf(stderr, "Unexpected infix token: %s\n",
                        lookupTokenKind(op.kind));
            }
        }
    }

    return left;
}

Precedence getPrecedence(TokenKind kind) {
    switch (kind) {
        case TOK_ASTERISK:
        case TOK_SLASH:
            return PREC_FACTOR;

        case TOK_PLUS:
        case TOK_MINUS:
            return PREC_TERM;

        // TODO: add more for comparison operators
        default:
            return PREC_NONE;
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

char* lookupTokenKind(TokenKind kind) {
    switch (kind) {
        case TOK_NUMBER:
            return "Number";
        case TOK_PLUS:
            return "Plus";
        case TOK_MINUS:
            return "Minus";
        case TOK_ASTERISK:
            return "Asterisk";
        case TOK_SLASH:
            return "Slash";
        case TOK_CARET:
            return "Caret";
        case TOK_PERCENT:
            return "Percent";
        case TOK_LPAREN:
            return "Lparen";
        case TOK_RPAREN:
            return "Rparen";
        case TOK_END:
            return "End";
        case TOK_UNKNOWN:
            return "Unknown";
    }
}
