#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "common.h"

char *token_kind_str(TokenKind kind) {
    return token_strings[kind].text;
}

void ast_print(int t, Ast *node) {
    switch (node->kind) {
    case Ast_Ident:
        printf("%s", node->ident.token.ident_val.text);
        break;
    case Ast_Constant:
        switch (node->constant.value.kind) {
        case Token_Ident:
            printf("%s", node->constant.value.ident_val.text);
            break;
        case Token_Int:
            printf("%lld", node->constant.value.int_val);
            break;
        case Token_Float:
            printf("%f", node->constant.value.float_val);
            break;
        case Token_Str:
            printf("\"%s\"", node->constant.value.str_val.text);
            break;
        }
        break;
    case Ast_UnaryExpr:
        printf("%s", token_kind_str(node->unary_expr.op.kind));
        ast_print(t, node->unary_expr.expr);
        break;
    case Ast_BinaryExpr:
        ast_print(t, node->binary_expr.left);
        printf(" %s ", token_kind_str(node->binary_expr.op.kind));
        ast_print(t, node->binary_expr.right);
        break;
    case Ast_AssignStmt:
        printf("%*s", t, "");
        ast_print(t, node->assign_stmt.lvalue);
        printf(" = ");
        ast_print(t, node->assign_stmt.rvalue);
        printf(";\n");
        break;
    case Ast_CompoundStmt:
        printf(" {%*s\n", t, "");
        for (int i = 0; i < node->compound_stmt.statements.length; i++) {
            ast_print(t, node->compound_stmt.statements[i]);
        }
        printf("%*s}\n", t-2, "");
        break;
    case Ast_IfStmt:
        printf("%*sif ", t, "");
        ast_print(t, node->if_stmt.conditions[0]);
        ast_print(t+2, node->if_stmt.blocks[0]);
        for (int i = 1; i < node->if_stmt.conditions.length; i++) {
            printf("%*selse if ", t, "");
            ast_print(t, node->if_stmt.conditions[i]);
            ast_print(t+2, node->if_stmt.blocks[i]);
        }
        if (node->if_stmt.else_block) {
            printf("else ");
            ast_print(t, node->if_stmt.else_block);
        }
        break;
    case Ast_WhileStmt:
        printf("%*swhile ", t, "");
        ast_print(t+2, node->while_stmt.condition);
        ast_print(t+2, node->while_stmt.block);
        break;
    case Ast_ReturnStmt:
        printf("%*sreturn ", t, "");
        ast_print(t, node->return_stmt.expr);
        printf(";\n");
        break;
    case Ast_ProcDecl:
        printf("%*s%s :: proc(", t, "", node->proc_decl.ident->ident.token.ident_val.text);
        for (int i = 0; i < node->proc_decl.params.length; i++) {
            printf("%s: %s,", node->proc_decl.params[i]->ident.text, node->proc_decl.params[i]->type->name.text);
        }
        printf(") -> ");
        for (int i = 0; i < node->proc_decl.return_types.length; i++) {
            printf("%s, ", node->proc_decl.return_types[i]->name.text);
        }
        ast_print(t+2, node->proc_decl.block);
        break;
    }
    return;
}

Ast *ast_init(AstKind kind) {
    Ast *node = (Ast *)malloc(sizeof(Ast));
    memset(node, 0, sizeof(*node));
    node->kind = kind;
    return node;
}

Param *param_init(String ident, Typespec *type) {
    Param *param = (Param *)malloc(sizeof(Param));
    memset(param, 0, sizeof(*param));
    param->ident = ident;
    param->type = type;
    return param;
}

Typespec *typespec_init(TypespecKind kind) {
    Typespec *type = (Typespec *)calloc(1, sizeof(Typespec));
    type->kind = kind;
    return type;
}

Ast *ast_ident(Token tok) {
    Ast *node = ast_init(Ast_Ident);
    node->ident.token = tok;
    return node;
}

Ast *ast_constant(Token tok) {
    Ast *node = ast_init(Ast_Constant);
    node->constant.value = tok;
    return node;
}

Ast *ast_expr_unary(Ast *expr, Token op) {
    Ast *node = ast_init(Ast_UnaryExpr);
    node->unary_expr.expr = expr;
    node->unary_expr.op = op;
    return node;
}

Ast *ast_expr_binary(Ast *left, Ast *right, Token op) {
    Ast *node = ast_init(Ast_BinaryExpr);
    node->binary_expr.left = left;
    node->binary_expr.right = right;
    node->binary_expr.op = op;
    return node;
}

