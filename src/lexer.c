#include "lexer.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

Token tokenise(Lexer* lexer) {
    Token token;

    if (lexer->cursor == lexer->length) {
        token.kind = TOK_END;
        return token;
    }

    switch (lexer->string[lexer->cursor]) {
        // TODO: lex idents and nums
        // as char* in Stack and
        // change to nums in parser
        case 'a' ... 'z':
        case 'A' ... 'Z': {
            int start = lexer->cursor;

            while (isalpha(lexer->string[lexer->cursor])) {
                lexer->cursor++;
            }

            int keyword_len = lexer->cursor - start;

            char* keyword = &lexer->string[start];

            token.kind = lookupKeyword(keyword, keyword_len);

            if (token.kind == TOK_VAR) {
                token.ident = (Stack){.arr = keyword,
                                      .count = keyword_len,
                                      .capacity = keyword_len};
            }

            break;
        }

        case '0' ... '9':
        case '.': {
            token.num = 0;
            token.kind = TOK_NUMBER;

            int i;
            for (i = lexer->cursor; isdigit(lexer->string[i]); i++) {
                token.num *= 10;
                token.num += (lexer->string[i] - '0');
            }

            lexer->cursor = i;

            if (lexer->string[lexer->cursor] == '.') {
                int initial_cursor = lexer->cursor;
                lexer->cursor++;

                for (i = lexer->cursor; isdigit(lexer->string[i]); i++) {
                    token.num += (lexer->string[i] - '0') *
                                 pow(10, -(i - initial_cursor));
                }

                lexer->cursor = i;
            }

            break;
        }

        case '=': {
            if (peekNext(lexer) == '=') {
                token.kind = TOK_EQUALS_EQUALS;
                lexer->cursor = lexer->cursor + 2;

            } else {
                token.kind = TOK_EQUALS;
                lexer->cursor++;
            }

            break;
        }

        case '+': {
            token.kind = TOK_PLUS;
            lexer->cursor++;

            break;
        }
        case '-': {
            token.kind = TOK_MINUS;
            lexer->cursor++;

            break;
        }
        case '*': {
            token.kind = TOK_ASTERISK;
            lexer->cursor++;

            break;
        }
        case '/': {
            token.kind = TOK_SLASH;
            lexer->cursor++;

            break;
        }

        case '^': {
            token.kind = TOK_CARET;
            lexer->cursor++;

            break;
        }

        case '%': {
            token.kind = TOK_PERCENT;
            lexer->cursor++;

            break;
        }

        case '!': {
            if (peekNext(lexer) == '=') {
                token.kind = TOK_NOT_EQUALS;
                lexer->cursor = lexer->cursor + 2;

            } else {
                token.kind = TOK_BANG;
                lexer->cursor++;
            }

            break;
        }

        case '(': {
            token.kind = TOK_LPAREN;
            lexer->cursor++;

            break;
        }

        case ')': {
            token.kind = TOK_RPAREN;
            lexer->cursor++;

            break;
        }

        case ' ': {
            lexer->cursor++;
            return tokenise(lexer);

            // token.kind = TOK_SPACE;
            // lexer->cursor++;
            // printDebug("Space\n");
            // break;
        }

        default: {
            token.kind = TOK_UNKNOWN;
            lexer->cursor++;
        }
    }

    return token;
}

char peekNext(Lexer* lexer) {
    if (lexer->cursor + 1 > lexer->length) {
        // TODO: handle this error gracefully
        fprintf(stderr, "Error: string accessed at illegal index\n");
        exit(1);
    } else {
        return lexer->string[lexer->cursor + 1];
    }
}

void lexString(char* string) {
    int len = strlen(string);
    Lexer lexer = {
        .string = string,
        .length = len,
        .cursor = 0,
    };

    while (lexer.cursor != lexer.length) {
        printf("%s\n", lookupTokenKind(tokenise(&lexer).kind));
    }
    return;
}

void checkAllocation(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
}

TokenKind lookupKeyword(char* keyword, int len) {
    if (len == 3 && memcmp(keyword, "sin", 3) == 0)
        return TOK_SIN;
    else if (len == 3 && memcmp(keyword, "cos", 3) == 0)
        return TOK_COS;
    else if (len == 3 && memcmp(keyword, "tan", 3) == 0)
        return TOK_TAN;
    else if (len == 5 && memcmp(keyword, "cosec", 5) == 0)
        return TOK_COSEC;
    else if (len == 3 && memcmp(keyword, "sec", 3) == 0)
        return TOK_SEC;
    else if (len == 3 && memcmp(keyword, "cot", 3) == 0)
        return TOK_COT;
    else if (len == 3 && memcmp(keyword, "sgn", 3) == 0)
        return TOK_SGN;

    else {
        return TOK_VAR;
    }
}
