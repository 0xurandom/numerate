#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "num_utils.h"

typedef enum {
    UNIT_LENGTH,
    UNIT_MASS,
    UNIT_VOLUME,
    UNIT_TIME,
    UNIT_FREQ,
    UNIT_FORCE,
    UNIT_POWER,
    UNIT_ENERGY,
    UNIT_TEMPERATURE,
    UNIT_PRESSURE,
    UNIT_DIGITAL,
    UNIT_ANGLE,
    UNIT_UNKNOWN
} UnitKind;

typedef struct {
    const char *name;
    UnitKind kind;
    const char *baseNumer;
    const char *baseDenom;
    const char *offset;
} Unit;

typedef struct {
    const char *alias;
    const int aliasLength;
    const Unit *unit;
} UnitAlias;

const Unit *unitLookup(const char *name, size_t len);
UnitKind getUnitKind(const Unit *unit);
bool areUnitsCompatible(const Unit *u1, const Unit *u2);
Number *unitConvert(const Number *val, const Unit *from, const Unit *to);
