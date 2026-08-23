#include <stdio.h>
#include <string.h>

#include "api.h"
#include "lexer.h"
#include "utils/parser_utils.h"

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    Calc *calc = init_calc();
    char buffer[2048];

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "RESET_CALC") == 0) {
            reset_calc(calc);
            printf("OK\n");
            fflush(stdout);
            continue;
        }

        const Unit *resultUnit = NULL;
        Number *result =
            evaluateString(&calc->lexer, &calc->parser, buffer, &resultUnit);
        numPrint(result);

        if (result->kind != NUM_ERROR && resultUnit != NULL)
            printf(" %s\n", resultUnit->name);
        else
            printf("\n");
    }

    return 0;
}
