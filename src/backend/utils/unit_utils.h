#pragma once

#include <stddef.h>
#include <string.h>

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
