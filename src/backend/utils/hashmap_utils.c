#include "hashmap_utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll_utils.h"
#include "num_utils.h"
#include "string_view_utils.h"

// keep this as power of two
// for faster hash calculations
#define HASHMAP_CAPACITY 32

// TODO: make hashmap automatically resize
void initHashmap(HashMap* map) {
    map->arr = malloc(HASHMAP_CAPACITY * sizeof(LL_Node*));
    map->count = 0;
    map->capacity = HASHMAP_CAPACITY;

    for (size_t i = 0; i < HASHMAP_CAPACITY; i++) {
        map->arr[i] = NULL;
    }

    if (map->arr == NULL) {
        fprintf(stderr, "Error: Could not allocate hashmap arr\n");
        exit(1);
    }
    return;
}

void insertVar(HashMap* hashMap, const StringView* stringView,
               const Number* value) {
    size_t bucketIndex = djb2(stringView->arr, stringView->length);

    LL_Node* newNode = newLL_Node(stringView->arr, stringView->length, value);

    if (hashMap->arr[bucketIndex] == NULL) {
        hashMap->arr[bucketIndex] = newNode;
    } else {
        newNode->next = hashMap->arr[bucketIndex];
        hashMap->arr[bucketIndex] = newNode;
    }

    hashMap->count++;
    return;
}

// func returns status code,
// TODO: result is allocated and stored at *result
int lookupVar(HashMap* hashMap, const StringView* stringView, Number* result) {
    size_t bucketIndex = djb2(stringView->arr, stringView->length);
    LL_Node* node = hashMap->arr[bucketIndex];

    while (node != NULL) {
        if (compareViews(&node->key, stringView)) {
            result->kind = node->value.kind;
            numClear(result);
            numInit(result, node->value.kind);
            fprintf(stderr,
                    "not: numSet received dest(kind: %d) and src(kind: %d)) of "
                    "different kinds\n",
                    result->kind, node->value.kind);
            numSet(result, &node->value);
            return 0;

        } else {
            node = node->next;
        }
    }

    return -1;
}

void deleteVar(HashMap* hashMap, const StringView* stringView) {
    size_t bucketIndex = djb2(stringView->arr, stringView->length);
    LL_Node* prevNode = NULL;
    LL_Node* curNode = hashMap->arr[bucketIndex];

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

void freeHashmap(HashMap* hashMap) {
    LL_Node* curNode = NULL;
    LL_Node* nextNode = NULL;

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

size_t djb2(char* str, size_t str_length) {
    size_t hash = 5381;

    for (size_t i = 0; i < str_length; i++) {
        hash = ((hash << 5) + hash) + str[i];
    }

    return hash & (HASHMAP_CAPACITY - 1);
}
