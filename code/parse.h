#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>

#include "darray.h"
#include "tok.h"

struct Ast;
struct Param;
struct Typespec;
struct AggregateField;
struct EnumField;

enum AstKind {
    Ast_None,
    
    Ast_Ident,
    Ast_Constant,

    Ast_UnaryExpr,
    Ast_BinaryExpr,
    Ast_TernaryExpr,
    Ast_CallExpr,
    Ast_IndexExpr,

    Ast_CompoundStmt,
    Ast_AssignStmt,
    Ast_ExprStmt,
    Ast_IfStmt,
    Ast_WhileStmt,
    Ast_ForStmt,
    Ast_ReturnStmt,

    Ast_VarDecl,
    Ast_ProcDecl,
    Ast_StructDecl,
    Ast_EnumDecl,
};

struct Ast {
    AstKind kind;

    union {
        struct {
            Token token;
        } ident;
        struct {
            Token value;
        } constant;
        struct {
            Ast *expr;
            Token op;
        } unary_expr;
        struct {
            Ast *left;
            Ast *right;
            Token op;
        } binary_expr;
        struct {
            Ast *cond;
            Ast *left;
            Ast *right;
        } ternary_expr;
        struct {
            Ast *ident;
            DArray<Ast*> args;
        } call_expr;

        struct {
            Ast *expr;
        } expr_stmt;
        struct {
            Ast *lvalue;
            Ast *rvalue;
        } assign_stmt;
        struct {
            DArray<Ast*> conditions;
            DArray<Ast*> blocks;
            Ast *else_block;
        } if_stmt;
        struct {
            Ast *init;
            Ast *condition;
            Ast *iterator;
            Ast *block;
        } for_stmt;
        struct {
            Ast *condition;
            Ast *block;
        } while_stmt;
        struct {
            DArray<Ast*> statements;
        } compound_stmt;
        struct {
            Ast *expr;
        } return_stmt;
        
        struct {
            Ast *ident;
            Typespec *type;
        } var_decl;
        struct {
            Ast *ident;
            DArray<Param*> params;
            DArray<Typespec*> return_types;
            Ast *block;
        } proc_decl;
        struct {
            Ast *ident;
            DArray<AggregateField*> fields;
        } struct_decl;
        struct {
            Ast *ident;
            DArray<EnumField*> fields;
        } enum_decl;
    };
};


/*
enum ExprKind {
    Expr_None,

    Expr_Int,
    Expr_Float,
    Expr_Str,
    Expr_Ident,
    
    Expr_Call,
    Expr_Index,
    Expr_Field,
    Expr_Compound,
    Expr_Cast,

    Expr_Unary,
    Expr_Binary,
    Expr_Ternary,
};
*/

enum TypespecKind {
    Typespec_None,

    Typespec_Ident,
    
    Typespec_Array,
    Typespec_Ptr,
    
    Typespec_Struct,
    Typespec_Enum,
    Typespec_Proc
};

struct Typespec {
    TypespecKind kind;
    String name;

    union {
        struct {
            Ast *expr;
        } array;
    };
    Typespec *sub;
};

struct Param {
    String ident;
    Typespec *type;
};

struct AggregateField {
    Token ident;
    Typespec *type;
    // initializer?
};

struct EnumField {
    Token ident;
    int64_t val;
};

Ast *parse_expr_stream(char *str);
Ast *parse_expr();
Ast *parse_decl();
Ast *parse_decl_stream(char *str);
Ast *parse_stmt();
Ast *parse_compound_stmt();

Typespec *typespec_init(TypespecKind kind);
Typespec *parse_type();

void ast_print(int t,Ast *root);
#endif // PARSE_H
