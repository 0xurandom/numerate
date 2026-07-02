#pragma once

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
    NODE_UNARY,
    NODE_BINARY,
} NodeKind;

// TODO: change union to
// anonymous union

struct Node {
    union {
        struct {
            double value;
        } literal;

        struct {
            // TODO: change token op to tokenkind op
            Token op;
            struct Node* operand;
        } unary;

        struct {
            Token op;
            struct Node* left;
            struct Node* right;
        } binary;
    } data;

    NodeKind kind;
};

typedef struct Node Node;

Node* parse(Parser* parser, Precedence precedence);
Node* simplifyTree(Node* node);
Precedence getPrecedence(TokenKind kind);
void nextToken(Parser* parser);
void freeNode(Node* node);
Node* newLiteralNode(double num);
Node* newUnaryNode(Token op, Node* operand);
Node* newBinaryNode(Token op, Node* left, Node* right);
char* lookupTokenKind(TokenKind kind);
