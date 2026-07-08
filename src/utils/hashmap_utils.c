#include "hashmap_utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll_utils.h"
#include "string_view_utils.h"

// keep this as power of two
// for faster hash calculations
#define HASHMAP_CAPACITY 64

HashMap newHashmap() {
    HashMap hashmap = {.arr = malloc(HASHMAP_CAPACITY * sizeof(LL_Node *)),
                       .count = 0,
                       .capacity = HASHMAP_CAPACITY};

    for (size_t i = 0; i < HASHMAP_CAPACITY; i++) {
        hashmap.arr[i] = NULL;
    }

    if (hashmap.arr == NULL) {
        fprintf(stderr, "Error: Could not allocate hashmap arr\n");
        exit(1);
    }
    return hashmap;
}

void insertVar(HashMap *hashMap, StringView *stringView, double value) {
    size_t bucketIndex = djb2(stringView->arr, stringView->length);

    LL_Node newNode = newLL_Node(stringView->arr, stringView->length, value);

    if (hashMap->arr[bucketIndex] == NULL) {
        hashMap->arr[bucketIndex] = &newNode;
    } else {
        newNode.next = hashMap->arr[bucketIndex];
        hashMap->arr[bucketIndex] = &newNode;
    }

    return;
}

// func returns status code,
// result is stored at *result
int lookupVar(HashMap *hashMap, StringView *stringView, double *result) {
    size_t bucketIndex = djb2(stringView->arr, stringView->length);
    LL_Node *node = hashMap->arr[bucketIndex];

    while (node != NULL) {
        if (compareViews(&node->key, stringView)) {
            *result = node->value;
            return 0;
        } else {
            node = node->next;
        }
    }

    return -1;
}

size_t djb2(char *str, size_t str_length) {
    size_t hash = 5381;
    int c;

    for (int i = 0; i < str_length; i++) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash & (HASHMAP_CAPACITY - 1);
}
