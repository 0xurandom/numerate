#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef enum {
    TOK_NUMBER,
    TOK_PLUS,
    TOK_MINUS,
    TOK_ASTERISK,
    TOK_SLASH,
    TOK_CARET,
    TOK_PERCENT,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_END,
    TOK_UNKNOWN,
} TokenKind;

typedef struct {
    double num;
    TokenKind kind;
} Token;

typedef struct {
    char *string;
    size_t cursor;
    size_t length;
} Lexer;

Token tokenise(Lexer *lexer);

#endif
