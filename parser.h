#ifndef PARSER_H
#define  PARSER_H

#include "lexer.h"

typedef struct {
    Token *arr;
    size_t count;
    size_t capacity;
} Stack;

typedef struct {
    Lexer* lexer;
    Token cur;
    Token prev;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY
} Precedence;

typedef enum {
    NODE_LITERAL,
    NODE_UNARY,
    NODE_BINARY,
} NodeType;

struct Node {
    union{

        struct{
            double value;
        } literal;

        struct{
            Token op;
            struct Node* operand;
        } unary;

        struct{
            Token op;
            struct Node* left;
            struct Node* right;
        } binary;
    } data;

    NodeType type;
};

typedef struct Node Node;

Node* parse(Parser *parser, Precedence precedence);
Precedence getPrecedence(TokenKind kind);
void nextToken(Parser* parser);
Node* newLiteralNode(double num);
Node* newUnaryNode(Token op, Node* operand);
Node* newBinaryNode(Token op, Node* left, Node* right);
char* lookupTokenKind(TokenKind kind);

#endif
