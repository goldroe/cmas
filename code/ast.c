#include <stdlib.h>
#include "parse.h"

Typespec *typespec_init(int kind) {
    Typespec *type = calloc(1, sizeof(Typespec));
    type->kind = kind;
    return type;
}

Typespec *typespec_array(Expr *e) {
    Typespec *type = typespec_init(Typespec_Array);
    type->expr = e;
    return type;
}

Expr *expr_init(int kind) {
    Expr *expr = malloc(sizeof(Expr));
    expr->kind = kind;
    return expr;
}

Expr *expr_unary(Expr *expr, int op) {
    Expr *result = expr_init(Expr_Unary);
    result->unary.expr = expr;
    result->unary.op = op;
    return expr;
}

Expr *expr_binary(Expr *left, Expr *right, int op) {
    Expr *result = expr_init(Expr_Binary);
    result->binary.left = left;
    result->binary.right = right;
    result->binary.op = op;
    return result;
}

Expr *expr_ternary(Expr *cond, Expr *left, Expr *right) {
    Expr *result = expr_init(Expr_Ternary);
    result->ternary.cond = cond;
    result->ternary.left = left;
    result->ternary.right = right;
    return result;
}

Expr *expr_int(int64_t val) {
    Expr *expr = expr_init(Expr_Int);
    expr->int_val = val;
    return expr;
}

Expr *expr_float(double val) {
    Expr *expr = expr_init(Expr_Float);
    expr->float_val = val;
    return expr;
}


Expr *expr_str(char *val) {
    Expr *expr = expr_init(Expr_Str);
    expr->str_val = val;
    return expr;
}

Expr *expr_ident(char *val) {
    Expr *expr = expr_init(Expr_Ident);
    expr->ident = val;
    return expr;
}

Decl *decl_init(int kind) {
    Decl *decl = malloc(sizeof(Decl));
    decl->kind = kind;
    return decl;
}

Decl *decl_proc(char *ident, Param **params, Typespec **types, StmtBlock *block) {
    Decl *decl = decl_init(Decl_Proc);
    decl->ident = ident;
    decl->proc.params = params;
    decl->proc.return_types = types;
    decl->proc.block = block;
    return decl;
}

Decl *decl_var(char *ident, Typespec *t) {
    Decl *decl = decl_init(Decl_Var);
    decl->ident = ident;
    decl->var.type = t;
    return decl;
}
