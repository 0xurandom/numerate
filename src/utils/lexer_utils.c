#include "lexer_utils.h"

#include <string.h>

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

    else if (len == 3 && memcmp(keyword, "and", 3) == 0)
        return TOK_AND;
    else if (len == 2 && memcmp(keyword, "or", 2) == 0)
        return TOK_OR;

    else if (len == 3 && memcmp(keyword, "sgn", 3) == 0)
        return TOK_SGN;
    else if (len == 4 && memcmp(keyword, "twos", 4) == 0)
        return TOK_TWOS;
    else if (len == 3 && memcmp(keyword, "abs", 3) == 0)
        return TOK_ABS;

    else {
        return TOK_VAR;
    }
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
