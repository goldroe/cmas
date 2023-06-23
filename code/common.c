#include "common.h"

#include <stdlib.h>

void *buf_grow(void *buf, size_t num, size_t elem_size) {
    BufHeader *head = NULL;
    size_t cap = 0;
    if (buf == NULL) {
        head = malloc(sizeof(BufHeader) + num * elem_size);
        head->len = 0;
        head->cap = num * elem_size;
    } else {
        head = _buf_header(buf);
        cap = head->cap;
        size_t new_cap = 0;
        while (new_cap <= cap + num * elem_size) {
            new_cap = 2 * new_cap + 1;
        }
        head = realloc(head, new_cap + sizeof(BufHeader));
        head->cap = new_cap;
    }

    return ((BufHeader *)(head) + 1);
}

