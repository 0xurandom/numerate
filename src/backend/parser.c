#include "parser.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ucontext.h>

#include "lexer.h"
#include "utils/hashmap_utils.h"
#include "utils/num_ops.h"
#include "utils/num_utils.h"
#include "utils/parser_utils.h"
#include "utils/string_view_utils.h"

Node *parse(Parser *parser, Precedence precedence) {
    nextToken(parser);

    // TODO: handle spaces for keyword funcs

    // TODO: handle percent as modulo
    // and as * 0.01

    Node *left = NULL;

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
            Node *operand = parse(parser, PREC_UNARY);
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
            Node *operand = parse(parser, PREC_FUNC);
            left = newPrefixNode(op, operand);
            break;
        }

        case TOK_MINUS: {
            // unary minus
            Token op = parser->prev;
            Node *operand = parse(parser, PREC_UNARY);
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
    bool isImplicitMult = isImplicitMult(parser->prev.kind, parser->cur.kind);

    while (precedence <= getPrecedence(parser->cur.kind)) {
        if (getPrecedence(parser->cur.kind) == PREC_NONE) {
            break;
        }

        // go to next token so the infix operator is prev
        nextToken(parser);
        Token op = parser->prev;

        switch (op.kind) {
                // left associative tokens:

            case TOK_PLUS:
            case TOK_MINUS:
            case TOK_ASTERISK:
            case TOK_SLASH:

            case TOK_AND:
            case TOK_OR: {
                Node *right = parse(parser, getPrecedence(op.kind) + 1);

                left = newBinaryNode(op, left, right);

                break;
            }

            case TOK_LESS:
            case TOK_GREATER:
            case TOK_LESS_EQUALS:
            case TOK_GREATER_EQUALS: {
                Node *right = parse(parser, getPrecedence(op.kind) + 1);

                left = newBinaryNode(op, left, right);

                break;
            }

            // right associative tokens
            case TOK_EQUALS: {
                Node *rightVal = parse(parser, getPrecedence(op.kind));

                if (left->kind == NODE_VAR) {
                    left = newAssignmentNode(left->var.name, rightVal);
                } else if (left->kind == NODE_FUNCCALL) {
                    Token name = left->funcCall.funcName;

                    StringViewArr *params =
                        newStringViewArr(left->funcCall.argCount);

                    for (int i = 0; i < left->funcCall.argCount; i++) {
                        Node *argNode = left->funcCall.args[i];

                        if (argNode->kind != NODE_VAR) {
                            // TODO: errror better
                            fprintf(stderr, "error\n");
                            exit(1);
                        }

                        StringView *sv = malloc(sizeof(StringView));
                        *sv = argNode->var.name.ident;
                        addStringToStringViewArr(params, sv);
                    }

                    Node *funcDef = newFuncDefNode(name, params, rightVal);
                    freeNode(left);
                    left = funcDef;
                } else {
                    freeNode(rightVal);
                    const char error[] = "Unable to assign to invalid target";
                    Number *result = numNew(NUM_ERROR);
                    numSetError(result, error, strlen(error));
                    left = newLiteralNode(result);
                }
                break;
            }

            case TOK_LPAREN: {
                if (left->kind != NODE_VAR) {
                    fprintf(stderr, "err 3");
                    exit(1);
                }

                Token funcName = left->var.name;
                freeNode(left);

                int argCap = DEF_FUNC_ARGS;
                int argCount = 0;
                Node **args = malloc(argCap * sizeof(Node));

                if (parser->cur.kind != TOK_RPAREN) {
                    args[argCount] = parse(parser, PREC_ASSIGNMENT);
                    argCount++;

                    while (parser->cur.kind == TOK_COMMA) {
                        nextToken(parser);
                        if (argCount >= argCap) {
                            args = realloc(args, 2 * argCap * sizeof(Node));
                            argCap *= 2;
                        }

                        args[argCount] = parse(parser, PREC_ASSIGNMENT);
                    }
                }

                if (parser->cur.kind == TOK_RPAREN) {
                    nextToken(parser);
                } else {
                    fprintf(stderr, "err4\n");
                    exit(1);
                }

                left = newFuncCallNode(funcName, args, argCount);

                break;
            }

            case TOK_EQUALS_EQUALS:
            case TOK_NOT_EQUALS: {
                Node *right = parse(parser, getPrecedence(op.kind));
                left = newBinaryNode(op, left, right);

                break;
            }

            case TOK_CARET: {
                Node *right = parse(parser, getPrecedence(op.kind));
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
                exit(1);
            }
        }
    }

    return left;
}

