#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "tok.h"
#include "parse.h"
#include "ast.h"

Expr *parse_expr_operand() {
    Expr *expr = NULL;
    if (is_token(Token_Int)) {
        expr = expr_int(token.int_val);
    } else if (is_token(Token_Float)) {
        expr = expr_float(token.float_val);
    } else if (is_token(Token_Str)) {
        expr = expr_str(token.str_val);
    } else if (is_token(Token_Ident)) {
        expr = expr_ident(token.ident_val);
    } else {
        return expr_init(Expr_None);
    }
    
    next_token();
    return expr;
}

Expr *parse_expr_base() {
    Expr *expr = parse_expr_operand();
    if (match_token(Token_Lparen)) {
        ASSERT(expr->kind == Expr_Ident);
    } else if (match_token(Token_Lbracket)) {
        
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
    while (match_token(Token_And)) {
        Expr *right = parse_expr_cmp();
        expr = expr_binary(expr, right, Token_And);
    }
    
    return expr;
}

Expr *parse_expr_or() {
    Expr *expr = parse_expr_and();
    while (match_token(Token_Or)) {
        Expr *right = parse_expr_and();
        expr = expr_binary(expr, right, Token_Or);
    }
    
    return expr;
}

Expr *parse_expr_ternary() {
    Expr *expr = parse_expr_or();
    if (match_token(Token_Question)) {
        Expr *left = parse_expr_ternary();
        expect_token(Token_Colon);
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

Typespec *parse_type() {
    Typespec *type = NULL;

    if (match_token(Token_Mult)) {
        type = typespec_init(Typespec_Ptr);
        while (match_token(Token_Mult)) {
            Typespec *t = typespec_init(Typespec_Ptr);
            t->sub = type;
            type = t;
        }
    }

    if (is_token(Token_Lbracket)) {
        if (type == NULL) {
            type = typespec_init(Typespec_Array);
        }
        
        while (match_token(Token_Lbracket)) {
            Expr *expr = parse_expr();
            expect_token(Token_Rbracket);
            Typespec *t = typespec_init(Typespec_Array);
            t->sub = type;
            type = t;
        }
    }

    // TODO: int, float, keyword parsing
    char *ident = token.ident_val;
    expect_token(Token_Ident);
    Typespec *t = typespec_init(Typespec_Ident);
    t->name = ident;
    t->sub = type;
    type = t;
    return type;
}

Param *param_init(char *ident, Typespec *type) {
    Param *param = malloc(sizeof(Param));
    param->ident = ident;
    param->type = type;
    return param;
}

Param *parse_param() {
    Param *p = NULL;
    if (is_token(Token_Ident)) {
        char *ident = token.ident_val;
        next_token();
        expect_token(Token_Colon);
        Typespec *type = parse_type();
        p = param_init(ident, type);
    } else {
        syntax_error("Expected identifier\n");
    }

    return p;
}

Param **parse_param_list() {
    Param **params = NULL;
    
    Param *p = parse_param();
    buf_push(params, p);
    
    while (match_token(Token_Comma)) {
        p = parse_param();
        buf_push(params, p);
    }
    return params;
}

Typespec **parse_type_list() {
    Typespec **types = NULL;
    Typespec *t = parse_type();
    buf_push(types, t);

    while (match_token(Token_Comma)) {
        t = parse_type();
        buf_push(types, t);
    }
    return types;
}

Decl *parse_decl() {
    Decl *decl = NULL;

    expect_token(Token_Ident);
    char *ident = token.ident_val;
    next_token();

    if (match_token(Token_Colon2)) {
        if (strcmp(token.ident_val, "proc") == 0) {
            next_token();
            expect_token(Token_Lparen);
            Param **params = parse_param_list();
            expect_token(Token_Rparen);
            expect_token(Token_Arrow);
            Typespec **ret_types = parse_type_list();

            StmtBlock *block = parse_stmt_block();
            decl = decl_proc(ident, params, ret_types, block);
        }
    } else if (match_token(Token_Colon)) {
        Typespec *t = parse_type();
        expect_token(Token_Semi);
        decl = decl_var(ident, t);
    }
    
    return decl;
}

Decl *parse_decl_stream(char *str) {
    init_stream(str);
    Decl *decl = parse_decl();
    return decl;
}

StmtBlock *parse_stmt_block() {
    StmtBlock *block = malloc(sizeof(StmtBlock));
    block->statements = NULL;
    
    expect_token(Token_Lbrace);
    while (!is_token(Token_EOF)) {
        if (is_token(Token_Rbrace)) {
            break;
        }

        Stmt *stmt = parse_stmt();
        buf_push(block->statements, stmt);
    }

    expect_token(Token_Rbrace);
    
    return block;
}

Stmt *parse_stmt() {
    Stmt *stmt = NULL;
    return stmt;
}
