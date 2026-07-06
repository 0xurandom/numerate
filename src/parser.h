#pragma once

#include <stdbool.h>

#include "lexer.h"

typedef struct {
    Lexer* lexer;
    Token cur;
    Token prev;
} Parser;

// Precedence in ascending order

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_EQUALILTY,   // !=, ==
    PREC_COMPARISON,  // <, >, <=, >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // -x, !x prefixes
    PREC_EXPONENT,    // ^
    PREC_FUNC,        // trig
    PREC_POSTFIX,     // x! postfixes
    PREC_CALL,
    PREC_PRIMARY
} Precedence;

typedef enum {
    NODE_LITERAL,
    NODE_BOOLEAN,  // can be calculated as literal
    NODE_VARIABLE,
    NODE_UNARY,   // postfix
    NODE_PREFIX,  // can be calculated as unary
    NODE_BINARY,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;

    union {
        struct {
            double value;
        } literal;

        struct {
            Token name;
        } variable;

        struct {
            Token op;
            Node* operand;
        } unary;

        struct {
            Token op;
            Node* left;
            Node* right;
        } binary;
    };
};

Node* parse(Parser* parser, Precedence precedence);
Node* simplifyTree(Node* node);
Precedence getPrecedence(TokenKind kind);
void nextToken(Parser* parser);
double evaluateString(Lexer* lexer, Parser* parser, char* str);
void freeNode(Node* node);
Node* newLiteralNode(double num);
Node* newBooleanNode(double num);
Node* newVariableNode(Token name);
Node* newUnaryNode(Token op, Node* operand);
Node* newPrefixNode(Token op, Node* operand);
Node* newBinaryNode(Token op, Node* left, Node* right);
char* lookupTokenKind(TokenKind kind);
char* lookupNodeKind(NodeKind kind);
bool canBeNodeLiteral(Node* node);
bool isArithOp(TokenKind kind);
bool isComparisonOp(TokenKind kind);
