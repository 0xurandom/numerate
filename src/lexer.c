#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/lexer_utils.h"
#include "utils/string_view_utils.h"

Token tokenise(Lexer* lexer) {
    Token token;

    if (lexer->cursor == lexer->length) {
        token.kind = TOK_END;
        return token;
    }

    switch (lexer->string[lexer->cursor]) {
        case 'a' ... 'z':
        case 'A' ... 'Z': {
            int start = lexer->cursor;

            while (isalnum(lexer->string[lexer->cursor])) {
                lexer->cursor++;
            }

            char* keyword = &lexer->string[start];
            size_t keyword_len = lexer->cursor - start;

            token.kind = lookupKeyword(keyword, keyword_len);

            if (token.kind == TOK_VAR) {
                StringView view;
                newStringView(&view, keyword, keyword_len);
                token.ident = view;
            }

            break;
        }

        case '0' ... '9':
        case '.': {
            token.kind = TOK_NUMBER;

            if (lexer->string[lexer->cursor] == '0') {
                char nextChar = peekNext(lexer);

                if (nextChar == 'x' || nextChar == 'X') {
                    token.num = parseHex(lexer);
                    break;

                } else if (nextChar == 'b' || nextChar == 'B') {
                    token.num = parseBin(lexer);
                    break;
                }
            }

            token.num = parseDec(lexer);

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

        case '<': {
            if (peekNext(lexer) == '=') {
                token.kind = TOK_LESS_EQUALS;
                lexer->cursor = lexer->cursor + 2;
            } else {
                token.kind = TOK_LESS;
                lexer->cursor++;
            }

            break;
        }

        case '>': {
            if (peekNext(lexer) == '=') {
                token.kind = TOK_GREATER_EQUALS;
                lexer->cursor = lexer->cursor + 2;
            } else {
                token.kind = TOK_GREATER;
                lexer->cursor++;
            }

            break;
        }

        case '&': {
            if (peekNext(lexer) == '&') {
                token.kind = TOK_AND;
                lexer->cursor = lexer->cursor + 2;
            } else {
                token.kind = TOK_BITWISE_AND;
                lexer->cursor++;
            }

            break;
        }

        case '|': {
            if (peekNext(lexer) == '|') {
                token.kind = TOK_OR;
                lexer->cursor = lexer->cursor + 2;
            } else {
                token.kind = TOK_BITWISE_OR;
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
            // TODO: this is jank
            if (peekNext(lexer) == '*') {
                token.kind = TOK_CARET;
                lexer->cursor = lexer->cursor + 2;
            } else {
                token.kind = TOK_ASTERISK;
                lexer->cursor++;
            }

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
            StringView view;
            newStringView(&view, &lexer->string[lexer->cursor], 1);
            token.ident = view;
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
