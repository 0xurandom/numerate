#include "ll_utils.h"

#include <stdlib.h>

#include "string_view_utils.h"

LL_Node *newLL_Node(char *key, size_t key_length, double value) {
    LL_Node *node = malloc(sizeof(LL_Node));

    StringView *view = newStringView(key, key_length);

    node->key = *view;
    node->value = value;
    node->next = NULL;

    return node;
}

void setLL_Node(LL_Node *node, double value) { node->value = value; }

void setNextLL_Node(LL_Node *node, LL_Node *next_node) {
    node->next = next_node;
    next_node->next = NULL;
}

void freeLL_Node(LL_Node *node) {
    freeStringView(&node->key);
    node->value = 0;

    if (node->next != NULL) {
        freeLL_Node(node->next);
    }
}
