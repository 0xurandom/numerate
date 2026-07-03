#include "parser.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ucontext.h>

#include "lexer.h"
#include "math_utils.h"

Node* parse(Parser* parser, Precedence precedence) {
    nextToken(parser);

    // TODO: handle spaces for keyword funcs

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

        case TOK_SIN:
        case TOK_COS:
        case TOK_TAN:
        case TOK_COSEC:
        case TOK_SEC:
        case TOK_COT:

        case TOK_SGN: {
            Token op = parser->prev;
            Node* operand = parse(parser, PREC_FUNC);
            left = newUnaryNode(op, operand);
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
            node->unary.operand = simplifyTree(node->unary.operand);

            // TODO: handle error gracefully
            if (node->unary.operand->kind != NODE_LITERAL) {
                fprintf(stderr, "Error: Could not simplify unary operand\n");
                exit(1);
            }

            double num = node->unary.operand->literal.value;
            double result;
            switch (node->unary.op.kind) {
                case TOK_SIN: {
                    result = sin(num);
                    break;
                }
                case TOK_COS: {
                    result = cos(num);
                    break;
                }
                case TOK_TAN: {
                    result = tan(num);
                    break;
                }

                // TODO: handle division by zero case
                case TOK_COSEC: {
                    result = 1 / sin(num);
                    break;
                }
                case TOK_SEC: {
                    result = 1 / cos(num);
                    break;
                }
                case TOK_COT: {
                    result = 1 / tan(num);
                    break;
                }

                case TOK_SGN: {
                    result = signum(num);
                    break;
                }

                case TOK_MINUS: {
                    result = -num;
                    break;
                }

                case TOK_BANG: {
                    result = factorial(num);
                    break;
                }

                default: {
                    fprintf(stderr, "Error: Unexpected unary operator: %s\n",
                            lookupTokenKind(node->unary.op.kind));
                }
            }

            Node* newNode = newLiteralNode(result);
            freeNode(node);
            return newNode;

            break;
        }

        case NODE_BINARY: {
            node->binary.left = simplifyTree(node->binary.left);
            node->binary.right = simplifyTree(node->binary.right);

            if (node->binary.left->kind == NODE_LITERAL &&
                node->binary.right->kind == NODE_LITERAL) {
                double left = node->binary.left->literal.value;
                double right = node->binary.right->literal.value;

                double result;

                switch (node->binary.op.kind) {
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
                                lookupTokenKind(node->binary.op.kind));
                        exit(1);
                    }
                }

                Node* newNode = newLiteralNode(result);
                freeNode(node);
                return newNode;
            } else {
                fprintf(stderr, "Could not simplify subnodes for operator: %s",
                        lookupTokenKind(node->binary.op.kind));
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

double evaluateString(Lexer* lexer, Parser* parser, char* str) {
    printf("\n\nevaluating: %s\n", str);
    lexer->string = str;
    lexer->cursor = 0;
    lexer->length = strlen(str);

    parser->cur = tokenise(lexer);

    Node* tree = parse(parser, PREC_ASSIGNMENT);
    Node* result = simplifyTree(tree);

    double result_val = result->literal.value;
    freeNode(result);

    return result_val;
}

void nextToken(Parser* parser) {
    parser->prev = parser->cur;

    parser->cur = tokenise(parser->lexer);
}

void freeNode(Node* node) {
    if (node == NULL) return;

    switch (node->kind) {
        case NODE_UNARY: {
            freeNode(node->unary.operand);
            break;
        }

        case NODE_BINARY: {
            freeNode(node->binary.left);
            freeNode(node->binary.right);
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
    node->literal.value = num;

    return node;
}

Node* newUnaryNode(Token op, Node* operand) {
    Node* node = malloc(sizeof(Node));

    node->kind = NODE_UNARY;
    node->unary.op = op;
    node->unary.operand = operand;

    return node;
}

Node* newBinaryNode(Token op, Node* left, Node* right) {
    Node* node = malloc(sizeof(Node));

    node->kind = NODE_BINARY;

    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;

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
