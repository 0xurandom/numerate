#pragma once

#include <stddef.h>

typedef enum {
    UNIT_LENGTH,
    UNIT_MASS,
    UNIT_TIME,
    UNIT_TEMPERATURE,
    UNIT_VOLUME,
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
    const Unit *unit;
} UnitAlias;
