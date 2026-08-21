#include "unit_utils.h"

#include <gmp-x86_64.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "num_ops.h"
#include "num_utils.h"

#define SI_TABLE(symbol, UnitKind)                         \
    {"p" symbol, UNIT_LENGTH, "1", "1000000000000", "0"},  \
        {"n" symbol, UNIT_LENGTH, "1", "1000000000", "0"}, \
        {"u" symbol, UNIT_LENGTH, "1", "1000000", "0"},    \
        {"m" symbol, UNIT_LENGTH, "1", "1000", "0"},       \
        {"c" symbol, UNIT_LENGTH, "1", "100", "0"},        \
        {"d" symbol, UNIT_LENGTH, "1", "10", "0"},         \
        {symbol, UNIT_LENGTH, "1", "1", "0"},              \
        {"da" symbol, UNIT_LENGTH, "10", "1", "0"},        \
        {"h" symbol, UNIT_LENGTH, "100", "1", "0"},        \
        {"k" symbol, UNIT_LENGTH, "1000", "1", "0"},       \
        {"M" symbol, UNIT_LENGTH, "1000000", "1", "0"},    \
        {"G" symbol, UNIT_LENGTH, "1000000000", "1", "0"}, \
        {"T" symbol, UNIT_LENGTH, "1000000000000", "1", "0"}

static const Unit table[] = {SI_TABLE("m", UNIT_LENGTH),  // 0 - 12
                             SI_TABLE("g", UNIT_MASS),    // 13 - 25
                             SI_TABLE("l", UNIT_VOLUME),  // 26-38
                             SI_TABLE("s", UNIT_TIME),    // 39-52

                             {"lb", UNIT_MASS, "45359237", "100000000", "0"},
                             {"oz", UNIT_MASS, "45359237", "1600000000", "0"},

                             {"in", UNIT_LENGTH, "127", "5000", "0"},
                             {"ft", UNIT_LENGTH, "381", "1250", "0"},
                             {"yd", UNIT_LENGTH, "1143", "1250", "0"},
                             {"mi", UNIT_LENGTH, "201168", "125", "0"},
                             {"nmi", UNIT_LENGTH, "1852", "1", "0"},

                             {"K", UNIT_TEMPERATURE, "1", "1", "0"},
                             {"C", UNIT_TEMPERATURE, "1", "1", "27315/100"},
                             {"F", UNIT_TEMPERATURE, "5", "9", "45967/100"}};

static const size_t unitTableCount = sizeof(table) / sizeof(table[0]);

#define ALIAS_1(index, a1) {a1, sizeof(a1) - 1, &table[index]}
#define ALIAS_2(index, a1, a2) \
    {a1, sizeof(a1) - 1, &table[index]}, { a2, sizeof(a2) - 1, &table[index] }

#define ALIAS_3(index, a1, a2, a3)                                            \
    {a1, sizeof(a1) - 1, &table[index]}, {a2, sizeof(a2) - 1, &table[index]}, \
    {                                                                         \
        a3, sizeof(a3) - 1, &table[index]                                     \
    }

#define ALIAS_4(index, a1, a2, a3, a4)                                        \
    {a1, sizeof(a1) - 1, &table[index]}, {a2, sizeof(a2) - 1, &table[index]}, \
        {a3, sizeof(a3) - 1, &table[index]}, {                                \
        a4, sizeof(a4) - 1, &table[index]                                     \
    }

#define ALIAS_GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define ALIAS(index, ...)                                             \
    ALIAS_GET_MACRO(__VA_ARGS__, ALIAS_4, ALIAS_3, ALIAS_2, ALIAS_1)( \
        index, __VA_ARGS__)

#define SI_TIER_2(index, a1, a2) \
    {a1, sizeof(a1) - 1, &table[index]}, { a2, sizeof(a2) - 1, &table[index] }

#define SI_TIER_2_DUAL(index, a1, a2, a3)                                     \
    {a1, sizeof(a1) - 1, &table[index]}, {a2, sizeof(a2) - 1, &table[index]}, \
    {                                                                         \
        a3, sizeof(a3) - 1, &table[index]                                     \
    }

