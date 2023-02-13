#pragma once

#include <stdint.h>

enum Token_Kind {
    TOKEN_EOF,

    TOKEN_IDENT = 128,
    TOKEN_CHAR,
    TOKEN_STR,
    TOKEN_INT,
    TOKEN_FLOAT,

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_BACKTICK,

    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DIV,
    TOKEN_MUL,
    TOKEN_MOD,

    TOKEN_AND,
    TOKEN_OR,
    TOKEN_LSHIFT,
    TOKEN_RSHIFT,
    TOKEN_XOR,
    TOKEN_NOR,

    TOKEN_BAR,
    TOKEN_AMPER,
    TOKEN_BANG,
    TOKEN_DOT,
    TOKEN_COMMA,
    TOKEN_SEMI,
    TOKEN_COLON,
    TOKEN_DOUBLE_COLON,
    TOKEN_QUESTION,

    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LTEQ,
    TOKEN_GTEQ,
    TOKEN_EQ,
    TOKEN_NEQ,
    
    TOKEN_MUL_ASSIGN,
    TOKEN_ADD_ASSIGN,
    TOKEN_SUB_ASSIGN,
    TOKEN_DIV_ASSIGN,
    TOKEN_MOD_ASSIGN,
    TOKEN_AUTO_ASSIGN,
    TOKEN_LSHIFT_ASSIGN,
    TOKEN_RSHIFT_ASSIGN,
    TOKEN_OR_ASSIGN,
    TOKEN_XOR_ASSIGN,
    TOKEN_AND_ASSIGN,
    TOKEN_NOT_ASSIGN,
};

struct Token {
    Token_Kind kind;

    union {
        char char_lit;
        char *str_lit;
        char *ident;
        int64_t int_lit;
        double float_lit;
    };
};

extern char *stream;
extern Token token;

void next_token();