Ast *ast_expr_ternary(Ast *cond, Ast *left, Ast *right) {
    Ast *node = ast_init(Ast_TernaryExpr);
    node->ternary_expr.cond = cond;
    node->ternary_expr.left = left;
    node->ternary_expr.right = right;
    return node;
}

Ast *ast_expr_call(Ast *ident, DArray<Ast *> args) {
    Ast *node = ast_init(Ast_CallExpr);
    node->call_expr.ident = ident;
    node->call_expr.args = args;
    return node;
}

Ast *ast_stmt_expr(Ast *expr) {
    Ast *node = ast_init(Ast_ExprStmt);
    node->expr_stmt.expr = expr;
    return node;
}

Ast *ast_stmt_assign(Ast *lvalue, Ast *rvalue) {
    Ast *node = ast_init(Ast_AssignStmt);
    node->assign_stmt.lvalue = lvalue;
    node->assign_stmt.rvalue = rvalue;
    return node;
}

Ast *ast_stmt_if(DArray<Ast*> conditions, DArray<Ast*> blocks, Ast *else_block) {
    Ast *node = ast_init(Ast_IfStmt);
    node->if_stmt.conditions = conditions;
    node->if_stmt.blocks = blocks;
    node->if_stmt.else_block = else_block;
    return node; 
}

Ast *ast_stmt_while(Ast *expr, Ast *block) {
    Ast *node = ast_init(Ast_WhileStmt);
    node->while_stmt.condition = expr;
    node->while_stmt.block = block;
    return node;
}

Ast *ast_stmt_for(Ast *init, Ast *cond, Ast *iterator, Ast *block) {
    Ast *node = ast_init(Ast_ForStmt);
    node->for_stmt.init = init;
    node->for_stmt.condition = cond;
    node->for_stmt.iterator = iterator;
    node->for_stmt.block = block;
    return node;
}

Ast *ast_stmt_compound(DArray<Ast*> stmts) {
    Ast *node = ast_init(Ast_CompoundStmt);
    node->compound_stmt.statements = stmts;
    return node;
}

Ast *ast_decl_var(Ast *ident, Typespec *type) {
    Ast *node = ast_init(Ast_VarDecl);
    node->var_decl.ident = ident;
    node->var_decl.type = type;
    return node;
}

Ast *ast_decl_proc(Ast *ident, DArray<Param*> params, DArray<Typespec*> return_types, Ast *block) {
    Ast *node = ast_init(Ast_ProcDecl);
    node->proc_decl.ident = ident;
    node->proc_decl.params = params;
    node->proc_decl.return_types = return_types;
    node->proc_decl.block = block;
    return node;
}

Ast *ast_decl_struct(Ast *ident, DArray<AggregateField*> fields) {
    Ast *node = ast_init(Ast_StructDecl);
    node->struct_decl.ident = ident;
    node->struct_decl.fields = fields;
    return node;
}

Ast *ast_decl_enum(Ast *ident, DArray<EnumField*> fields) {
    Ast *node = ast_init(Ast_EnumDecl);
    node->enum_decl.ident = ident;
    node->enum_decl.fields = fields;
    return node;
}

Typespec *parse_type() {
    Typespec *type = nullptr;

    if (match_token(Token_Mult)) {
        type = typespec_init(Typespec_Ptr);
        while (match_token(Token_Mult)) {
            Typespec *t = typespec_init(Typespec_Ptr);
            t->sub = type;
            type = t;
        }
    }

    if (is_token(Token_Lbracket)) {
        if (type == nullptr) {
            type = typespec_init(Typespec_Array);
        }
        
        while (match_token(Token_Lbracket)) {
            Ast *expr = parse_expr();
            expect_token(Token_Rbracket);
            Typespec *t = typespec_init(Typespec_Array);
            t->sub = type;
            type = t;
        }
    }

    // TODO: int, float, keyword parsing
    String ident = token.ident_val;
    expect_token(Token_Ident);
    Typespec *t = typespec_init(Typespec_Ident);
    t->name = ident;
    t->sub = type;
    type = t;
    return type;
}


Ast *parse_expr_operand() {
    if (is_token(Token_Int) || is_token(Token_Float) ||
        is_token(Token_Str) || is_token(Token_Ident)) {
        Ast *constant = ast_constant(token);
        next_token();
        return constant;
    }
    return nullptr;
}

