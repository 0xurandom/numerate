#include "parser.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ucontext.h>

#include "lexer.h"
#include "utils/function_utils.h"
#include "utils/hashmap_utils.h"
#include "utils/lexer_utils.h"
#include "utils/num_ops.h"
#include "utils/num_utils.h"
#include "utils/parser_utils.h"
#include "utils/set_utils.h"
#include "utils/string_view_arr.h"
#include "utils/string_view_utils.h"
#include "utils/unit_utils.h"

Node *parse(Parser *parser, Precedence precedence) {
    nextToken(parser);

    // TODO: handle percent as modulo
    // and as * 0.01

    Node *left = NULL;

    // handle prefixes

    switch (parser->prev.kind) {
        case TOK_NUMBER: {
            Number *num = &parser->prev.num;
            const Unit *srcUnit = parser->prev.unit;

            if (srcUnit != NULL && parser->cur.kind == TOK_TO) {
                nextToken(parser);

                if (parser->cur.kind != TOK_VAR) {
                    const char error[] = "Expected a unit after 'to'";
                    Number *result = numNew(NUM_ERROR);
                    numSetError(result, error, strlen(error));
                    left = newLiteralNode(result);
                    break;
                }

                StringView targetView = parser->cur.ident;
                const Unit *targetUnit =
                    unitLookup(targetView.arr, targetView.length);

                if (targetUnit == NULL) {
                    const char error[] = "Unknown unit";
                    Number *result = numNew(NUM_ERROR);
                    numSetError(result, error, strlen(error));
                    left = newLiteralNode(result);
                    break;
                }

                if (srcUnit != NULL) {
                    Number *convertedNum =
                        unitConvert(num, srcUnit, targetUnit);
                    left = newLiteralNode(convertedNum);
                    numFree(convertedNum);
                } else {
                    left = newUnitLiteralNode(num, targetUnit);
                }
                nextToken(parser);

            } else {
                left = newLiteralNode(&parser->prev.num);
            }

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
                Number *result = numNew(NUM_BOOL);
                result->kind = NUM_ERROR;
                result->error =
                    *formatStringView("Expected ')', got: %s\n",
                                      lookupTokenKind(parser->cur.kind));
                left = newLiteralNode(result);
            }
            break;

            case TOK_LBRACE: {
                int cap = DEF_FUNC_ARGS;
                int count = 0;

                Node **elements = malloc(cap * sizeof(Node *));

                if (parser->cur.kind != TOK_RBRACE) {
                    elements[count] = parse(parser, PREC_ASSIGNMENT);
                    count++;

                    while (parser->cur.kind == TOK_COMMA) {
                        nextToken(parser);
                        if (count >= cap) {
                            elements =
                                realloc(elements, 2 * cap * sizeof(Node *));
                            cap *= 2;
                        }
                        elements[count] = parse(parser, PREC_ASSIGNMENT);
                        count++;
                    }
                }

                if (parser->cur.kind != TOK_RBRACE) {
                    free(elements);
                    const char error[] = "Expected '}'";
                    Number *result = numNew(NUM_ERROR);
                    numSetError(result, error, strlen(error));
                    left = newLiteralNode(result);
                    break;
                }
                nextToken(parser);

                left = newSetNode(elements, count);
                break;
            }

            break;
        }

        default: {
            Number *result = numNew(NUM_ERROR);
            result->error =
                *formatStringView("Unexpected prefix token: ",
                                  lookupTokenKind(parser->prev.kind));
            left = newLiteralNode(result);

            break;
        }
    }

    // handle infixes and postfixes
    bool implicitMult = isImplicitMult(parser->prev.kind, parser->cur.kind);
    Precedence curPrec =
        implicitMult ? PREC_FACTOR : getPrecedence(parser->cur.kind);

    while ((precedence <= curPrec) && curPrec != PREC_NONE) {
        if (implicitMult) {
            Node *right = parse(parser, PREC_FACTOR);
            left = newBinaryNode(newToken(TOK_ASTERISK), left, right);

        } else {
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
                        const char error[] =
                            "Unable to assign to invalid target";
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
                    Node **args = malloc(argCap * sizeof(Node *));

                    if (parser->cur.kind != TOK_RPAREN) {
                        args[argCount] = parse(parser, PREC_ASSIGNMENT);
                        argCount++;

                        while (parser->cur.kind == TOK_COMMA) {
                            nextToken(parser);
                            if (argCount >= argCap) {
                                args =
                                    realloc(args, 2 * argCap * sizeof(Node *));
                                argCap *= 2;
                            }

                            args[argCount] = parse(parser, PREC_ASSIGNMENT);
                            argCount++;
                        }
                    }

                    if (parser->cur.kind != TOK_RPAREN) {
                        free(args);

                        Number *result = numNew(NUM_ERROR);
                        result->error = *formatStringView("Expected ')'");
                        left = newLiteralNode(result);
                        break;
                    }
                    nextToken(parser);
                    left = newFuncCallNode(funcName, args, argCount);

                    free(args);

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
                    Number *result = numNew(NUM_ERROR);
                    result->error =
                        *formatStringView("Unexpected prefix/infix token: %s",
                                          lookupTokenKind(op.kind));
                    left = newLiteralNode(result);

                    break;
                }
            }
        }
        implicitMult = isImplicitMult(parser->prev.kind, parser->cur.kind);
        curPrec = implicitMult ? PREC_FACTOR : getPrecedence(parser->cur.kind);
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
                freeNode(node);
                Number *num = numNew(NUM_BOOL);
                numClear(num);
                num->error = *formatStringView(
                    "Error: Could not simplify the value of var: %s\n",
                    getCstring(&node->assignment.name.ident));

                Node *newNode = newLiteralNode(num);
                return newNode;
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

            Number *result = NULL;

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
                        const char error[] =
                            "Invalid operand for prefix node with bang "
                            "operator";
                        Number *result = numNew(NUM_ERROR);
                        numSetError(result, error, strlen(error));
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
                    const char errorStr[] = "Unexpected prefix operator";
                    result = numNew(NUM_ERROR);
                    numSetError(result, errorStr, strlen(errorStr));
                    break;
                }
            }
            Node *newNode = newLiteralNode(result);
            freeNode(node);
            return newNode;
            break;
        }

        case NODE_UNARY: {
            node->unary.operand = simplifyTree(parser, node->unary.operand);

            if (node->unary.operand->kind != NODE_LITERAL) {
                const char errorStr[] = "Could not simplify unary operand";
                Number *result = numNew(NUM_ERROR);
                numSetError(result, errorStr, strlen(errorStr));
                Node *newNode = newLiteralNode(result);
                freeNode(node);
                return newNode;
            }

            Number *num = &node->unary.operand->literal.value;
            Number *result = NULL;
            switch (node->unary.op.kind) {
                case TOK_BANG: {
                    result = numFact(num);
                    break;
                }

                default: {
                    const char errorStr[] = "Unexpected unary operator";
                    result = numNew(NUM_ERROR);
                    numSetError(result, errorStr, strlen(errorStr));
                    break;
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
                    const char errorStr[] = "Invalid nodes for arith op";
                    Number *result = numNew(NUM_ERROR);
                    numSetError(result, errorStr, strlen(errorStr));
                    Node *newNode = newLiteralNode(result);
                    freeNode(node);
                    return newNode;
                }
            } else if (isComparisonOp(node->binary.op.kind)) {
                if (node->binary.left->kind != NODE_LITERAL ||
                    node->binary.left->kind != NODE_LITERAL) {
                    const char errorStr[] = "Invalid nodes for comparison op";
                    Number *result = numNew(NUM_ERROR);
                    numSetError(result, errorStr, strlen(errorStr));
                    Node *newNode = newLiteralNode(result);
                    freeNode(node);
                    return newNode;
                }
            } else {
                const char errorStr[] =
                    "Binary operator is neither arith nor comparison";
                Number *result = numNew(NUM_ERROR);
                numSetError(result, errorStr, strlen(errorStr));
                Node *newNode = newLiteralNode(result);
                freeNode(node);
                return newNode;
            }

            Number *left = &node->binary.left->literal.value;
            Number *right = &node->binary.right->literal.value;

            if (left->kind == NUM_ERROR) {
                Number *result = numNew(NUM_ERROR);
                numSet(result, left);
                freeNode(node);
                return newLiteralNode(result);
            }

            if (right->kind == NUM_ERROR) {
                Number *result = numNew(NUM_ERROR);
                numSet(result, right);
                freeNode(node);
                return newLiteralNode(result);
            }

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
                    const char errorStr[] = "Unable to simplify token";
                    Number *result = numNew(NUM_ERROR);
                    numSetError(result, errorStr, strlen(errorStr));
                    newNode = newLiteralNode(result);
                    break;
                }
            }

            freeNode(node);
            return newNode;
        }

        case NODE_FUNCDEF: {
            Func *func =
                newFuncWithArr(&node->funcDef.name.ident, node->funcDef.val,
                               node->funcDef.params);
            addToFuncArr(parser->env->funcArr, func);
            free(node);
            return NULL;
        }

        case NODE_FUNCCALL: {
            Number *result = evaluateFunction(parser, node);
            Node *newNode = newLiteralNode(result);
            freeNode(node);

            return newNode;
        }

        case NODE_SET: {
            Set *set = newSet();

            for (int i = 0; i < node->set.count; i++) {
                node->set.elements[i] =
                    simplifyTree(parser, node->set.elements[i]);

                if (node->set.elements[i]->kind != NODE_LITERAL) {
                    freeSet(set);

                    for (int j = 0; j <= i; j++) {
                        freeNode(node->set.elements[j]);
                    }
                    free(node->set.elements);
                    free(node);

                    Number *result = numNew(NUM_ERROR);
                    const char error[] =
                        "Set elements do not simplify to a literal value";
                    numSetError(result, error, strlen(error));

                    return newLiteralNode(result);
                }

                insertElement(set, &node->set.elements[i]->literal.value);
            }

            for (int i = 0; i < node->set.count; i++) {
                freeNode(node->set.elements[i]);
            }
            free(node->set.elements);
            free(node);

            return newSetLiteralNode(set);
        }

        default: {
            Number *result = numNew(NUM_ERROR);
            result->error =
                *formatStringView("simplifyTree received unknown node kind");
            Node *newNode = newLiteralNode(result);
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
