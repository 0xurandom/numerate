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

            switch (op.kind) {
                case TOK_MINUS: {
                    return newPrefixNode(
                        op, getDerivative(node->unary.operand, wrtVar));
                }

                case TOK_SIN: {
                    return newBinaryNode(
                        newToken(TOK_ASTERISK),
                        newPrefixNode(newToken(TOK_COS),
                                      copyNode(node->unary.operand)),
                        getDerivative(node->unary.operand, wrtVar));
                }

                case TOK_COS: {
                    Node* negativeSinNode = newPrefixNode(
                        newToken(TOK_MINUS),
                        newPrefixNode(newToken(TOK_SIN),
                                      copyNode(node->unary.operand)));
                    return newBinaryNode(
                        newToken(TOK_ASTERISK), negativeSinNode,
                        getDerivative(node->unary.operand, wrtVar));
                }

                                default: {
                    break;
                }
            }
        }

        default: {
            break;
        }
    }
}
