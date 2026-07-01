#include "lexer.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

#define LEXER_DEBUG 1

Token tokenise(Lexer* lexer) {
    Token token;

    if (lexer->cursor == lexer->length) {
        token.kind = TOK_END;
        printDebug("End\n");
        return token;
    }

    switch (lexer->string[lexer->cursor]) {
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
            // if space, ignore and re-tokenise
            lexer->cursor++;
            printDebug("Space\n");
            return tokenise(lexer);
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
