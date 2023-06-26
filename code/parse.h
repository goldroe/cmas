#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>

typedef struct Expr Expr;
typedef struct Typespec Typespec;
typedef enum TypespecKind TypespecKind;
typedef struct Param Param;
typedef struct AggregateField AggregateField;
typedef struct EnumField EnumField;
typedef struct Stmt Stmt;
typedef struct StmtBlock StmtBlock;

struct Expr {
    int kind;

    union {
        int64_t int_val;
        double float_val;
        char *str_val;
        char *ident;
        
        struct {
            Expr *expr;
            int op;
        } unary;
        
        struct {
            Expr *left;
            Expr *right;
            int op;
        } binary;

        struct {
            Expr *cond;
            Expr *left;
            Expr *right;
        } ternary;

        struct {
            char *ident;
            Expr **args;
        } call;
    };
};

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

enum Typespec_Kind {
    Typespec_None,

    Typespec_Ident,
    
    Typespec_Array,
    Typespec_Ptr,
    
    Typespec_Struct,
    Typespec_Enum,
    Typespec_Proc
};

struct Typespec {
    int kind;
    char *name;

    Expr *expr;
    Typespec *sub;
};

struct Param {
    char *ident;
    Typespec *type;
};

struct AggregateField {
    Typespec *type;
    char *ident;
    // initializer?
};

struct EnumField {
    char *ident;
    int64_t val;
};
enum StmtKind {
    Stmt_None,

    Stmt_Assign,
    Stmt_Expr,
    
    Stmt_If,
    Stmt_Do,
    Stmt_While,
    Stmt_Switch,
    
    Stmt_Block,
};

struct Stmt {
    int kind;

    union {
        Expr *expr;
        
        struct {
            Expr *left;
            Expr *right;
        } assign;
    };
};

struct StmtBlock {
    Stmt **statements;
};

typedef struct Decl Decl;

enum Decl_Kind {
    Decl_None,

    Decl_Var,
    Decl_Proc,
    Decl_Struct,
    Decl_Enum,
};

struct Decl {
    int kind;
    char *ident;

    union {
        struct {
            Typespec *type;
        } var;
        struct {
            Param **params;
            Typespec **return_types;
            StmtBlock *block;
        } proc;
        struct {
            AggregateField **fields;
        } struct_decl;
        struct {
            EnumField **fields;
        } enum_decl;
    };
};

Expr *parse_expr_stream(char *str);
Expr *parse_expr();
Typespec *parse_type();
Decl *parse_decl();
Decl *parse_decl_stream(char *str);
Stmt *parse_stmt();
StmtBlock *parse_stmt_block();

#endif // PARSE_H
