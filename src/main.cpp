#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include "lex.h"

int main(int argc, char **argv) {
    stream = "0x1a";
    next_token();
    stream = "0B1111";
    next_token();
    stream = "02";
    next_token();
    stream = "'A'";
    next_token();
    stream = "'\\t'";
    next_token();
    stream = "\" This is a quote.\" ";
    next_token();
    stream = "10.0f";
    next_token();
    stream = "10.0";
    next_token();
    return 0;
}