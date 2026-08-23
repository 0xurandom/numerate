#include "lexer.h"

#include <ctype.h>
#include <mpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/lexer_utils.h"
#include "utils/num_utils.h"
#include "utils/string_view_utils.h"

Token tokenise(Lexer *lexer) {
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

            char *keyword = &lexer->string[start];
            size_t keyword_len = lexer->cursor - start;

            // check for lone 'i's that dont start with a number
            if (keyword_len == 1 && (keyword[0] == 'i' || keyword[0] == 'I')) {
                numInit(&token.num, NUM_COMPLEX);
                mpc_set_ui_ui(token.num.complex, 0, 1, MPC_RNDNN);
                token.kind = TOK_NUMBER;
            } else {
                token.kind = lookupKeyword(keyword, keyword_len);

                if (token.kind == TOK_VAR) {
                    StringView *view = newStringView(keyword, keyword_len);
                    token.ident = *view;
                }
            }

            break;
        }

        case '0' ... '9':
        case '.': {
            token.kind = TOK_NUMBER;

            if (lexer->string[lexer->cursor] == '0') {
                // char nextChar = peekNext(lexer);

                // TODO

                // if (nextChar == 'x' || nextChar == 'X') {
                //     token.num = parseHex(lexer);
                //     break;
                //
                // } else if (nextChar == 'b' || nextChar == 'B') {
                //     token.num = parseBin(lexer);
                //     break;
                // }
            }

            parseNum(lexer, &token);
            if (token.kind == TOK_NUMBER) {
                token.unit = lexUnitSuffix(lexer);
                // numPrint(&token.num);
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

        case '{': {
            token.kind = TOK_LBRACE;
            lexer->cursor++;
            break;
        }

        case '}': {
            token.kind = TOK_RBRACE;
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

        case ',': {
            token.kind = TOK_COMMA;
            lexer->cursor++;
            break;
        }

        case ':': {
            token.kind = TOK_COLON;
            lexer->cursor++;
            break;
        }

        default: {
            token.kind = TOK_UNKNOWN;
            StringView *view = newStringView(&lexer->string[lexer->cursor], 1);
            token.ident = *view;
            lexer->cursor++;
        }
    }

    return token;
}

TokenKind lookupKeyword(char *keyword, int len) {
    Keyword keywords[] = {
        (Keyword){.string = "sin", .len = 3, .tokenKind = TOK_SIN},
        (Keyword){.string = "cos", .len = 3, .tokenKind = TOK_COS},
        (Keyword){.string = "tan", .len = 3, .tokenKind = TOK_TAN},
        (Keyword){.string = "sec", .len = 3, .tokenKind = TOK_SEC},
        (Keyword){.string = "csc", .len = 3, .tokenKind = TOK_COSEC},
        (Keyword){.string = "cosec", .len = 5, .tokenKind = TOK_COSEC},
        (Keyword){.string = "cot", .len = 3, .tokenKind = TOK_COT},
        (Keyword){.string = "sgn", .len = 3, .tokenKind = TOK_SGN},
        (Keyword){.string = "signum", .len = 6, .tokenKind = TOK_SGN},
        (Keyword){.string = "twos", .len = 3, .tokenKind = TOK_TWOS},
        (Keyword){.string = "abs", .len = 3, .tokenKind = TOK_ABS},
        (Keyword){.string = "sqrt", .len = 4, .tokenKind = TOK_SQRT},
        (Keyword){.string = "cbrt", .len = 4, .tokenKind = TOK_CBRT},
        (Keyword){.string = "exp", .len = 3, .tokenKind = TOK_EXP},
        (Keyword){.string = "ln", .len = 2, .tokenKind = TOK_LN},
        (Keyword){.string = "to", .len = 2, .tokenKind = TOK_TO},
        // TODO: log breaks pi for some reason?
        // (Keyword){.string = "log", .len = 3, .tokenKind = TOK_LOG},
    };

    size_t keywordsCount = sizeof(keywords) / sizeof(Keyword);

    for (size_t i = 0; i < keywordsCount; i++) {
        if (len == keywords[i].len &&
            memcmp(keyword, keywords[i].string, len) == 0) {
            return keywords[i].tokenKind;
        }
    }

    return TOK_VAR;
}
