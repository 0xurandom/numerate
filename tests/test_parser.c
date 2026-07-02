#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/lexer.h"
#include "../src/main.h"
#include "../src/parser.h"
#include "stdbool.h"

typedef struct {
    char *input;
    double ans;
} TestCase;

double evaluateString(Lexer *lexer, Parser *parser, char *str) {
    printf("\n\nevaluating: %s\n", str);
    lexer->string = str;
    lexer->cursor = 0;
    lexer->length = strlen(str);

    parser->cur = tokenise(lexer);

    Node *tree = parse(parser, PREC_ASSIGNMENT);
    Node *result = simplifyTree(tree);

    double result_val = result->literal.value;
    freeNode(result);

    return result_val;
}

void test_parser(Lexer lexer, Parser parser) {
    TestCase tests[] = {
        //
        {"89+21", 110},
        {"-56+318", 262},
        {"798-5401", -4063},

    };
    size_t tests_count = sizeof(tests) / sizeof(TestCase);

    for (int i = 0; i < tests_count; i++) {
        printf("Running case %d: %s\n", i + 1, tests[i].input);
        double result = evaluateString(&lexer, &parser, tests[i].input);

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

    test_parser(lexer, parser);

    return 0;
}
