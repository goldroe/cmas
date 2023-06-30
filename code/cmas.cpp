#include <windows.h>
#include "common.h"
#include "tok.h"
#include "parse.h"

#define COLTERM_IMPLEMENTATION
#include "colterm.h"
#include "log.h"

void syntax_error(const char *fmt, ...) {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, 256, fmt, args);
    ALERT("syntax error: %s", buffer);
    va_end(args);

#ifdef DEBUG
    __debugbreak();
#endif

    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    Ast *ast;
    ast = parse_expr_stream("10 + 20 * 5.0f - foo");
    ast = parse_decl_stream("buzz :: enum { FOO, BIZZ, BUZZ }");
    ast = parse_decl_stream("bizzbuzz :: struct { Foo: int; Bar: float; Flags: buzz; }");

    ast =  parse_decl_stream("foo :: proc(x :int, y :int) -> int { while y < 0 { y = y - 1; } x = 0; if x == 10 { x = 11; } return 10; }");
    ast_print(0, ast);
}
