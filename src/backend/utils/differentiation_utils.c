#include "differentiation_utils.h"

#include "function_utils.h"
#include "lexer_utils.h"
#include "parser_utils.h"

Node* getDerivative(Node* node, StringView* wrtVar) {
    if (node == NULL) return NULL;

    switch (node->kind) {
        case NODE_LITERAL:
        case NODE_BOOLEAN: {
            return newLiteralNodeWithVal(0);
        }

        case NODE_VAR: {
            if (compareViews(&node->var.name.ident, wrtVar))
                return newLiteralNodeWithVal(1);
            else
                newLiteralNodeWithVal(0);
        }

        case NODE_PREFIX: {
            Token op = node->unary.op;
            Node* u = node->unary.operand;
            Node* uDerivative = getDerivative(u, wrtVar);

            switch (op.kind) {
                case TOK_MINUS: {
                    return newPrefixNode(op, getDerivative(u, wrtVar));
                }

                case TOK_SIN: {
                    return newMulNode(
                        newPrefixNode(newToken(TOK_COS),
                                      copyNode(node->unary.operand)),
                        uDerivative);
                }

                case TOK_COS: {
                    Node* negativeSinNode = newPrefixNode(
                        newToken(TOK_MINUS),
                        newPrefixNode(newToken(TOK_SIN), copyNode(u)));
                    return newMulNode(negativeSinNode, uDerivative);
                }

                case TOK_TAN: {
                    Node* sec2 = newPowNode(
                        newPrefixNode(newToken(TOK_SEC), copyNode(u)),
                        newLiteralNodeWithVal(2));
                    return newMulNode(sec2, uDerivative);
                }

                case TOK_COSEC: {
                    Node* minusCosec = newPrefixNode(
                        newToken(TOK_MINUS),
                        newPrefixNode(newToken(TOK_COSEC), copyNode(u)));
                    Node* cot = newPrefixNode(newToken(TOK_COT), copyNode(u));
                    return newMulNode(newMulNode(minusCosec, cot), uDerivative);
                }

                case TOK_SEC: {
                    Node* sec = newPrefixNode(newToken(TOK_SEC), copyNode(u));
                    Node* tan = newPrefixNode(newToken(TOK_TAN), copyNode(u));
                    return newMulNode(newMulNode(sec, tan), uDerivative);
                }

                case TOK_COT: {
                    Node* cosec2 = newPowNode(
                        newPrefixNode(newToken(TOK_COSEC), copyNode(u)),
                        newLiteralNodeWithVal(2));
                    Node* minusCosec2 =
                        newPrefixNode(newToken(TOK_MINUS), cosec2);
                    return newMulNode(minusCosec2, uDerivative);
                }

                case TOK_EXP: {
                    Node* exp = newPrefixNode(newToken(TOK_EXP), copyNode(u));
                    return newMulNode(exp, uDerivative);
                }

                case TOK_LN: {
                    return newDivNode(uDerivative, copyNode(u));
                }

                case TOK_LOG: {
                    Node* ln10 = newPrefixNode(newToken(TOK_LN),
                                               newLiteralNodeWithVal(10));
                    Node* uTimesLn10 = newMulNode(ln10, copyNode(u));
                    return newDivNode(uDerivative, uTimesLn10);
                }

                case TOK_SQRT: {
                    Node* sqrt2 = newMulNode(
                        newLiteralNodeWithVal(2),
                        newPrefixNode(newToken(TOK_SQRT), copyNode(u)));
                    return newDivNode(uDerivative, sqrt2);
                }

                case TOK_ABS: {
                    Node* sgn = newPrefixNode(newToken(TOK_SGN), copyNode(u));
                    return newMulNode(sgn, uDerivative);
                }

                default: {
                    char error[] = "Cannot differentiate unknwon function";
                    Number* num = numNew(NUM_ERROR);
                    numSetError(num, error, strlen(error));
                    return newLiteralNode(num);
                }
            }
        }

        default: {
            break;
        }
    }
}
