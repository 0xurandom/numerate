#include "unit_utils.h"

#include <gmp-x86_64.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "num_ops.h"
#include "num_utils.h"

// TODO: add strlen to these tables
static const Unit table[] = {{"pm", UNIT_LENGTH, "1", "1000000000000", "0"},
                             {"nm", UNIT_LENGTH, "1", "1000000000", "0"},
                             {"um", UNIT_LENGTH, "1", "1000000", "0"},
                             {"mm", UNIT_LENGTH, "1", "1000", "0"},
                             {"cm", UNIT_LENGTH, "1", "100", "0"},
                             {"m", UNIT_LENGTH, "1", "1", "0"},

                             {"km", UNIT_LENGTH, "1000", "1", "0"},
                             {"in", UNIT_LENGTH, "127", "5000", "0"},
                             {"ft", UNIT_LENGTH, "381", "1250", "0"},
                             {"yd", UNIT_LENGTH, "1143", "1250", "0"},
                             {"mi", UNIT_LENGTH, "201168", "125", "0"},
                             {"nmi", UNIT_LENGTH, "1852", "1", "0"},

                             {"kg", UNIT_MASS, "1", "1", "0"},
                             {"g", UNIT_MASS, "1", "1000", "0"},
                             {"mg", UNIT_MASS, "1", "1000000", "0"},
                             {"lb", UNIT_MASS, "45359237", "100000000", "0"},
                             {"oz", UNIT_MASS, "45359237", "1600000000", "0"},

                             {"K", UNIT_TEMPERATURE, "1", "1", "0"},
                             {"C", UNIT_TEMPERATURE, "1", "1", "27315/100"},
                             {"F", UNIT_TEMPERATURE, "5", "9", "45967/100"}};

static const size_t unitTableCount = sizeof(table) / sizeof(table[0]);

static const UnitAlias aliasTable[] = {
    {"nm", &table[1]},
    {"nanometer", &table[1]},
    {"nanometre", &table[1]},
    //
    {"mm", &table[3]},
    {"millimeter", &table[3]},
    {"millimetre", &table[3]},

    {"cm", &table[3]},
    {"centimeter", &table[3]},
    {"centimetre", &table[3]},

    {"m", &table[5]},
    {"meter", &table[5]},
    {"metre", &table[5]},

};
const Unit *unitLookup(const char *name, size_t len) {
    for (size_t i = 0; i < unitTableCount; i++) {
        if (strlen(table[i].name) == len && strncmp(table[i].name, name, len)) {
            return &table[i];
        }
    }
    return NULL;
}

UnitKind getUnitKind(const Unit *unit) {
    if (unit == NULL) return UNIT_UNKNOWN;

    return unit->kind;
}

bool areUnitsCompatible(const Unit *u1, const Unit *u2) {
    return (u1 != NULL) && (u2 != NULL) && (getUnitKind(u1) == getUnitKind(u2));
}

Number *unitConvert(const Number *val, const Unit *from, const Unit *to) {
    if (!areUnitsCompatible(from, to)) {
        Number *result = numNew(NUM_ERROR);
        const char error[] =
            "Cannot convert between incompatible unit categories";
        numSetError(result, error, strlen(error));
        return result;
    }

    if (getUnitKind(from) == UNIT_TEMPERATURE) {
        Number *scaleFrom =
            rationalStringToNum(from->baseNumer, from->baseDenom);
        Number *offsetFrom = numNew(NUM_RATIONAL);

        mpq_set_str(offsetFrom->rational, from->offset, 10);
        Number *scaled = numMultiply(val, scaleFrom);
        Number *added = numAdd(scaled, offsetFrom);

        numFrees(scaleFrom, offsetFrom, scaled, (Number *)NULL);

        Number *scaleTo = rationalStringToNum(to->baseNumer, to->baseDenom);
        Number *offsetTo = numNew(NUM_RATIONAL);
        mpq_set_str(offsetTo->rational, to->offset, 10);

        Number *diff = numSubtract(added, offsetTo);
        Number *result = numDivide(diff, scaleTo);

        numFrees(added, scaleTo, offsetTo, diff, (Number *)NULL);
        return result;
    }

    Number *fromFactor = rationalStringToNum(from->baseNumer, from->baseDenom);
    Number *toFactor = rationalStringToNum(to->baseNumer, to->baseDenom);

    Number *inBase = numMultiply(val, fromFactor);
    Number *result = numDivide(inBase, toFactor);

    numFrees(fromFactor, toFactor, inBase, (Number *)NULL);
    return result;
}
