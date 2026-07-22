#pragma once

#include "num_utils.h"
#include "string_view_utils.h"

typedef struct LL_Node LL_Node;

struct LL_Node {
    StringView key;
    Number value;

    LL_Node *next;
};

LL_Node *newLL_Node(const char *key, size_t key_length, const Number *src);
void setLL_Node(LL_Node *node, double value);
void setNextLL_Node(LL_Node *node, LL_Node *next_node);
void freeLL_Node(LL_Node *node);
