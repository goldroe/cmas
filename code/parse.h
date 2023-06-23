#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>
enum Expr_Kind {
    EXPR_NONE,

    EXPR_INT,
    EXPR_FLOAT,
    EXPR_STR,
    EXPR_IDENT,
    
    EXPR_CALL,
    EXPR_INDEX,
    EXPR_FIELD,
    EXPR_COMPOUND,
    EXPR_CAST,

    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_TERNARY,
};

typedef struct Expr Expr;

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

typedef struct Typespec Typespec;
typedef enum Typespec_Kind Typespec_Kind;
typedef struct Param Param;
typedef struct Aggregate_Field Aggregate_Field;
typedef struct Enum_Field Enum_Field;

enum Typespec_Kind {
    TYPESPEC_NONE,

    TYPESPEC_IDENT,
    
    TYPESPEC_ARRAY,
    TYPESPEC_PTR,
    
    TYPESPEC_STRUCT,
    TYPESPEC_ENUM,
    TYPESPEC_PROC
};

struct Typespec {
    Typespec_Kind kind;
    char *name;

    Typespec *sub;
};

struct Param {
    Typespec *type;
    char *ident;
};

struct Aggregate_Field {
    Typespec *type;
    char *ident;
    // initializer?
};

struct Enum_Field {
    char *ident;
    int64_t val;
};

typedef enum Stmt_Kind Stmt_Kind;    
typedef struct Stmt Stmt;
typedef struct Stmt_Block Stmt_Block;

enum Stmt_Kind {
    STMT_NONE,

    STMT_ASSIGN,
    STMT_EXPR,
    
    STMT_IF,
    STMT_DO,
    STMT_WHILE,
    STMT_SWITCH,
    
    STMT_BLOCK,
};

struct Stmt {
    Stmt_Kind kind;

    union {
        Expr *expr;
        
        struct {
            Expr *left;
            Expr *right;
        } assign;
    };
};

struct Stmt_Block {
    Stmt **statements;
};

typedef struct Decl Decl;
typedef enum Decl_Kind Decl_Kind;

enum Decl_Kind {
    DECL_NONE,

    DECL_VAR,
    DECL_PROC,
    DECL_STRUCT,
    DECL_ENUM,
};

struct Decl {
    Decl_Kind kind;

    union {
        struct {
            Typespec *type;
            char *ident;
        } var;
        struct {
            Param **params;
            Typespec **return_types;
            Stmt_Block *block;
        } proc;
        struct {
            Aggregate_Field **fields;
        } struct_decl;
        struct {
            Enum_Field **fields;
        } enum_decl;
    };
};


Expr *parse_expr_stream(char *str);
Expr *parse_expr();
Typespec *parse_type();
Decl *parse_decl();
Decl *parse_decl_stream(char *str);
Stmt *parse_stmt();
Stmt_Block *parse_stmt_block();

#endif // PARSE_H
