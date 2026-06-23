#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef enum {
    NUMBER,
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    LPAREN,
    RPAREN,
    END,
    UNKNOWN,
} TokenKind;

typedef struct {
    double num;
    TokenKind kind;
} Token;

typedef struct {
    char *string;
    size_t cursor;
} Lexer;

Token tokenise(Lexer *lexer);

#endif
