#include "parser_utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer_utils.h"
#include "num_utils.h"
#include "string_view_utils.h"

Node *newLiteralNode(Number *num) {
    Node *node = malloc(sizeof(Node));

    node->kind = NODE_LITERAL;

    numInit(&node->literal.value, num->kind);
    numSet(&node->literal.value, num);

    return node;
}

Node *newBooleanNode(Number *num) {
    if (num->kind != NUM_BOOL) {
        fprintf(
            stderr,
            "Warning: num of kind not NUM_BOOLEAN passed to newBooleanNode\n");
    }

    Node *node = malloc(sizeof(Node));
    node->kind = NODE_BOOLEAN;

    Number *boolNum = numConvertandSet(num, NUM_BOOL);
    numSet(&node->literal.value, boolNum);

    numFree(boolNum);

    return node;
}

Node *newAssignmentNode(Token name, Node *value) {
    Node *node = malloc(sizeof(Node));

    node->kind = NODE_ASSIGNMENT;
    node->assignment.name = name;
    node->assignment.value = value;

    return node;
}

Node *newVarNode(Token name) {
    Node *node = malloc(sizeof(Node));

    node->kind = NODE_VAR;
    node->var.name = name;

    return node;
}

Node *newUnaryNode(Token op, Node *operand) {
    Node *node = malloc(sizeof(Node));

    node->kind = NODE_UNARY;
    node->unary.op = op;
    node->unary.operand = operand;

    return node;
}

Node *newPrefixNode(Token op, Node *operand) {
    Node *node = malloc(sizeof(Node));

    node->kind = NODE_PREFIX;
    node->unary.op = op;
    node->unary.operand = operand;

    return node;
}

Node *newBinaryNode(Token op, Node *left, Node *right) {
    Node *node = malloc(sizeof(Node));

    node->kind = NODE_BINARY;

    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;

    return node;
}

Node *newFuncCallNode(Token name, Node **args, int argCount) {
    Node *node = malloc(sizeof(Node));

    node->kind = NODE_FUNCCALL;
    node->funcCall.funcName = name;
    node->funcCall.argCount = argCount;

    node->funcCall.args = malloc((argCount) * sizeof(Node *));

    for (int i = 0; i < argCount; i++) {
        node->funcCall.args[i] = args[i];
    }

    node->funcCall.args[node->funcCall.argCount] = NULL;

    return node;
}

Node *newFuncDefNode(Token name, StringViewArr *params, Node *val) {
    Node *node = malloc(sizeof(Node));
    node->kind = NODE_FUNCDEF;
    node->funcDef.name = name;
    node->funcDef.params = params;
    node->funcDef.val = val;
    return node;
}

Number *evaluateString(Lexer *lexer, Parser *parser, char *str) {
    printf("\n\nevaluating: %s\n", str);
    lexer->string = str;
    lexer->cursor = 0;
    lexer->length = strlen(str);

    parser->cur = tokenise(lexer);

    Node *tree = parse(parser, PREC_ASSIGNMENT);

    if (tree == NULL) {
        Number *error = numNew(NUM_ERROR);
        error->error = *formatStringView("Could not parse syntax tree");
        return error;
    }

    Node *result = simplifyTree(parser, tree);

    if (result == NULL) return NULL;

    Number *result_val = numNew(result->literal.value.kind);
    numSet(result_val, &result->literal.value);

    freeNode(result);

    return result_val;
}

Node *newLiteralNodeWithVal(double val) {
    Node *node = malloc(sizeof(Node));
    node->kind = NODE_LITERAL;
    numInit(&node->literal.value, NUM_REAL);
    numSetRealSd(&node->literal.value, val);

    return node;
}

