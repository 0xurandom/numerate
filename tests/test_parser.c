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
        //
        {"8+21", 29},
        {"-56+318", 262},
        {"798-5401", -4603},

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
