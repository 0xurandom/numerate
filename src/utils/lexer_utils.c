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

// TODO: parse bools and complex as a + ib
Number* parseNum(Lexer* lexer) {
    bool isComplex = false;
    bool isRational = false;

    const char* startPtr = &lexer->string[lexer->cursor];
    char* endPtr = &lexer->string[lexer->cursor];

    size_t i = lexer->cursor;

    while (lexer->string[i] != '\0') {
        char c = lexer->string[i];

        if (isdigit(c)) {
        } else if (c == 'i' || c == 'I') {
            isComplex = true;
            i++;

        } else if (c == '/') {
            isRational = true;
            i++;
        }
    }

    if (isComplex) {
        Number* result = numNew(NUM_COMPLEX);

        mpc_strtoc(result->complex, startPtr, &endPtr, 10, MPC_RNDNN);

        if (endPtr == startPtr) {
            fprintf(stderr, "Error: Could not parse complex number\n");
            numFree(result);
            exit(1);
        }

        lexer->cursor = endPtr - lexer->string;
        return result;
    }

    if (isRational) {
        Number* result = numNew(NUM_RATIONAL);

        size_t len = i - lexer->cursor;
        char* rationalString = malloc(len + 1);

        memcpy(rationalString, startPtr, len);
        rationalString[len] = '\0';

        if (mpq_set_str(result->rational, rationalString, 10) != 0) {
            fprintf(stderr, "Error: Could not parse rational number\n");
            free(rationalString);
            numFree(result);

            exit(1);
        }

        mpq_canonicalize(result->rational);

        lexer->cursor = i;

        free(rationalString);
        return result;
    }

    // parse real numbers
    Number* result = numNew(NUM_REAL);
    mpfr_strtofr(result->real, startPtr, &endPtr, 10, MPFR_RNDN);

    if (endPtr == startPtr) {
        fprintf(stderr, "Error: Could not parse real number\n");
        numFree(result);
        exit(1);
    }

    lexer->cursor = endPtr - lexer->string;
    return result;
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
