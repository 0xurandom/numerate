#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/backend/lexer.h"
#include "../src/backend/parser.h"
#include "../src/backend/utils/num_utils.h"
#include "../src/backend/utils/parser_utils.h"
#include "../src/backend/variable_store.h"
#include "stdbool.h"

typedef struct {
    char* input;
    double ans;
} ParserTestCase;

void test_parser(Lexer* lexer, Parser* parser) {
    ParserTestCase tests[] = {

        {"8+21", 29},
        {"-56+318", 262},
        {"798-5401", -4603},
        {"sin(pi/2)", 1},
        {"-(6!)", -720},
        {"tan 0.785", 1},
        {"21*70-31+78^2", 7523},
        {"(21+90)/2", 55.5},
        {"sin(pi/4)/ cos(pi/4)", 1},
        {"5!-(-sin 0)", 120},

    };

    size_t tests_count = sizeof(tests) / sizeof(ParserTestCase);

    int casesPassed = 0;
    for (int i = 0; i < tests_count; i++) {
        // printf("Running case %d: %s\n", i + 1, tests[i].input);

        Number* result = evaluateString(lexer, parser, tests[i].input);

        printf("Expected: %.9f\tGot: %.9f\n", tests[i].ans, result);

        if (result == tests[i].ans) {
            casesPassed++;
            printf("Case %d passed!\n\n", i + 1);
        } else {
            printf("Case %d FAILED!\n\n", i + 1);
        }
    }

    if (casesPassed == tests_count) {
        printf("\n\nAll test cases passed!\n\n");
    } else {
        printf("\n\n%d/%lu cases passed\n\n", casesPassed, tests_count);
    }
}

int main() {
    HashMap varStore;
    initVarStore(&varStore);

    Lexer lexer;
    Parser parser = {
        .lexer = &lexer,
        .varStore = varStore,
    };

    test_parser(&lexer, &parser);

    return 0;
}