Node *copyNode(Node *node) {
    if (node == NULL) return NULL;

    Node *newNode = malloc(sizeof(Node));
    newNode->kind = node->kind;

    switch (node->kind) {
        case NODE_LITERAL: {
            numInit(&newNode->literal.value, node->literal.value.kind);
            numSet(&newNode->literal.value, &node->literal.value);
            break;
        }

        case NODE_VAR: {
            newNode->var.name = node->var.name;
            break;
        }

        case NODE_UNARY:
        case NODE_PREFIX: {
            newNode->unary.op = node->unary.op;
            newNode->unary.operand = copyNode(node->unary.operand);
            break;
        }

        case NODE_BINARY: {
            newNode->binary.op = node->binary.op;
            newNode->binary.left = copyNode(node->binary.left);
            newNode->binary.right = copyNode(node->binary.right);
            break;
        }

        case NODE_FUNCCALL: {
            newNode->funcCall.funcName = node->funcCall.funcName;
            newNode->funcCall.argCount = newNode->funcCall.argCount;

            newNode->funcCall.args =
                malloc(newNode->funcCall.argCount * sizeof(Node *));

            for (int i = 0; i < newNode->funcCall.argCount; i++) {
                newNode->funcCall.args[i] = copyNode(node->funcCall.args[i]);
            }

            break;
        }

        default: {
            node->kind = NODE_LITERAL;
            numInit(&newNode->literal.value, NUM_ERROR);
            newNode->literal.value.error =
                *formatStringView("Could not copy Node of unknwon type");
            break;
        }
    }

    return newNode;
}

void nextToken(Parser *parser) {
    parser->prev = parser->cur;

    parser->cur = tokenise(parser->lexer);
}

void freeNode(Node *node) {
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

        case NODE_ASSIGNMENT: {
            freeNode(node->assignment.value);
            break;
        }

        case NODE_FUNCCALL: {
            for (int i = 0; i < node->funcCall.argCount; i++) {
                freeNode(node->funcCall.args[i]);
            }

            free(node->funcCall.args);
            break;
        }

        case NODE_FUNCDEF: {
            freeStringViewArr(node->funcDef.params);
            freeNode(node->funcDef.val);
            break;
        }

        default: {
            break;
        }
    }

    free(node);
}

bool isImplicitMult(TokenKind left, TokenKind right) {
    if (left == TOK_VAR && right == TOK_LPAREN) return false;

    bool leftValid =
        (left == TOK_NUMBER || left == TOK_VAR || left == TOK_RPAREN);
    bool rightValid =
        (right == TOK_VAR || right == TOK_NUMBER || right == TOK_LPAREN ||
         right == TOK_SIN || right == TOK_COS || right == TOK_TAN ||
         right == TOK_COSEC || right == TOK_SEC || right == TOK_COT ||
         right == TOK_SGN || right == TOK_LN || right == TOK_LOG ||
         right == TOK_EXP || right == TOK_SQRT || right == TOK_CBRT);

    return leftValid && rightValid;
}

Node *newAddNode(Node *left, Node *right) {
    return newBinaryNode(newToken(TOK_PLUS), left, right);
}
Node *newSubNode(Node *left, Node *right) {
    return newBinaryNode(newToken(TOK_MINUS), left, right);
}
Node *newMulNode(Node *left, Node *right) {
    return newBinaryNode(newToken(TOK_ASTERISK), left, right);
}
Node *newDivNode(Node *left, Node *right) {
    return newBinaryNode(newToken(TOK_SLASH), left, right);
}
Node *newPowNode(Node *base, Node *exp) {
    return newBinaryNode(newToken(TOK_CARET), base, exp);
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
        case TOK_BITWISE_AND:
        case TOK_BITWISE_OR:
        case TOK_TWOS:
        case TOK_ABS:
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

char *lookupTokenKind(TokenKind kind) {
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
        case TOK_TWOS:
            return "Twos";
        case TOK_SQRT:
            return "Sqrt";
        case TOK_SPACE:
            return "Space";
        case TOK_BITWISE_AND:
            return "Bitwise And";
        case TOK_BITWISE_OR:
            return "Bitwise Or";
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

char *lookupNodeKind(NodeKind kind) {
    switch (kind) {
        case NODE_LITERAL:
            return "Literal";
        case NODE_BOOLEAN:
            return "Boolean";
        case NODE_BINARY:
            return "Binary";
        case NODE_UNARY:
            return "Unary";
        case NODE_ASSIGNMENT:
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

bool canBeNodeLiteral(Node *node) {
    return (node->kind == NODE_LITERAL || node->kind == NODE_BOOLEAN);
}
