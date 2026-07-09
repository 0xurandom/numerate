#include "hashmap_utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll_utils.h"
#include "string_view_utils.h"

// keep this as power of two
// for faster hash calculations
#define HASHMAP_CAPACITY 32

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

    hashMap->count++;
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

void deleteVar(HashMap *hashMap, StringView *stringView) {
    size_t bucketIndex = djb2(stringView->arr, stringView->length);
    LL_Node *prevNode = NULL;
    LL_Node *curNode = hashMap->arr[bucketIndex];

    while (curNode != NULL) {
        if (compareViews(&curNode->key, stringView)) {
            prevNode->next = curNode->next;
            freeLL_Node(curNode);

            return;
        } else {
            prevNode = curNode;
            curNode = curNode->next;
        }
    }

    hashMap->count--;
    return;
}

void freeHashmap(HashMap *hashMap) {
    LL_Node *curNode = NULL;
    LL_Node *nextNode = NULL;

    for (size_t i = 0; i < hashMap->capacity; i++) {
        curNode = hashMap->arr[i];
        nextNode = curNode->next;

        while (curNode != NULL) {
            nextNode = curNode->next;
            freeLL_Node(curNode);
            curNode = nextNode;
        }
    }

    free(hashMap->arr);
    hashMap->count = 0;
    hashMap->capacity = 0;
}

size_t djb2(char *str, size_t str_length) {
    size_t hash = 5381;
    int c;

    for (int i = 0; i < str_length; i++) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash & (HASHMAP_CAPACITY - 1);
}