#define SI_PREFIXES(startIndex, symbol, word)                  \
    SI_TIER_2((startIndex + 0), "p" symbol, "pico" word),      \
        SI_TIER_2((startIndex + 1), "n" symbol, "nano" word),  \
        SI_TIER_2((startIndex + 2), "u" symbol, "micro" word), \
        SI_TIER_2((startIndex + 3), "m" symbol, "milli" word), \
        SI_TIER_2((startIndex + 4), "c" symbol, "centi" word), \
        SI_TIER_2((startIndex + 5), "d" symbol, "deci" word),  \
        {symbol, sizeof(symbol) - 1, &table[startIndex + 6]},  \
        {word, sizeof(word) - 1, &table[startIndex + 6]},      \
        SI_TIER_2((startIndex), "da" symbol, "deca" word),     \
        SI_TIER_2((startIndex), "h" symbol, "hecto" word),     \
        SI_TIER_2((startIndex), "k" symbol, "kilo" word),      \
        SI_TIER_2((startIndex), "M" symbol, "mega" word),      \
        SI_TIER_2((startIndex), "G" symbol, "giga" word),      \
        SI_TIER_2((startIndex), "T" symbol, "tera" word)

#define SI_PREFIXES_DUAL(startIndex, symbol, word, word2)                    \
    SI_TIER_2_DUAL((startIndex + 0), "p" symbol, "pico" word, "pico" word2), \
        SI_TIER_2_DUAL((startIndex + 1), "n" symbol, "nano" word,            \
                       "nano" word2),                                        \
        SI_TIER_2_DUAL((startIndex + 2), "u" symbol, "micro" word,           \
                       "micro" word2),                                       \
        SI_TIER_2_DUAL((startIndex + 3), "m" symbol, "milli" word,           \
                       "milli" word2),                                       \
        SI_TIER_2_DUAL((startIndex + 4), "c" symbol, "centi" word,           \
                       "centi" word2),                                       \
        SI_TIER_2_DUAL((startIndex + 5), "d" symbol, "deci" word,            \
                       "deci" word2),                                        \
        {symbol, sizeof(symbol) - 1, &table[startIndex + 6]},                \
        {word, sizeof(word) - 1, &table[startIndex + 6]},                    \
        {word2, sizeof(word2) - 1, &table[startIndex + 6]},                  \
        SI_TIER_2_DUAL((startIndex + 7), "da" symbol, "deca" word,           \
                       "deca" word2),                                        \
        SI_TIER_2_DUAL((startIndex + 8), "h" symbol, "hecto" word,           \
                       "hecto" word2),                                       \
        SI_TIER_2_DUAL((startIndex + 9), "k" symbol, "kilo" word,            \
                       "kilo" word2),                                        \
        SI_TIER_2_DUAL((startIndex + 10), "M" symbol, "mega" word,           \
                       "mega" word2),                                        \
        SI_TIER_2_DUAL((startIndex + 11), "G" symbol, "giga" word,           \
                       "giga" word2),                                        \
        SI_TIER_2_DUAL((startIndex + 12), "T" symbol, "tera" word,           \
                       "tera" word2)

static const UnitAlias aliasTable[] = {

    SI_PREFIXES_DUAL(0, "m", "meter", "metre"),
    SI_PREFIXES(13, "g", "gram"),
    ALIAS(26, "L"),
    SI_PREFIXES_DUAL(26, "l", "liter", "litre"),
    SI_PREFIXES(39, "s", "second"),

};

static const size_t aliasTableCount =
    sizeof(aliasTable) / sizeof(aliasTable[0]);

const Unit *unitLookup(const char *name, size_t len) {
    for (size_t i = 0; i < aliasTableCount; i++) {
        if (aliasTable[i].aliasLength == len &&
            strncmp(aliasTable[i].alias, name, len)) {
            return aliasTable[i].unit;
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
