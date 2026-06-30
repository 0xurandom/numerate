#include "lexer.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LEXER_DEBUG 0

Token tokenise(Lexer* lexer) {
    Token token;

    switch (lexer->string[lexer->cursor]) {
        case '\0': {
            token.kind = TOK_END;
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

            // printf("%f\n", token.num);
            printDebug("number\n");
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
            token.kind = TOK_SPACE;
            lexer->cursor++;
            printDebug("space\n");
            break;
        }

        default: {
            token.kind = TOK_UNKNOWN;
            lexer->cursor++;
            printDebug("unknown\n");
            break;
        }
    }

    return token;
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
