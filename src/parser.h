#pragma once

#include <stdbool.h>

#include "lexer.h"
#include "utils/hashmap_utils.h"
#include "variable_store.h"

typedef struct {
    Lexer* lexer;

    Token cur;
    Token prev;

    HashMap varStore;
} Parser;

// Precedence in ascending order

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_BITWISE_OR,
    PREC_BITWISE_AND,
    PREC_OR,
    PREC_AND,
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
    NODE_UNARY,    // postfix
    NODE_PREFIX,   // can be calculated as unary
    NODE_BINARY,
    NODE_ASSIGNMENT,
} NodeKind;

typedef struct Node Node;

// TODO: add a format kind for lilteral
struct Node {
    NodeKind kind;

    union {
        struct {
            double value;
        } literal;

        struct {
            Token op;
            Node* operand;
        } unary;

        struct {
            Token op;
            Node* left;
            Node* right;
        } binary;

        struct {
            Token name;
            Node* value;
        } assignment;
    };
};

Node* parse(Parser* parser, Precedence precedence);
Node* simplifyTree(Parser* parser, Node* node);
Precedence getPrecedence(TokenKind kind);
void nextToken(Parser* parser);
double evaluateString(Lexer* lexer, Parser* parser, char* str);
void freeNode(Node* node);
bool isArithOp(TokenKind kind);
bool isComparisonOp(TokenKind kind);
