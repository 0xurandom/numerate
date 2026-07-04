#include "lexer.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

#define LEXER_DEBUG 0

Token tokenise(Lexer* lexer) {
    Token token;

    if (lexer->cursor == lexer->length) {
        token.kind = TOK_END;
        printDebug("End\n");
        return token;
    }

    switch (lexer->string[lexer->cursor]) {
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

            if (LEXER_DEBUG == 1) printf("%.*s", keyword_len, keyword);
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

            if (LEXER_DEBUG == 1) printf("%f\n", token.num);

            break;
        }

        case '=': {
            token.kind = TOK_EQUALS;
            lexer->cursor++;
            printDebug("=\n");
            break;
        }

        case '+': {
            token.kind = TOK_PLUS;
            lexer->cursor++;
            printDebug("+\n");
            break;
        }
        case '-': {
            token.kind = TOK_MINUS;
            lexer->cursor++;
            printDebug("-\n");
            break;
        }
        case '*': {
            token.kind = TOK_ASTERISK;
            lexer->cursor++;
            printDebug("*\n");
            break;
        }
        case '/': {
            token.kind = TOK_SLASH;
            lexer->cursor++;
            printDebug("/\n");
            break;
        }

        case '^': {
            token.kind = TOK_CARET;
            lexer->cursor++;
            printDebug("^\n");
            break;
        }

        case '%': {
            token.kind = TOK_PERCENT;
            lexer->cursor++;
            printDebug("%\n");
            break;
        }

        case '!': {
            token.kind = TOK_BANG;
            lexer->cursor++;
            printDebug("!\n");
            break;
        }

        case '(': {
            token.kind = TOK_LPAREN;
            lexer->cursor++;
            printDebug("(\n");
            break;
        }

        case ')': {
            token.kind = TOK_RPAREN;
            lexer->cursor++;
            printDebug(")\n");
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
            printDebug("Unknown\n");
        }
    }

    return token;
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

void printDebug(char* string) {
    if (LEXER_DEBUG == 1) printf("%s", string);

    return;
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
