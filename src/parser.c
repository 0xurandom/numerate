#include "parser.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h>

#include "lexer.h"
#include "math_utils.h"

Node* parse(Parser* parser, Precedence precedence) {
    nextToken(parser);

    // TODO: handle spaces
    // TODO: handle percent as modulo
    // and as * 0.01

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

            exit(1);
        }
    }

    // handle infixes and postfixes

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

            case TOK_BANG: {
                left = newUnaryNode(op, left);

                break;
            }

            default: {
                fprintf(stderr, "Unexpected postfix/infix token: %s\n",
                        lookupTokenKind(op.kind));
            }
        }
    }

    return left;
}

Node* simplifyTree(Node* node) {
    if (node == NULL) return NULL;

    switch (node->kind) {
        case NODE_LITERAL: {
            return node;
        }

        case NODE_UNARY: {
            node->data.unary.operand = simplifyTree(node->data.unary.operand);

            // handle unary minus
            if (node->data.unary.operand->kind == NODE_LITERAL &&
                node->data.unary.op.kind == TOK_MINUS) {
                Node* newNode = newLiteralNode(
                    -node->data.unary.operand->data.literal.value);
                freeNode(node);
                return newNode;
            }

            if (node->data.unary.operand->kind == NODE_LITERAL &&
                node->data.unary.op.kind == TOK_BANG) {
                Node* newNode = newLiteralNode(
                    factorial(node->data.unary.operand->data.literal.value));
                freeNode(node);
                return newNode;
            }

            fprintf(stderr, "Error: Could not simplify unary operand\n");
            exit(1);
            break;
        }

        case NODE_BINARY: {
            node->data.binary.left = simplifyTree(node->data.binary.left);
            node->data.binary.right = simplifyTree(node->data.binary.right);

            if (node->data.binary.left->kind == NODE_LITERAL &&
                node->data.binary.right->kind == NODE_LITERAL) {
                double left = node->data.binary.left->data.literal.value;
                double right = node->data.binary.right->data.literal.value;

                double result;

                switch (node->data.binary.op.kind) {
                    case TOK_PLUS: {
                        result = left + right;
                        break;
                    }

                    case TOK_MINUS: {
                        result = left - right;
                        break;
                    }

                    case TOK_ASTERISK: {
                        result = left * right;
                        break;
                    }

                    case TOK_SLASH: {
                        if (right == 0) {
                            fprintf(stderr, "Warning: cannot divide by zero\n");
                            // TODO: handle divisions by zero gracefully
                            exit(1);
                        }
                        result = left / right;
                        break;
                    }

                    case TOK_CARET: {
                        // TODO: caret simplification does not reach
                        // this case
                        result = pow(left, right);
                        break;
                    }

                    default: {
                        fprintf(stderr, "Unable to simplify token: %s\n",
                                lookupTokenKind(node->data.binary.op.kind));
                        exit(1);
                    }
                }

                Node* newNode = newLiteralNode(result);
                freeNode(node);
                return newNode;
            } else {
                fprintf(stderr, "Could not simplify subnodes for operator: %s",
                        lookupTokenKind(node->data.binary.op.kind));
                exit(1);
            }
        }
    }
}

Precedence getPrecedence(TokenKind kind) {
    switch (kind) {
        case TOK_BANG:
            return PREC_POSTFIX;

        case TOK_CARET:
            return PREC_EXPONENT;

        case TOK_ASTERISK:
        case TOK_SLASH:
            return PREC_FACTOR;

        case TOK_PLUS:
        case TOK_MINUS:
            return PREC_TERM;

        case TOK_END:
            return PREC_NONE;

        // TODO: add more for comparison operators
        default:
            fprintf(stderr, "Warning: using PREC_NONE for token kind: %s\n",
                    lookupTokenKind(kind));
            return PREC_NONE;
    }
}

void nextToken(Parser* parser) {
    parser->prev = parser->cur;

    parser->cur = tokenise(parser->lexer);
}

void freeNode(Node* node) {
    if (node == NULL) return;

    switch (node->kind) {
        case NODE_UNARY: {
            freeNode(node->data.unary.operand);
            break;
        }

        case NODE_BINARY: {
            freeNode(node->data.binary.left);
            freeNode(node->data.binary.right);
            break;
        }

        default: {
            break;
        }
    }

    free(node);
}

Node* newLiteralNode(double num) {
    Node* node = malloc(sizeof(Node));

    node->kind = NODE_LITERAL;
    node->data.literal.value = num;

    return node;
}

Node* newUnaryNode(Token op, Node* operand) {
    Node* node = malloc(sizeof(Node));

    node->kind = NODE_UNARY;
    node->data.unary.op = op;
    node->data.unary.operand = operand;

    return node;
}

Node* newBinaryNode(Token op, Node* left, Node* right) {
    Node* node = malloc(sizeof(Node));

    node->kind = NODE_BINARY;

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
        case TOK_BANG:
            return "Bang";
        case TOK_LPAREN:
            return "Lparen";
        case TOK_RPAREN:
            return "Rparen";
        case TOK_END:
            return "End";
        case TOK_UNKNOWN:
            return "Unknown";

        default: {
            fprintf(stderr,
                    "Warning: lookupTokenKind returning Unknown for unknown "
                    "TokenKind\n");
            return "Unknown Tokenkind";
        }
    }
}
