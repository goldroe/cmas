#ifndef TOK_H
#define TOK_H

#include <stdbool.h>
#include <stdint.h>

enum Token_Kind {
    TOKEN_EOF,

    TOKEN_IDENT = 128,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STR,

    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LTEQ,
    TOKEN_GTEQ,
    
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,

    TOKEN_LSHIFT,
    TOKEN_RSHIFT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DIV,
    TOKEN_MULT,
    TOKEN_MOD,

    TOKEN_INC,
    TOKEN_DEC,
    
    TOKEN_BANG,
    TOKEN_AMPER,
    TOKEN_AND,
    TOKEN_BAR,
    TOKEN_OR,
    TOKEN_XOR,
    
    TOKEN_ASSIGN,
    TOKEN_AUTO_ASSIGN,
    TOKEN_PLUS_ASSIGN,
    TOKEN_MINUS_ASSIGN,
    TOKEN_MULT_ASSIGN,
    TOKEN_DIV_ASSIGN,
    TOKEN_LSHIFT_ASSIGN,
    TOKEN_RSHIFT_ASSIGN,
    TOKEN_MOD_ASSIGN,
    TOKEN_AND_ASSIGN,
    TOKEN_OR_ASSIGN,
    TOKEN_XOR_ASSIGN,

    TOKEN_SEMI,
    TOKEN_COLON,
    TOKEN_COLON2,
    TOKEN_DOT,
    TOKEN_COMMA,
    TOKEN_ELLIPSIS,
    TOKEN_ARROW,
    TOKEN_HASH,
    TOKEN_QUESTION,
};

typedef struct {
    int kind;

    union {
        int64_t int_val;
        double float_val;
        char *str_val;
        char *ident_val;
    };
} Token;

extern char *stream;
extern Token token;

inline bool is_cmp_op(int kind) {
    return (kind == TOKEN_EQ || kind == TOKEN_NEQ || kind == TOKEN_LT || kind == TOKEN_GT || kind == TOKEN_LTEQ || kind == TOKEN_GTEQ);
}

inline bool is_add_op(int kind) {
    return (kind == TOKEN_PLUS || kind == TOKEN_MINUS || kind == TOKEN_XOR || kind == TOKEN_BAR);
}

inline bool is_mul_op(int kind) {
    return (kind == TOKEN_MULT || kind == TOKEN_DIV || kind == TOKEN_MOD || kind == TOKEN_AMPER || kind == TOKEN_LSHIFT || kind == TOKEN_RSHIFT);
}

inline bool is_unary_op(int op) {
    return (op == TOKEN_PLUS || op == TOKEN_MINUS || op == TOKEN_AMPER || op == TOKEN_MULT);
}

void init_stream(char *s);
void next_token();
bool is_token(int kind);
bool match_token(int kind);
void expect_token(int kind);


#endif // TOK_H
