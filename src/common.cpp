#include "common.h"

#include <stdarg.h>
#include <intrin.h>
#include <stdlib.h>
#include <stdio.h>

void syntax_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    printf("syntax error: ");
    vprintf(fmt, args);
    printf("\n");

    va_end(args);

#ifdef DEBUG
    __debugbreak();
#endif

    exit(EXIT_FAILURE);
}