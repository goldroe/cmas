#include "common.h"
#include "tok.h"
#include "parse.h"

#include <stdlib.h>
#include <string.h>

Expr *expr_init(int kind) {
    Expr *expr = malloc(sizeof(Expr));
    expr->kind = kind;
    return expr;
}

Expr *expr_unary(Expr *expr, int op) {
    Expr *result = expr_init(EXPR_UNARY);
    result->unary.expr = expr;
    result->unary.op = op;
    return expr;
}

Expr *expr_binary(Expr *left, Expr *right, int op) {
    Expr *result = expr_init(EXPR_BINARY);
    result->binary.left = left;
    result->binary.right = right;
    result->binary.op = op;
    return result;
}

Expr *expr_ternary(Expr *cond, Expr *left, Expr *right) {
    Expr *result = expr_init(EXPR_TERNARY);
    result->ternary.cond = cond;
    result->ternary.left = left;
    result->ternary.right = right;
    return result;
}

Expr *expr_int(int64_t val) {
    Expr *expr = expr_init(EXPR_INT);
    expr->int_val = val;
    return expr;
}

Expr *expr_float(double val) {
    Expr *expr = expr_init(EXPR_FLOAT);
    expr->float_val = val;
    return expr;
}


Expr *expr_str(char *val) {
    Expr *expr = expr_init(EXPR_STR);
    expr->str_val = val;
    return expr;
}

Expr *expr_ident(char *val) {
    Expr *expr = expr_init(EXPR_IDENT);
    expr->ident = val;
    return expr;
}

Expr *parse_expr_operand() {
    Expr *expr = NULL;
    if (is_token(TOKEN_INT)) {
        expr = expr_int(token.int_val);
    } else if (is_token(TOKEN_FLOAT)) {
        expr = expr_float(token.float_val);
    } else if (is_token(TOKEN_STR)) {
        expr = expr_str(token.str_val);
    } else if (is_token(TOKEN_IDENT)) {
        expr = expr_ident(token.ident_val);
    } else {
        return expr_init(EXPR_NONE);
    }
    
    next_token();
    return expr;
}

Expr *parse_expr_base() {
    Expr *expr = parse_expr_operand();
    if (match_token(TOKEN_LPAREN)) {
        ASSERT(expr->kind == EXPR_IDENT);
    } else if (match_token(TOKEN_LBRACKET)) {
        
    }
    return expr;
}

Expr *parse_expr_unary() {
    Expr *expr = NULL;
    int op = token.kind;
    if (is_unary_op(op)) {
        next_token();
        expr = parse_expr_unary();
        expr = expr_unary(expr, op);
        op = token.kind;
    } else {
        expr = parse_expr_base();
    }
    return expr;
}

Expr *parse_expr_mul() {
    Expr *expr = parse_expr_unary();
    int op = token.kind;
    while (is_mul_op(op)) {
        next_token();
        Expr *right = parse_expr_unary();
        expr = expr_binary(expr, right, op);
        op = token.kind;
    }
    return expr;
}

Expr *parse_expr_add() {
    Expr *expr = parse_expr_mul();
    int op = token.kind;
    while (is_add_op(op)) {
        next_token();
        Expr *right = parse_expr_mul();
        expr = expr_binary(expr, right, op);
        op = token.kind;
    }
    return expr;
}

Expr *parse_expr_cmp() {
    Expr *expr = parse_expr_add();
    int op = token.kind;
    while (is_cmp_op(op)) {
        next_token();
        Expr *right = parse_expr_add();
        expr = expr_binary(expr, right, op);
    }
    return expr;
}

Expr *parse_expr_and() {
    Expr *expr = parse_expr_cmp();
    while (match_token(TOKEN_AND)) {
        Expr *right = parse_expr_cmp();
        expr = expr_binary(expr, right, TOKEN_AND);
    }
    
    return expr;
}

Expr *parse_expr_or() {
    Expr *expr = parse_expr_and();
    while (match_token(TOKEN_OR)) {
        Expr *right = parse_expr_and();
        expr = expr_binary(expr, right, TOKEN_OR);
    }
    
    return expr;
}

Expr *parse_expr_ternary() {
    Expr *expr = parse_expr_or();
    if (match_token(TOKEN_QUESTION)) {
        Expr *left = parse_expr_ternary();
        expect_token(TOKEN_COLON);
        Expr *right = parse_expr_ternary();
        expr = expr_ternary(expr, left, right);
    }
    
    return expr;
}

