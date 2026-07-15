#include "lexer_utils.h"

#include <stdio.h>
#include <string.h>

#include "parser_utils.h"

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
    };

    size_t keywordsCount = sizeof(keywords) / sizeof(Keyword);

    for (size_t i = 0; i < keywordsCount; i++) {
        if (len == keywords[i].len &&
            memcmp(keyword, keywords[i].string, len) == 0) {
            return keywords[i].tokenKind;
        }
    }

    // string did not match a keyword,
    // is a varibale instead
    return TOK_VAR;
}

double parseDec(Lexer* lexer) {
    int i;
    double value;
    for (i = lexer->cursor; isdigit(lexer->string[i]); i++) {
        value *= 10;
        value += (lexer->string[i] - '0');
    }

    lexer->cursor = i;

    if (lexer->string[lexer->cursor] == '.') {
        int initial_cursor = lexer->cursor;
        lexer->cursor++;

        for (i = lexer->cursor; isdigit(lexer->string[i]); i++) {
            value += (lexer->string[i] - '0') * pow(10, -(i - initial_cursor));
        }

        lexer->cursor = i;
    }

    return value;
}

double parseHex(Lexer* lexer) {
    lexer->cursor = lexer->cursor + 2;
    char* endptr;

    double value = strtol(&lexer->string[lexer->cursor], &endptr, 16);
    int offset = endptr - (lexer->string + lexer->cursor);

    if (*endptr == '.') {
        lexer->cursor += offset + 1;

        int dec_index = 0;

        while (isxdigit(lexer->string[lexer->cursor])) {
            value += hexToInt(lexer->string[lexer->cursor]) *
                     pow(16, -(dec_index + 1));
            dec_index++;
            lexer->cursor++;
        }

    } else {
        lexer->cursor += offset;
    }

    return value;
}

double parseBin(Lexer* lexer) {
    lexer->cursor = lexer->cursor + 2;

    char* endptr;
    double value = strtol(&lexer->string[lexer->cursor], &endptr, 2);

    int offset = endptr - (lexer->string + lexer->cursor);
    lexer->cursor += offset;

    return value;
}

int hexToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}
