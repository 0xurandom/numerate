#include "lexer_utils.h"

#include <ctype.h>
#include <gmp-x86_64.h>
#include <mpc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "num_utils.h"
#include "parser_utils.h"

void lexString(char* string) {
    int len = strlen(string);
    Lexer lexer = {
        .string = string,
        .length = len,
        .cursor = 0,
    };

    // while (lexer.cursor != lexer.length) {
    // printf("%s\n", lookupTokenKind(tokenise(&lexer).kind));
    // }

    return;
}

void checkAllocation(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
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

void parseNum(Lexer* lexer, Token* token) {
    const char* startPtr = &lexer->string[lexer->cursor];
    char* endPtr = (char*)startPtr;

    mpfr_t num1;
    mpfr_init2(num1, PRECISION);

    mpfr_strtofr(num1, startPtr, &endPtr, 10, MPFR_RNDN);

    if (startPtr == endPtr) {
        fprintf(stderr, "Error: Unable to parse Number\n");
        mpfr_clear(num1);
        exit(1);
    }

    // is complex
    if (*endPtr == 'i' || *endPtr == 'I') {
        numInit(&token->num, NUM_COMPLEX);

        mpc_set_ui_ui(token->num.complex, 0, 0, MPC_RNDNN);
        mpfr_set(mpc_imagref(token->num.complex), num1, MPFR_RNDN);

        mpfr_clear(num1);
        return;
    }

    // is rational
    if (*endPtr == '/') {
        char* denomEnd = endPtr++;

        while (isdigit(*denomEnd)) {
            denomEnd++;
        }

        int len = denomEnd - startPtr;
        char* rationalStr = malloc(len + 1);
        memcpy(rationalStr, startPtr, len);
        rationalStr[len] = '\0';

        numInit(&token->num, NUM_RATIONAL);

        if (mpq_set_str(token->num.rational, rationalStr, 10) != 0) {
            fprintf(stderr, "Error: Failed to parse rational number\n");
            free(rationalStr);
            mpfr_clear(num1);
            exit(1);
        }

        mpq_canonicalize(token->num.rational);
        free(rationalStr);
        mpfr_clear(num1);

        lexer->cursor = endPtr - lexer->string;
        return;
    }

    mpfr_clear(num1);
}

int hexToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
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
