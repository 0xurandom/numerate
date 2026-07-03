#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/lexer.h"
#include "../src/parser.h"
#include "stdbool.h"

typedef struct {
    char *input;
    double ans;
} ParserTestCase;

void test_parser(Lexer *lexer, Parser *parser) {
    ParserTestCase tests[] = {

        {"8+21", 29},
        {"-56+318", 262},
        {"798-5401", -4603},
        {"sin 1.57", 1},
        {"-(6!)", -720},
        {"tan 0.785", 1},
        {"21*70-31+78^2", 7523},
        {"(21+90)/2", 55.5},
        {"sin 1.57/ cos 1.57", 1},
        {"5!-(-sin 0)", 120},

    };

    size_t tests_count = sizeof(tests) / sizeof(ParserTestCase);

    for (int i = 0; i < tests_count; i++) {
        printf("Running case %d: %s\n", i + 1, tests[i].input);

        double result = evaluateString(lexer, parser, tests[i].input);

        // TODO: nodes are not simplified
        assert(result == tests[i].ans);

        printf("Case %d passed!\n\n", i + 1);
    }

    printf("\n\nAll test cases passed!\n\n");
}

int main() {
    Lexer lexer;
    Parser parser = {
        .lexer = &lexer,
    };

    test_parser(&lexer, &parser);

    return 0;
}
