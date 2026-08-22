#pragma once

#include <stdbool.h>

#include "lexer.h"
#include "utils/function_utils.h"
#include "utils/num_utils.h"
#include "utils/set_utils.h"
#include "utils/string_view_arr.h"

#define DEF_FUNC_ARGS 4

typedef struct Env Env;
typedef struct Parser Parser;

struct Parser {
    Lexer *lexer;

    Token cur;
    Token prev;

    Env *env;
};

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
    NODE_VAR,  // subset of assignment
    NODE_FUNCCALL,
    NODE_FUNCDEF,
    NODE_SET,
    NODE_SET_LITERAL,
} NodeKind;

typedef struct Node Node;

// TODO: add a format kind for lilteral
struct Node {
    NodeKind kind;

    union {
        struct {
            Number value;
            const Unit *unit;
        } literal;

        struct {
            Token op;
            Node *operand;
        } unary;

        struct {
            Token op;
            Node *left;
            Node *right;
        } binary;

        struct {
            Token name;
        } var;

        struct {
            Token name;
            Node *value;
        } assignment;

        struct {
            Token funcName;
            struct Node **args;
            int argCount;
        } funcCall;

        struct {
            Token name;
            StringViewArr *params;
            Node *val;
        } funcDef;

        struct {
            Node **elements;
            int count;
        } set;

        struct {
            Set *value;
        } setLiteral;
    };
};

Node *parse(Parser *parser, Precedence precedence);
Node *simplifyTree(Parser *parser, Node *node);
Precedence getPrecedence(TokenKind kind);
void nextToken(Parser *parser);
void freeNode(Node *node);
bool isArithOp(TokenKind kind);
bool isComparisonOp(TokenKind kind);