Ast *parse_expr_base() {
    Ast *expr = parse_expr_operand();
    if (match_token(Token_Lparen)) {
        ASSERT(expr->kind == Ast_Ident);
    } else if (match_token(Token_Lbracket)) {
        
    }
        
    return expr;
}
    
Ast *parse_expr_unary() {
    Ast *expr = nullptr;
    Token op = token;
    if (is_unary_op(op.kind)) {
        next_token();
        expr = parse_expr_unary();
        expr = ast_expr_unary(expr, op);
    } else {
        expr = parse_expr_base();
    }

    return expr;
}

Ast *parse_expr_mul() {
    Ast *expr = parse_expr_unary();
    Token op = token;
    while (is_mul_op(op.kind)) {
        next_token();
        Ast *right = parse_expr_unary();
        expr = ast_expr_binary(expr, right, op);
        op = token;
    }

    return expr;
}

Ast *parse_expr_add() {
    Ast *expr = parse_expr_mul();
    Token op = token;
    while (is_add_op(op.kind)) {
        next_token();
        Ast *right = parse_expr_mul();
        expr = ast_expr_binary(expr, right, op);
        op = token;
    }
    return expr;
}

Ast *parse_expr_cmp() {
    Ast *expr = parse_expr_add();
    Token op = token;
    while (is_cmp_op(op.kind)) {
        next_token();
        Ast *right = parse_expr_add();
        expr = ast_expr_binary(expr, right, op);
        op = token;
    }
    return expr;
}

Ast *parse_expr_and() {
    Ast *expr = parse_expr_cmp();
    Token op = token;
    while (match_token(Token_And)) {
        Ast *right = parse_expr_cmp();
        expr = ast_expr_binary(expr, right, op);
    }
    return expr;
}

Ast *parse_expr_or() {
    Ast *expr = parse_expr_and();
    Token op = token;
    while (match_token(Token_Or)) {
        Ast *right = parse_expr_and();
        expr = ast_expr_binary(expr, right, op);
    }
    return expr;
}

Ast *parse_expr_ternary() {
    Ast *expr = parse_expr_or();
    if (match_token(Token_Question)) {
        Ast *left = parse_expr_ternary();
        expect_token(Token_Colon);
        Ast *right = parse_expr_ternary();
        expr = ast_expr_ternary(expr, left, right);
    }
    return expr;
}

Ast *parse_expr() {
    Ast *expr = parse_expr_ternary();
    return expr;
}

Ast *parse_expr_stream(char *str) {
    init_stream(str);
    Ast *expr = parse_expr();
    return expr;
}

Param *parse_param() {
    Param *param = nullptr;
    if (is_token(Token_Ident)) {
        String ident = token.ident_val;
        next_token();
        expect_token(Token_Colon);
        Typespec *type = parse_type();
        param = param_init(ident, type);
    } else {
        syntax_error("Expected identifier\n");
    }

    return param;
}

DArray<Param*> parse_param_list() {
    DArray<Param*> params;
    Param *param = parse_param();
    if (param != nullptr) {
        params.push(param);
    }
    
    while (match_token(Token_Comma)) {
        param = parse_param();
        params.push(param);
    }
    return params;
}

DArray<Typespec*> parse_type_list() {
    DArray<Typespec*> types;
    Typespec *t = parse_type();

    types.push(t);
    while (match_token(Token_Comma)) {
        t = parse_type();
        types.push(t);
    }
    return types;
}

AggregateField *aggregate_field(Token ident, Typespec *type) {
    AggregateField *field = (AggregateField *)malloc(sizeof(AggregateField));
    field->type = type;
    field->ident = ident;
    return field;
}

DArray<AggregateField*> parse_aggregate_field_list() {
    DArray<AggregateField*> result;
    while (!is_token(Token_EOF) &&
        !is_token(Token_Rbrace)) {
        Token ident = token;
        expect_token(Token_Ident);
        expect_token(Token_Colon);
        Typespec *type = parse_type();
        expect_token(Token_Semi);
        AggregateField *field = aggregate_field(ident, type);
        result.push(field);
    }
    return result;
}

EnumField *enum_field(Token ident) {
    EnumField *field = (EnumField *)malloc(sizeof(EnumField));
    field->ident = ident;
    field->val = 0;
    return field;
}

