#include "lexer_utils.h"

#include <ctype.h>
#include <mpc.h>
#include <stddef.h>
#include <stdio.h>
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

Number* parseNum(Lexer* lexer) {
    bool isComplex = false;

    char* endptr;
    size_t i;

    for (i = lexer->cursor;
         (isdigit(lexer->string[i]) || lexer->string[i] == 'i' ||
          lexer->string[i] == '/');
         i++) {
        if (lexer->string[i] == 'i') isComplex = true;
    }

    if (isComplex) {
        Number* result = numNew(NUM_COMPLEX);

        if (mpc_strtoc(result->complex, &lexer->string[lexer->cursor], &endptr,
                       10, MPC_RNDNN) == 0) {
            return result;
        } else {
            fprintf(stderr, "Error: Could not parse complex number\n");
            exit(1);
        }

        return result;
    }

    Number* result = numNew(NUM_REAL);

    if (mpfr_strtofr(result->real, &lexer->string[lexer->cursor], &endptr, 10,
                     MPFR_RNDN)) {
        return result;
    } else {
        fprintf(stderr, "Error: Could not parse real number\n");
        exit(1);
    }
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
