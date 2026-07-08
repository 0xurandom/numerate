#include "variable_store.h"

#include "utils/string_view_utils.h"

HashMap newVarStore() {
    HashMap varStore = newHashmap();

    StringView pi = newStringView("pi", 2);
    insertVar(&varStore, &pi, 3.14159);

    StringView tau = newStringView("tau", 3);
    insertVar(&varStore, &tau, 6.2832);

    StringView phi = newStringView("phi", 3);
    insertVar(&varStore, &tau, 1.6180);

    return varStore;
}
