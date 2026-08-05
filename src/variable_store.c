#include "variable_store.h"

#include <mpc.h>

#include "utils/hashmap_utils.h"
#include "utils/num_utils.h"
#include "utils/string_view_utils.h"

void initVarStore(HashMap *map) {
    initHashmap(map);

    StringView *pi = newStringView("pi", 2);
    Number *numPi = numNew(NUM_REAL);
    mpfr_const_pi(numPi->real, MPFR_RNDN);

    insertVar(map, pi, numPi);

    StringView *e = newStringView("e", 2);
    Number *numE = numNew(NUM_REAL);
    mpfr_const_euler(numE->real, MPFR_RNDN);

    insertVar(map, e, numE);

    StringView *tau = newStringView("tau", 3);
    Number *numTau = numNew(NUM_REAL);
    mpfr_const_pi(numTau->real, MPFR_RNDN);
    mpfr_mul_ui(numTau->real, numTau->real, 2, MPFR_RNDN);

    insertVar(map, tau, numTau);

    StringView *phi = newStringView("phi", 3);
    Number *numPhi = numNew(NUM_REAL);
    mpfr_set_ui(numPhi->real, 5, MPFR_RNDN);
    mpfr_sqrt(numPhi->real, numPhi->real, MPFR_RNDN);
    mpfr_add_ui(numPhi->real, numPhi->real, 1, MPFR_RNDN);
    mpfr_div_ui(numPhi->real, numPhi->real, 2, MPFR_RNDN);

    insertVar(map, phi, numPhi);
}