DArray<EnumField*> parse_enum_field_list() {
    DArray<EnumField*> result;
    Token ident = token;
    if (!match_token(Token_Ident)) {
        return result;
    }
    EnumField *field = enum_field(ident);
    result.push(field);
    
    while (match_token(Token_Comma)) {
        ident = token;
        if (!match_token(Token_Ident)) {
            break;
        }
        field = enum_field(ident);
        result.push(field);
    }

    return result;
}

Ast *parse_decl() {
    Ast *decl = nullptr;

    Token name = token;
    String ident = name.ident_val;
    expect_token(Token_Ident);

    if (match_token(Token_Colon2)) {
        if (match_token(Token_Proc)) {
            expect_token(Token_Lparen);
            DArray<Param*> params = parse_param_list();
            expect_token(Token_Rparen);
            expect_token(Token_Arrow);
            DArray<Typespec*> ret_types = parse_type_list();

            Ast *block = parse_compound_stmt();
            decl = ast_decl_proc(ast_ident(name), params, ret_types, block);
        } else if (match_token(Token_Struct)) {
            expect_token(Token_Lbrace);
            DArray<AggregateField*> struct_fields = parse_aggregate_field_list();
            expect_token(Token_Rbrace);
            decl = ast_decl_struct(ast_ident(name), struct_fields);
        } else if (match_token(Token_Enum)) {
            expect_token(Token_Lbrace);
            DArray<EnumField*> enum_fields = parse_enum_field_list();
            expect_token(Token_Rbrace);
            decl = ast_decl_enum(ast_ident(name), enum_fields);
        } else {
            syntax_error("Expected proc, struct, or enum keyword");
        }
    } else if (match_token(Token_Colon)) {
        Typespec *t = parse_type();
        expect_token(Token_Semi);
        decl = ast_decl_var(ast_ident(name), t);
    }
    
    return decl;
}

Ast *parse_decl_stream(char *str) {
    init_stream(str);
    Ast *decl = parse_decl();
    return decl;
}

Ast *parse_compound_stmt() {
    Ast *block = ast_init(Ast_CompoundStmt);
    expect_token(Token_Lbrace);
    while (!is_token(Token_EOF)) {
        if (is_token(Token_Rbrace)) {
            break;
        }

        Ast *stmt = parse_stmt();
        block->compound_stmt.statements.push(stmt);
    }
    expect_token(Token_Rbrace);
    return block;
}

Ast *parse_if_stmt() {
    Ast *result = nullptr;
    DArray<Ast*> conditions;
    DArray<Ast*> blocks;
    expect_token(Token_If);
    Ast *expr = parse_expr();
    Ast *block = parse_compound_stmt();
    conditions.push(expr);
    blocks.push(block);
    while (match_token(Token_Else)) {
        if (!match_token(Token_If)) {
            break;
        }
        expr = parse_expr();
        block = parse_compound_stmt();
        conditions.push(expr);
        blocks.push(block);
    }

    if (match_token(Token_If)) {
        block = parse_compound_stmt();
        result = ast_stmt_if(conditions, blocks, block);
    } else {
        result = ast_stmt_if(conditions, blocks, nullptr);
    }
    return result;
}

Ast *parse_while_stmt() {
    Ast *result = nullptr;
    expect_token(Token_While);
    Ast *expr = parse_expr();
    Ast *block = parse_compound_stmt();
    result = ast_stmt_while(expr, block);
    return result;
}

Ast *parse_return_stmt() {
    expect_token(Token_Return);
    Ast *expr = parse_expr();
    Ast *stmt = ast_init(Ast_ReturnStmt);
    stmt->return_stmt.expr = expr;
    expect_token(Token_Semi);
    return stmt;
}

Ast *parse_stmt() {
    int x = 0;
    Ast *stmt = nullptr;
    if (is_keyword()) {
        switch (token.kind) {
        case Token_If:
            return parse_if_stmt();
        case Token_While:
            return parse_while_stmt();
        case Token_Return:
            return parse_return_stmt();
        }
    } else if (is_token(Token_Ident)) {
        Token ident = token;
        next_token();
        if (match_token(Token_Assign)) {
            Ast *expr = parse_expr();
            stmt = ast_stmt_assign(ast_ident(ident), expr);
        } else if (match_token(Token_Colon)) {
            Typespec *type = parse_type();
            stmt = ast_decl_var(ast_ident(ident), type);
        } else if (match_token(Token_Colon2)) {
        } else {
            stream = ident.start; // @todo peek token 
            Ast *expr = parse_expr();
            stmt = ast_stmt_expr(expr);
        }
        expect_token(Token_Semi);
    }
    return stmt;
}