Expr *parse_expr() {
    Expr *expr = parse_expr_ternary();
    return expr;
}

Expr *parse_expr_stream(char *str) {
    init_stream(str);
    Expr *expr = parse_expr();
    return expr;
}

Typespec *typespec_init(Typespec_Kind kind) {
    Typespec *type = calloc(1, sizeof(Typespec));
    type->kind = kind;
    return type;
}

Typespec *parse_type() {
    Typespec *type = NULL;
    if (match_token(TOKEN_MULT)) {
        type = typespec_init(TYPESPEC_PTR);
        while (match_token(TOKEN_MULT)) {
            Typespec *t = typespec_init(TYPESPEC_PTR);
            t->sub = type;
            type = t;
        }
        while (match_token(TOKEN_LBRACKET)) {
            // Expr *expr = parse_expr();
            expect_token(TOKEN_RBRACKET);
            Typespec *t = typespec_init(TYPESPEC_ARRAY);
            t->sub = type;
            type = t;
        }
    } else if (match_token(TOKEN_LBRACKET)) {
        // Expr *expr = parse_expr();
        expect_token(TOKEN_RBRACKET);
        type = typespec_init(TYPESPEC_ARRAY);
        while (match_token(TOKEN_LBRACKET)) {
            // Expr *expr = parse_expr();
            expect_token(TOKEN_RBRACKET);
            Typespec *t = typespec_init(TYPESPEC_ARRAY);
            t->sub = type;
            type = t;
        }
        while (match_token(TOKEN_MULT)) {
            Typespec *t = typespec_init(TYPESPEC_PTR);
            t->sub = type;
            type = t;
        }
    }

    char *ident = token.ident_val;
    expect_token(TOKEN_IDENT);
    Typespec *t = typespec_init(TYPESPEC_IDENT);
    t->name = ident;
    t->sub = type;
    type = t;
    return type;
}

Param **parse_param_list() {
    Param **params = NULL;

    for (;;) {
        Param *p = malloc(sizeof(Param));
        p->ident = token.ident_val;
        expect_token(TOKEN_IDENT);

        expect_token(TOKEN_COLON);
        
        Typespec *type = parse_type();
        
        p->type = type;
        buf_push(params, p);
        
        if (!match_token(TOKEN_COMMA)) {
            break;
        }
    }
    return params;
}

Typespec **parse_type_list() {
    Typespec **types = NULL;
    for (;;) {
        Typespec *t = parse_type();
        expect_token(TOKEN_COMMA);
        buf_push(types, t);

        if (!match_token(TOKEN_COMMA)) {
            break;
        }
    }
    return types;
}

Decl *decl_init(Decl_Kind kind) {
    Decl *decl = malloc(sizeof(Decl));
    decl->kind = kind;
    return decl;
}

Decl *parse_decl() {
    Decl *decl = NULL;
    float f = .0;

    expect_token(TOKEN_IDENT);
    char *ident = token.ident_val;
    next_token();

    if (match_token(TOKEN_COLON2)) {
        if (strcmp(token.ident_val, "struct") == 0) {
            
        } else if (strcmp(token.ident_val, "enum") == 0) {
            
        } else if (strcmp(token.ident_val, "proc") == 0) {
            // function header
            next_token();
            expect_token(TOKEN_LPAREN);
            Param **params = parse_param_list();
            expect_token(TOKEN_RPAREN);
            expect_token(TOKEN_ARROW);
            Typespec **ret_types = parse_type_list();
            // function body
            Stmt_Block *block = parse_stmt_block();
            decl = decl_init(DECL_PROC);
            decl->proc.params = params;
            decl->proc.return_types = ret_types;
            decl->proc.block = block;
        }
    }

    return decl;
}

Decl *parse_decl_stream(char *str) {
    init_stream(str);
    Decl *decl = parse_decl();
    return decl;
}

Stmt *parse_stmt() {
    Stmt *stmt = NULL;
    return stmt;
}

Stmt_Block *parse_stmt_block() {
    Stmt_Block *block = malloc(sizeof(Stmt_Block));
    block->statements = NULL;
    
    expect_token(TOKEN_LBRACE);
    while (!is_token(TOKEN_EOF)) {
        if (is_token(TOKEN_RBRACE)) {
            break;
        }

        Stmt *stmt = parse_stmt();
        buf_push(block->statements, stmt);
    }

    expect_token(TOKEN_RBRACE);
    
    return block;
}

