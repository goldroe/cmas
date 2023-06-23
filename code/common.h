#ifndef COMMON_H
#define COMMON_H

#include <stdarg.h>
#include "log.h"

typedef struct {
    size_t len;
    size_t cap;
    char buf[1];
} BufHeader;

void *buf_grow(void *buf, size_t num, size_t elem_size);

#define _buf_header(B) ((BufHeader *)(B) - 1)

#define _buf_len(B) ((B) ? (_buf_header(B))->len : 0)
#define _buf_cap(B) ((B) ? (_buf_header(B))->cap : 0)


#define _buf_maybe_grow(B, X) ((_buf_len(B) + sizeof(*(B)) > _buf_cap(B)) ? (B) = buf_grow((B), 1, sizeof(*(B))) : 0)
#define buf_push(B, X) (_buf_maybe_grow(B, X), (B)[_buf_header(B)->len++] = (X))

void syntax_error(const char *fmt, ...);


#endif // COMMON_H
