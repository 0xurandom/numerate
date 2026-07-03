#include "../src/lexer.h"
#include "stdbool.h"

typedef struct {
    char *input;
    Token tokens[10];
} LexerTestCase;

bool areTokensEqual(Token answer, Token result) {
    if (answer.kind == TOK_NUMBER) {
        if (result.kind == TOK_NUMBER && answer.num == result.num)
            return true;
        else
            return false;
    } else {
        if (answer.kind == result.kind)
            return true;
        else
            return false;
    }
}

void testLexer(Lexer *lexer) {
    LexerTestCase tests[] = {
        {"67 - 218",
         {(Token){.kind = TOK_NUMBER, .num = 67}, (Token){.kind = TOK_MINUS},
          (Token){.kind = TOK_NUMBER, .num = 218}}},
        {"sin 1.57",
         {(Token){.kind = TOK_SIN}, (Token){.kind = TOK_NUMBER, .num = 1.57}}}};

    size_t tests_count = sizeof(tests) / sizeof(LexerTestCase);

    for (size_t i = 0; i < tests_count; i++) {
        //
    }
}

int main() {
    Lexer lexer;
    return 0;
}