Node *simplifyTree(Parser *parser, Node *node) {
    if (node == NULL) return NULL;

    switch (node->kind) {
        case NODE_LITERAL:
        case NODE_BOOLEAN: {
            return node;
        }

        case NODE_VAR: {
            Number *result = numNew(NUM_REAL);

            if (lookupVar(parser->env->varStore, &node->var.name.ident,
                          result) != 0) {
                numClear(result);
                numInit(result, NUM_ERROR);

                char error[] = "Invalid variable referenced:";
                numSetError(result, error, strlen(error));
            }
            Node *newNode = newLiteralNode(result);
            return newNode;
        }

        case NODE_ASSIGNMENT: {
            node->assignment.value =
                simplifyTree(parser, node->assignment.value);

            if (node->assignment.value->kind != NODE_LITERAL) {
                fprintf(stderr,
                        "Error: Could not simplify the value of var: %s\n",
                        getCstring(&node->assignment.name.ident));
                exit(1);
            }

            Number *value = &node->assignment.value->literal.value;
            Number *hashmapValue = numNew(value->kind);
            numSet(hashmapValue, value);
            insertVar(parser->env->varStore, &node->assignment.name.ident,
                      hashmapValue);

            Number *newValue = numNew(value->kind);
            numSet(newValue, value);

            Node *newNode = newLiteralNode(newValue);
            freeNode(node);
            return newNode;

            break;
        }

        // unary -, trig, sgn, subfactorial !, not !
        case NODE_PREFIX: {
            node->unary.operand = simplifyTree(parser, node->unary.operand);

            Number *num = &node->unary.operand->literal.value;

            Number *result;

            switch (node->unary.op.kind) {
                case TOK_BANG: {
                    if (node->unary.operand->kind == NODE_LITERAL) {
                        result = numSubfact(num);

                    } else if (node->unary.operand->kind == NODE_BOOLEAN) {
                        result = numNew(NUM_BOOL);
                        Node *newNode = newBooleanNode(result);

                        result->boolean = (num->boolean == 0) ? 1 : 0;

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
                    result = numNeg(num);
                    break;
                }
                case TOK_SIN: {
                    result = numSin(num);
                    break;
                }
                case TOK_COS: {
                    result = numCos(num);
                    break;
                }
                case TOK_TAN: {
                    result = numTan(num);
                    break;
                }
                case TOK_COSEC: {
                    result = numCosec(num);
                    break;
                }
                case TOK_SEC: {
                    result = numSec(num);
                    break;
                }
                case TOK_COT: {
                    result = numCot(num);
                    break;
                }

                case TOK_SGN: {
                    result = numSgn(num);
                    break;
                }

                case TOK_TWOS: {
                    // TODO
                    // result = twosComplement(num);
                    break;
                }

                case TOK_ABS: {
                    result = numAbs(num);
                    break;
                }

                case TOK_SQRT: {
                    result = numSqrt(num);
                    break;
                }

                case TOK_CBRT: {
                    result = numCbrt(num);
                    break;
                }

                case TOK_EXP: {
                    result = numExp(num);
                    break;
                }

                case TOK_LN: {
                    result = numLn(num);
                    break;
                }

                case TOK_LOG: {
                    result = numLog(num);
                    break;
                }

                default: {
                    fprintf(stderr, "Error: Unexpected prefix operator: %s\n",
                            lookupTokenKind(node->unary.op.kind));
                }
            }
            Node *newNode = newLiteralNode(result);
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

            Number *num = &node->unary.operand->literal.value;
            Number *result = NULL;
            switch (node->unary.op.kind) {
                case TOK_BANG: {
                    result = numFact(num);
                    break;
                }

                default: {
                    fprintf(stderr, "Error: Unexpected unary operator: %s\n",
                            lookupTokenKind(node->unary.op.kind));
                }
            }

            Node *newNode = newLiteralNode(result);
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

            Number *left = &node->binary.left->literal.value;
            Number *right = &node->binary.right->literal.value;

            Node *newNode = NULL;
            Number *result = NULL;

            switch (node->binary.op.kind) {
                case TOK_AND: {
                    Number *result = numAnd(left, right);
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_OR: {
                    Number *result = numOr(left, right);
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_BITWISE_AND: {
                    Number *result = numNew(NUM_REAL);
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_EQUALS_EQUALS: {
                    Number *result = numNew(NUM_BOOL);
                    result->boolean = (numCompare(left, right) == 0);

                    newNode = newBooleanNode(result);
                    break;
                }

                case TOK_NOT_EQUALS: {
                    Number *result = numNew(NUM_BOOL);
                    result->boolean = (numCompare(left, right) != 0);

                    newNode = newBooleanNode(result);
                    break;
                }

                case TOK_LESS: {
                    Number *result = numNew(NUM_BOOL);
                    result->boolean = (numCompare(left, right) < 0);

                    newNode = newBooleanNode(result);
                    break;
                }

                case TOK_GREATER: {
                    Number *result = numNew(NUM_BOOL);
                    result->boolean = (numCompare(left, right) > 0);

                    newNode = newBooleanNode(result);
                    break;
                }

                case TOK_LESS_EQUALS: {
                    Number *result = numNew(NUM_BOOL);
                    result->boolean = (numCompare(left, right) <= 0);

                    newNode = newBooleanNode(result);
                    break;
                }

                case TOK_GREATER_EQUALS: {
                    Number *result = numNew(NUM_BOOL);
                    result->boolean = (numCompare(left, right) >= 0);

                    newNode = newBooleanNode(result);
                    break;
                }

                case TOK_PLUS: {
                    result = numAdd(left, right);
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_MINUS: {
                    result = numSubtract(left, right);
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_ASTERISK: {
                    result = numMultiply(left, right);
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_SLASH: {
                    result = numDivide(left, right);
                    newNode = newLiteralNode(result);
                    break;
                }

                case TOK_CARET: {
                    result = numPow(left, right);
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

        default: {
            exit(1);
        }
    }
}

Precedence getPrecedence(TokenKind kind) {
    switch (kind) {
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

        case TOK_NUMBER:
        case TOK_VAR:
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

        case TOK_LPAREN:
            return PREC_CALL;

        default:
            fprintf(stderr, "Warning: using PREC_NONE for token kind: %s\n",
                    lookupTokenKind(kind));
            return PREC_NONE;
    }
}
