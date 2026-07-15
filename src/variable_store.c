#include "variable_store.h"

#include "utils/string_view_utils.h"

void initVarStore(HashMap *map) {
    initHashmap(map);

    StringView pi;
    newStringView(&pi, "pi", 2);
    insertVar(map, &pi, 3.14159);

    StringView tau;
    newStringView(&tau, "tau", 3);
    insertVar(map, &tau, 6.2832);

    StringView phi;
    newStringView(&phi, "phi", 3);
    insertVar(map, &phi, 1.6180);
}
