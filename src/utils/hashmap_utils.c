#include "hashmap_utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll_utils.h"

#define HASHMAP_CAPACITY 10

HashMap newHashmap() {
    HashMap hashmap = {.arr = malloc(HASHMAP_CAPACITY * sizeof(LL_Node *)),
                       .count = 0,
                       .capacity = HASHMAP_CAPACITY};

    if (hashmap.arr == NULL) {
        fprintf(stderr, "Error: Could not allocate hashmap arr\n");
        exit(1);
    }
    return hashmap;
}

size_t djb2(char *str, size_t str_length) {
    size_t hash = 5381;
    int c;

    for (int i = 0; i < str_length; i++) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}
