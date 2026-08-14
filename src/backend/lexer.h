#pragma once
#include <stddef.h>

#include "utils/num_utils.h"
#include "utils/string_view_utils.h"

typedef enum {
    TOK_VAR,
    TOK_NUMBER,

    TOK_EQUALS_EQUALS,
    TOK_NOT_EQUALS,
    TOK_LESS,
    TOK_GREATER,
    TOK_GREATER_EQUALS,
    TOK_LESS_EQUALS,

    TOK_BITWISE_AND,
    TOK_BITWISE_OR,
    TOK_AND,
    TOK_OR,

    TOK_EQUALS,
    TOK_PLUS,
    TOK_MINUS,
    TOK_ASTERISK,
    TOK_SLASH,
    TOK_CARET,
    TOK_BANG,
    TOK_PERCENT,
    TOK_COMMA,

    TOK_LPAREN,
    TOK_RPAREN,

    TOK_SIN,
    TOK_COS,
    TOK_TAN,
    TOK_COSEC,
    TOK_SEC,
    TOK_COT,

    TOK_SGN,
    TOK_ABS,
    TOK_TWOS,
    TOK_SQRT,
    TOK_CBRT,
    TOK_EXP,
    TOK_LN,
    TOK_LOG,

    TOK_SPACE,
    TOK_END,
    TOK_UNKNOWN,
} TokenKind;

typedef struct {
    // TODO: remove stack.cap
    // if zero copy lexing
    union {
        StringView ident;
        Number num;
    };
    TokenKind kind;
} Token;

typedef struct {
    char* string;
    size_t cursor;
    size_t length;
} Lexer;

typedef struct {
    char string[10];
    int len;
    TokenKind tokenKind;
} Keyword;

Token tokenise(Lexer* lexer);
TokenKind lookupKeyword(char* keyword, int len);
