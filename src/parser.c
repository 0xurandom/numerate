#include "parser.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ucontext.h>

#include "lexer.h"
#include "utils/math_utils.h"

Node* parse(Parser* parser, Precedence precedence) {
    nextToken(parser);

    // TODO: handle spaces for keyword funcs

    // TODO: spinoff helper funcs into
    // different file

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
        // subfactorial for literal and
        // not operator for bool
        case TOK_BANG: {
            Token op = parser->prev;
            Node* operand = parse(parser, PREC_UNARY);
            left = newPrefixNode(op, operand);
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
            left = newPrefixNode(op, operand);
            break;
        }

        case TOK_MINUS: {
            // unary minus
            Token op = parser->prev;
            Node* operand = parse(parser, PREC_UNARY);
            left = newPrefixNode(op, operand);

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
            case TOK_SLASH:

            case TOK_AND:
            case TOK_OR: {
                Node* right = parse(parser, getPrecedence(op.kind) + 1);

                left = newBinaryNode(op, left, right);

                break;
            }

            case TOK_LESS:
            case TOK_GREATER:
            case TOK_LESS_EQUALS:
            case TOK_GREATER_EQUALS: {
                Node* right = parse(parser, getPrecedence(op.kind) + 1);

                left = newBinaryNode(op, left, right);

                break;
            }

            // right associative tokens
            case TOK_EQUALS_EQUALS:
            case TOK_NOT_EQUALS: {
                // comparison operators

                Node* right = parse(parser, getPrecedence(op.kind));
                left = newBinaryNode(op, left, right);

                break;
            }

            case TOK_EQUALS: {
                // assignment operator

                Node* right = parse(parser, getPrecedence(op.kind));
                left = newBinaryNode(op, left, right);

                break;
            }

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
        case NODE_LITERAL:
        case NODE_BOOLEAN: {
            return node;
        }

        case NODE_VARIABLE: {
            // TODO: lookup variable value
            exit(1);
            break;
        }

        // unary -, trig, sgn, subfactorial !, not !
        case NODE_PREFIX: {
            node->unary.operand = simplifyTree(node->unary.operand);

            double num = node->unary.operand->literal.value;
            double result;
            switch (node->unary.op.kind) {
                case TOK_BANG: {
                    if (node->unary.operand->kind == NODE_LITERAL) {
                        result = subfactorial(num);
                    } else if (node->unary.operand->kind == NODE_BOOLEAN) {
                        result = (num == 1 ? 0 : 1);
                        Node* newNode = newBooleanNode(result);
                        freeNode(node);
                        return newNode;
                    } else {
                        fprintf(stderr,
                                "Invalid operand for prefix node with bang "
                                "operator\n");
                        exit(1);
                    }
                    break;
                }
                case TOK_MINUS: {
                    result = -num;
                    break;
                }
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
                    if (sin(num) == 0) {
                        fprintf(
                            stderr,
                            "Error: Cosec is undefined for multiples of pi\n");
                        exit(1);
                    }

                    result = 1 / sin(num);
                    break;
                }
                case TOK_SEC: {
                    if (cos(num) == 0) {
                        fprintf(stderr,
                                "Error: Sec is undefined for odd multiples of "
                                "pi/2\n");
                        exit(1);
                    }

                    result = 1 / cos(num);
                    break;
                }
                case TOK_COT: {
                    if (tan(num) == 0) {
                        fprintf(
                            stderr,
                            "Error: Cot is undefined for multiples of pi\n");
                        exit(1);
                    }

                    result = 1 / tan(num);
                    break;
                }

                case TOK_SGN: {
                    result = signum(num);
                    break;
                }
                default: {
                    fprintf(stderr, "Error: Unexpected prefix operator: %s\n",
                            lookupTokenKind(node->unary.op.kind));
                }
            }
            Node* newNode = newLiteralNode(result);
            freeNode(node);
            return newNode;
            break;
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

            if (isArithOp(node->binary.op.kind)) {
                // can handle literals and bools
                if (!(canBeNodeLiteral(node->binary.left) ||
                      canBeNodeLiteral(node->binary.right))) {
                    fprintf(stderr,
                            "Error: Invalid nodes for arith op: %s\tleft: "
                            "%s\tright: %s",
                            lookupTokenKind(node->binary.op.kind),
                            lookupNodeKind(node->binary.left->kind),
                            lookupNodeKind(node->binary.right->kind));
                    exit(1);
                }
            } else if (isComparisonOp(node->binary.op.kind)) {
                if (node->binary.left->kind != NODE_LITERAL ||
                    node->binary.left->kind != NODE_LITERAL) {
                    fprintf(stderr,
                            "Error: Invalid nodes for comparison op: %s\tleft: "
                            "%s\tright: %s",
                            lookupTokenKind(node->binary.op.kind),
                            lookupNodeKind(node->binary.left->kind),
                            lookupNodeKind(node->binary.right->kind));
                    exit(1);
                }
            } else {
                fprintf(stderr,
                        "Error: Binary operator is neither arith nor "
                        "comparison: %s",
                        lookupTokenKind(node->binary.op.kind));
                exit(1);
            }

            double left = node->binary.left->literal.value;
            double right = node->binary.right->literal.value;

            Node* newNode;
            double result;

            switch (node->binary.op.kind) {
                case TOK_AND: {
                    newNode = newBooleanNode(left && right);
                    break;
                }

                case TOK_OR: {
                    newNode = newBooleanNode(left || right);
                }

                case TOK_EQUALS_EQUALS: {
                    newNode = newBooleanNode(left == right);
                    break;
                }

                case TOK_NOT_EQUALS: {
                    newNode = newBooleanNode(left != right);
                    break;
                }

                case TOK_LESS: {
                    newNode = newBooleanNode(left < right);
                    break;
                }

                case TOK_GREATER: {
                    newNode = newBooleanNode(left > right);
                    break;
                }

                case TOK_LESS_EQUALS: {
                    newNode = newBooleanNode(left <= right);
                    break;
                }

                case TOK_GREATER_EQUALS: {
                    newNode = newBooleanNode(left >= right);
                    break;
                }

                case TOK_EQUALS: {
                    // assign variable
                    Node* variable = (node->binary.left->kind == NODE_VARIABLE
                                          ? node->binary.left
                                          : node->binary.right);
                    Node* literal = (node->binary.left->kind == NODE_LITERAL
                                         ? node->binary.left
                                         : node->binary.right);
                    // TODO: Make variable store
                    newNode = newLiteralNode(1);

                    break;
                }

                case TOK_PLUS: {
                    result = left + right;
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_MINUS: {
                    result = left - right;
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_ASTERISK: {
                    result = left * right;
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_SLASH: {
                    if (right == 0) {
                        fprintf(stderr, "Warning: cannot divide by zero\n");
                        // TODO: handle divisions by zero gracefully
                        exit(1);
                    }
                    result = left / right;
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_CARET: {
                    result = pow(left, right);
                    newNode = newLiteralNode(result);
                    break;
                }

                default: {
                    fprintf(stderr, "Unable to simplify token: %s\n",
                            lookupTokenKind(node->binary.op.kind));
                    exit(1);
                }
            }

            freeNode(node);
            return newNode;
        }
    }
}

Precedence getPrecedence(TokenKind kind) {
    switch (kind) {
        case TOK_AND:
            return PREC_AND;

        case TOK_OR:
            return PREC_OR;

        case TOK_EQUALS_EQUALS:
        case TOK_NOT_EQUALS:
            return PREC_EQUALILTY;

        case TOK_EQUALS:
            return PREC_ASSIGNMENT;

        case TOK_LESS:
        case TOK_GREATER:
        case TOK_LESS_EQUALS:
        case TOK_GREATER_EQUALS:
            return PREC_COMPARISON;

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

        case TOK_RPAREN:
        case TOK_END:
            return PREC_NONE;

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

Node* newBooleanNode(double num) {
    Node* node = malloc(sizeof(Node));

    node->kind = NODE_BOOLEAN;
    node->literal.value = num;

    return node;
}

Node* newVariableNode(Token name) {
    Node* node = malloc(sizeof(Node));

    node->kind = NODE_VARIABLE;
    node->variable.name = name;

    return node;
}

Node* newUnaryNode(Token op, Node* operand) {
    Node* node = malloc(sizeof(Node));

    node->kind = NODE_UNARY;
    node->unary.op = op;
    node->unary.operand = operand;

    return node;
}

Node* newPrefixNode(Token op, Node* operand) {
    Node* node = malloc(sizeof(Node));

    node->kind = NODE_PREFIX;
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
        case TOK_VAR:
            return "Var";
        case TOK_LESS:
            return "Less";
        case TOK_GREATER:
            return "Greater";
        case TOK_LESS_EQUALS:
            return "Less Equals";
        case TOK_GREATER_EQUALS:
            return "Greater Equals";
        case TOK_SIN:
            return "Sin";
        case TOK_COS:
            return "Cos";
        case TOK_TAN:
            return "Tan";
        case TOK_COSEC:
            return "Cosec";
        case TOK_SEC:
            return "Sec";
        case TOK_COT:
            return "Cot";
        case TOK_SGN:
            return "Sgn";
        case TOK_SPACE:
            return "Space";
        case TOK_AND:
            return "And";
        case TOK_OR:
            return "Or";
        case TOK_EQUALS:
            return "Equals";
        case TOK_EQUALS_EQUALS:
            return "Equals Equals";
        case TOK_NOT_EQUALS:
            return "Not Equals";
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

char* lookupNodeKind(NodeKind kind) {
    switch (kind) {
        case NODE_LITERAL:
            return "Literal";
        case NODE_BOOLEAN:
            return "Boolean";
        case NODE_BINARY:
            return "Binary";
        case NODE_UNARY:
            return "Unary";
        case NODE_VARIABLE:
            return "Variable";
        case NODE_PREFIX:
            return "Prefix";

        default: {
            fprintf(stderr,
                    "Warning: lookupNodeKind returning Unknown for unknown "
                    "NodeKind\n");
            return "Unknown NodeKind";
        }
    }
}

bool canBeNodeLiteral(Node* node) {
    return (node->kind == NODE_LITERAL || node->kind == NODE_BOOLEAN);
}

bool isArithOp(TokenKind kind) {
    switch (kind) {
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_ASTERISK:
        case TOK_SLASH:
        case TOK_CARET:
        case TOK_BANG:
        case TOK_SIN:
        case TOK_COS:
        case TOK_TAN:
        case TOK_COSEC:
        case TOK_SEC:
        case TOK_COT:
        case TOK_SGN:
            return true;

        default:
            return false;
    }
}

bool isComparisonOp(TokenKind kind) {
    switch (kind) {
        case TOK_AND:
        case TOK_OR:
        case TOK_EQUALS_EQUALS:
        case TOK_NOT_EQUALS:
        case TOK_LESS:
        case TOK_GREATER:
        case TOK_LESS_EQUALS:
        case TOK_GREATER_EQUALS:
            return true;
        default:
            return false;
    }
}
