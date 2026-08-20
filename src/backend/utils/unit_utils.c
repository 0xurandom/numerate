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
                             {"dm", UNIT_LENGTH, "1", "10", "0"},
                             {"m", UNIT_LENGTH, "1", "1", "0"},
                             {"dam", UNIT_LENGTH, "10", "1", "0"},
                             {"hm", UNIT_LENGTH, "100", "1", "0"},
                             {"km", UNIT_LENGTH, "1000", "1", "0"},
                             {"Mm", UNIT_LENGTH, "1000000", "1", "0"},
                             {"Gm", UNIT_LENGTH, "1000000000", "1", "0"},
                             {"Tm", UNIT_LENGTH, "1000000000000", "1", "0"},

                             {"pg", UNIT_MASS, "1", "1000000000000", "0"},
                             {"ng", UNIT_MASS, "1", "1000000000", "0"},
                             {"ug", UNIT_MASS, "1", "1000000", "0"},
                             {"mg", UNIT_MASS, "1", "1000", "0"},
                             {"cg", UNIT_MASS, "1", "100", "0"},
                             {"dg", UNIT_MASS, "1", "10", "0"},
                             {"G", UNIT_MASS, "1", "1", "0"},
                             {"daG", UNIT_MASS, "10", "1", "0"},
                             {"hg", UNIT_MASS, "100", "1", "0"},
                             {"kg", UNIT_MASS, "1000", "1", "0"},
                             {"Mg", UNIT_MASS, "1000000", "1", "0"},
                             {"Gg", UNIT_MASS, "1000000000", "1", "0"},
                             {"Tg", UNIT_MASS, "1000000000000", "1", "0"},

                             {"pl", UNIT_VOLUME, "1", "1000000000000", "0"},
                             {"nl", UNIT_VOLUME, "1", "1000000000", "0"},
                             {"ul", UNIT_VOLUME, "1", "1000000", "0"},
                             {"ml", UNIT_VOLUME, "1", "1000", "0"},
                             {"cl", UNIT_VOLUME, "1", "100", "0"},
                             {"dl", UNIT_VOLUME, "1", "10", "0"},
                             {"l", UNIT_VOLUME, "1", "1", "0"},
                             {"dal", UNIT_VOLUME, "10", "1", "0"},
                             {"hl", UNIT_VOLUME, "100", "1", "0"},
                             {"kl", UNIT_VOLUME, "1000", "1", "0"},
                             {"Ml", UNIT_VOLUME, "1000000", "1", "0"},
                             {"Gl", UNIT_VOLUME, "1000000000", "1", "0"},
                             {"Tl", UNIT_VOLUME, "1000000000000", "1", "0"},

                             {"ps", UNIT_TIME, "1", "1000000000000", "0"},
                             {"ns", UNIT_TIME, "1", "1000000000", "0"},
                             {"us", UNIT_TIME, "1", "1000000", "0"},
                             {"ms", UNIT_TIME, "1", "1000", "0"},
                             {"cs", UNIT_TIME, "1", "100", "0"},
                             {"ds", UNIT_TIME, "1", "10", "0"},
                             {"s", UNIT_TIME, "1", "1", "0"},
                             {"das", UNIT_TIME, "10", "1", "0"},
                             {"hs", UNIT_TIME, "100", "1", "0"},
                             {"ks", UNIT_TIME, "1000", "1", "0"},
                             {"Ms", UNIT_TIME, "1000000", "1", "0"},
                             {"Gs", UNIT_TIME, "1000000000", "1", "0"},
                             {"Ts", UNIT_TIME, "1000000000000", "1", "0"},

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

#define SI_PREFIXES(startIndex, symbol, word)                   \
    SI_TIER_2((startIndex + 0), "p" symbol, "pico" word),       \
        SI_TIER_2((startIndex + 1), "n" symbol, "nano" word),   \
        SI_TIER_2((startIndex + 2), "u" symbol, "micro" word),  \
        SI_TIER_2((startIndex + 3), "m" symbol, "millli" word), \
        SI_TIER_2((startIndex + 4), "c" symbol, "centi" word),  \
        SI_TIER_2((startIndex + 5), "d" symbol, "deci" word),   \
        {symbol, sizeof(symbol) - 1, &table[startIndex + 6]},   \
        {word, sizeof(word) - 1, &table[startIndex + 6]},       \
        SI_TIER_2((startIndex), "da" symbol, "deca" word),      \
        SI_TIER_2((startIndex), "h" symbol, "hecto" word),      \
        SI_TIER_2((startIndex), "k" symbol, "kilo" word),       \
        SI_TIER_2((startIndex), "M" symbol, "mega" word),       \
        SI_TIER_2((startIndex), "G" symbol, "giga" word),       \
        SI_TIER_2((startIndex), "T" symbol, "tera" word)

static const UnitAlias aliasTable[] = {

    SI_PREFIXES(0, "m", "meter"),

    ALIAS(13 + 0, "pg", "picogram"),
    ALIAS(13 + 1, "ng", "nanogram"),
    ALIAS(13 + 2, "ug", "microgram"),
    ALIAS(13 + 3, "mg", "milligram"),
    ALIAS(13 + 4, "cg", "centigram"),
    ALIAS(13 + 5, "dg", "decigram"),
    ALIAS(13 + 6, "g", "gram"),
    ALIAS(13 + 7, "dag", "decagram"),
    ALIAS(13 + 8, "hg", "hectogram"),
    ALIAS(13 + 9, "kg", "kilogram"),
    ALIAS(13 + 10, "Mg", "megagram"),
    ALIAS(13 + 11, "Gg", "gigagram"),
    ALIAS(13 + 12, "Tg", "teragram"),

    ALIAS(26 + 0, "pl", "picoliter", "picolitre"),
    ALIAS(26 + 1, "nl", "nanoliter", "nanolitre"),
    ALIAS(26 + 2, "ul", "microliter", "micrometre"),
    ALIAS(26 + 3, "ml", "milliliter", "millilitre"),
    ALIAS(26 + 4, "cl", "centiliter", "centilitre"),
    ALIAS(26 + 5, "dl", "deciliter", "decilitre"),
    ALIAS(26 + 6, "l", "L", "liter", "litre"),
    ALIAS(26 + 7, "dal", "decaliter", "decalitre"),
    ALIAS(26 + 8, "hl", "hectoliter", "hectolitre"),
    ALIAS(26 + 9, "kl", "kiloliter", "kilolitre"),
    ALIAS(26 + 10, "Ml", "megaliter", "megalitre"),
    ALIAS(26 + 11, "Gl", "gigaliter", "gigalitre"),
    ALIAS(26 + 12, "Tl", "teraliter", "teralitre"),

    ALIAS(39 + 0, "ps", "picosecond"),
    ALIAS(39 + 1, "ns", "nanosecond"),
    ALIAS(39 + 2, "us", "microsecond"),
    ALIAS(39 + 3, "ms", "millisecond"),
    ALIAS(39 + 4, "cs", "centisecond"),
    ALIAS(39 + 5, "ds", "decisecond"),
    ALIAS(39 + 6, "s", "second"),
    ALIAS(39 + 7, "das", "decasecond"),
    ALIAS(39 + 8, "hs", "hectosecond"),
    ALIAS(39 + 9, "ks", "kilogsecond"),
    ALIAS(39 + 10, "Ms", "megasecond"),
    ALIAS(39 + 11, "Gs", "gigasecond"),
    ALIAS(39 + 12, "Ts", "terasecond"),
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
