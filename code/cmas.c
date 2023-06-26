#include "common.h"
#include "tok.h"
#include "parse.h"

void main(int argc, char **argv) {
    Expr *expr = parse_expr_stream("10 + 20 * 5.0f - foo");
    Decl *decl =  parse_decl_stream("foo :: proc(x :int, y :int) -> int, float { }");
}
