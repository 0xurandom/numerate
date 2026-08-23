#include "ll_utils.h"

#include <stdlib.h>

#include "num_utils.h"
#include "string_view_utils.h"

// copies key and src into LL_node,
// they must be freed seperately
LL_Node *newLL_Node(const char *key, size_t key_length, const Number *src) {
    LL_Node *node = malloc(sizeof(LL_Node));

    initStringViewWithString(&node->key, key, key_length);

    numInit(&node->value, src->kind);
    numSet(&node->value, src);

    node->next = NULL;

    return node;
}

void setLL_Node(LL_Node *node, const Number *value) {
    numFree(&node->value);

    node->value.kind = value->kind;
    numSet(&node->value, value);

    return;
}

void setNextLL_Node(LL_Node *node, LL_Node *next_node) {
    node->next = next_node;
    next_node->next = NULL;
}

void freeLL_Node(LL_Node *node) {
    if (node == NULL) return;

    if (node->next != NULL) {
        freeLL_Node(node->next);
    }

    freeStringView(&node->key);
    numClear(&node->value);
    free(node);
}
