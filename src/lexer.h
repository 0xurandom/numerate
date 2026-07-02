#pragma once
#include <stddef.h>

typedef struct {
    char *arr;
    size_t count;
    size_t capacity;
} Stack;

typedef enum {
    TOK_NUMBER,

    TOK_PLUS,
    TOK_MINUS,
    TOK_ASTERISK,
    TOK_SLASH,
    TOK_CARET,
    TOK_BANG,
    TOK_PERCENT,

    TOK_LPAREN,
    TOK_RPAREN,

    TOK_SIN,
    TOK_COS,
    TOK_TAN,
    TOK_COSEC,
    TOK_SEC,
    TOK_COT,

    TOK_END,
    TOK_UNKNOWN,
} TokenKind;

typedef struct {
    union {
        Stack ident;
        double num;
    };
    TokenKind kind;
} Token;

typedef struct {
    char *string;
    size_t cursor;
    size_t length;
} Lexer;

Token tokenise(Lexer *lexer);
void checkAllocation(void *ptr);
void lexString(char *string);
void printDebug(char *string);
TokenKind lookupKeyword(char *keyword, int len);
