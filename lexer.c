#include "lexer.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>

// an arithmetic lexer

// int main() {
//     char testString[] = "5-2*4";
//
//     Lexer lexer = {
//         .string = testString,
//         .cursor = 0,
//     };
//
//     while (tokenise(&lexer).kind != END) {
//         tokenise(&lexer);
//     }
//
//     return 0;
// }

Token tokenise(Lexer *lexer) {
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
            break;
        }

        case '+': {
            token.kind = TOK_PLUS;
            lexer->cursor++;
            // printf("+\n");
            break;
        }
        case '-': {
            token.kind = TOK_MINUS;
            lexer->cursor++;
            // printf("-\n");
            break;
        }
        case '*': {
            token.kind = TOK_ASTERISK;
            lexer->cursor++;
            // printf("*\n");
            break;
        }
        case '/': {
            token.kind = TOK_SLASH;
            lexer->cursor++;
            // printf("/\n");
            break;
        }

        case ' ': {
            break;
        }

        default: {
            token.kind = TOK_UNKNOWN;
            lexer->cursor++;
            // printf("unknown\n");
            break;
        }
    }

    return token;
}
