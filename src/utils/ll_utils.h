#pragma once

#include "string_view_utils.h"

typedef struct LL_Node LL_Node;

struct LL_Node {
    StringView key;
    double value;

    LL_Node *next;
};
