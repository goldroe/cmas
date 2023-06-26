#ifndef AST_H
#define AST_H

Typespec *typespec_init(int kind);

Expr *expr_init(int kind);
Expr *expr_unary(Expr *expr, int op);
Expr *expr_binary(Expr *left, Expr *right, int op);
Expr *expr_ternary(Expr *cond, Expr *left, Expr *right);
Expr *expr_int(int64_t val);
Expr *expr_float(double val);
Expr *expr_str(char *val);
Expr *expr_ident(char *val);

Decl *decl_init(int kind);
Decl *decl_proc(char *ident, Param **params, Typespec **types, StmtBlock *block);
Decl *decl_var(char *ident, Typespec *t);


#endif // AST_H
