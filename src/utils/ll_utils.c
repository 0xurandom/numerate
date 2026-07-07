#include "ll_utils.h"

#include "string_view_utils.h"

LL_Node initLL_Node(char *key, size_t key_length, double value) {
    LL_Node node = {
        .key = initStringView(key, key_length),
        .value = value,
        .next = NULL,
    };

    return node;
}

void setNextLL_Node(LL_Node *node, LL_Node *next) {
    
}