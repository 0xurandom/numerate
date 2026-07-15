#pragma once

#include "string_view_utils.h"

typedef struct LL_Node LL_Node;

struct LL_Node {
    StringView key;
    double value;

    LL_Node *next;
};

void initLL_Node(LL_Node *node, char *key, size_t key_length, double value);
void setLL_Node(LL_Node *node, double value);
void setNextLL_Node(LL_Node *node, LL_Node *next_node);
void freeLL_Node(LL_Node *node);
