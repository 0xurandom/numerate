#include "parser.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h>

#include "lexer.h"
#include "utils/hashmap_utils.h"
#include "utils/math_utils.h"
#include "utils/num_utils.h"
#include "utils/parser_utils.h"
#include "utils/string_view_utils.h"

Node* parse(Parser* parser, Precedence precedence) {
    nextToken(parser);

    // TODO: handle spaces for keyword funcs

    // TODO: handle percent as modulo
    // and as * 0.01

    Node* left = NULL;

    // handle prefixes

    switch (parser->prev.kind) {
        case TOK_NUMBER: {
            left = newLiteralNode(&parser->prev.num);

            break;
        }

        case TOK_VAR: {
            // TODO: change var node names from token
            // to ident
            left = newVarNode(parser->prev);

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

        case TOK_SGN:
        case TOK_TWOS:
        case TOK_ABS:

        case TOK_SQRT:
        case TOK_CBRT:
        case TOK_EXP:

        case TOK_LN:
        case TOK_LOG: {
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
            case TOK_EQUALS: {
                if (left->kind != NODE_VAR) {
                    // TODO: maybe implement comparison here?
                    fprintf(stderr, "Error: Cannot assign to a non var\n");
                    exit(1);
                }

                Node* value = parse(parser, getPrecedence(op.kind));
                left = newAssignmentNode(left->var.name, value);
                break;
            }

            case TOK_EQUALS_EQUALS:
            case TOK_NOT_EQUALS: {
                // comparison operators

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

Node* simplifyTree(Parser* parser, Node* node) {
    if (node == NULL) return NULL;

    switch (node->kind) {
        case NODE_LITERAL:
        case NODE_BOOLEAN: {
            return node;
        }

        case NODE_VAR: {
            Number *num = numNew(NUM_REAL)
            double result;

            if (lookupVar(&parser->varStore, &node->assignment.name.ident,
                          &result) == 0) {
                Node* newNode = newLiteralNode(result);
                return newNode;
            } else {
                fprintf(stderr, "Error: Invalid variable referenced: %s\n",
                        getCstring(&node->var.name.ident));
                exit(1);
            }
        }

        // TODO: check if this is necessary
        case NODE_ASSIGNMENT: {
            node->assignment.value =
                simplifyTree(parser, node->assignment.value);

            if (node->assignment.value->kind != NODE_LITERAL) {
                fprintf(stderr,
                        "Error: Could not simplify the value of var: %s\n",
                        getCstring(&node->assignment.name.ident));
                exit(1);
            }

            double value = node->assignment.value->literal.value;

            insertVar(&parser->varStore, &node->assignment.name.ident, value);

            Node* newNode = newLiteralNode(value);
            freeNode(node);
            return newNode;

            break;
        }

        // unary -, trig, sgn, subfactorial !, not !
        case NODE_PREFIX: {
            node->unary.operand = simplifyTree(parser, node->unary.operand);

            // double num = node->unary.operand->literal.value;
            
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

                case TOK_TWOS: {
                    result = twosComplement(num);
                    break;
                }

                // TODO: does not work correctly
                // when used without ()
                case TOK_ABS: {
                    result = fabs(num);
                    break;
                }

                case TOK_SQRT: {
                    // TODO: assrt num isnt negative
                    result = sqrt(num);
                    break;
                }

                case TOK_CBRT: {
                    result = cbrt(num);
                    break;
                }

                case TOK_EXP: {
                    double e;
                    StringView* eVar = newStringView("e", 1);
                    lookupVar(&parser->varStore, eVar, &e);
                    freeStringView(eVar);

                    result = pow(e, num);
                    break;
                }

                case TOK_LN: {
                    result = log(num);
                    break;
                }

                case TOK_LOG: {
                    result = log10(num);
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
            node->unary.operand = simplifyTree(parser, node->unary.operand);

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
            node->binary.left = simplifyTree(parser, node->binary.left);
            node->binary.right = simplifyTree(parser, node->binary.right);

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
                    break;
                }

                case TOK_BITWISE_AND: {
                    // TODO: bitwise operators cannot be used with doubles
                    // newNode = newLiteralNode(left )
                    exit(1);
                    break;
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
        // TODO: check trig func precedence
        case TOK_AND:
            return PREC_AND;

        case TOK_OR:
            return PREC_OR;

        case TOK_BITWISE_AND:
            return PREC_BITWISE_AND;

        case TOK_BITWISE_OR:
            return PREC_BITWISE_OR;

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

        case TOK_SIN:
        case TOK_COS:
        case TOK_TAN:
        case TOK_COSEC:
        case TOK_SEC:
        case TOK_COT:
        case TOK_SGN:
        case TOK_ABS:
        case TOK_SQRT:
            return PREC_UNARY;

        default:
            fprintf(stderr, "Warning: using PREC_NONE for token kind: %s\n",
                    lookupTokenKind(kind));
            return PREC_NONE;
    }
}
