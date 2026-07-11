#include "parser_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
