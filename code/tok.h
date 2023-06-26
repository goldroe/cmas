#ifndef TOK_H
#define TOK_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int kind;

    union {
        int64_t int_val;
        double float_val;
        char *str_val;
        char *ident_val;
    };
} Token;

extern Token token;
extern char *stream;

enum TokenKind {
    Token_EOF,

    Token_Ident = 128,
    Token_Int,
    Token_Float,
    Token_Str,

    Token_Eq,
    Token_Neq,
    Token_Lt,
    Token_Gt,
    Token_Lteq,
    Token_Gteq,
    
    Token_Lparen,
    Token_Rparen,
    Token_Lbrace,
    Token_Rbrace,
    Token_Lbracket,
    Token_Rbracket,

    Token_Lshift,
    Token_Rshift,
    Token_Plus,
    Token_Minus,
    Token_Div,
    Token_Mult,
    Token_Mod,

    Token_Inc,
    Token_Dec,
    
    Token_Bang,
    Token_Amper,
    Token_And,
    Token_Bar,
    Token_Or,
    Token_Xor,
    
    Token_Assign,
    Token_AutoAssign,
    Token_PlusAssign,
    Token_MinusAssign,
    Token_MultAssign,
    Token_DivAssign,
    Token_LshiftAssign,
    Token_RshiftAssign,
    Token_ModAssign,
    Token_AndAssign,
    Token_OrAssign,
    Token_XorAssign,

    Token_Semi,
    Token_Colon,
    Token_Colon2,
    Token_Dot,
    Token_Comma,
    Token_Ellipsis,
    Token_Arrow,
    Token_Hash,
    Token_Question,
};

inline bool is_cmp_op(int kind) {
    return (kind == Token_Eq || kind == Token_Neq || kind == Token_Lt || kind == Token_Gt || kind == Token_Lteq || kind == Token_Gteq);
}

inline bool is_add_op(int kind) {
    return (kind == Token_Plus || kind == Token_Minus || kind == Token_Xor || kind == Token_Bar);
}

inline bool is_mul_op(int kind) {
    return (kind == Token_Mult || kind == Token_Div || kind == Token_Mod || kind == Token_Amper || kind == Token_Lshift || kind == Token_Rshift);
}

inline bool is_unary_op(int op) {
    return (op == Token_Plus || op == Token_Minus || op == Token_Amper || op == Token_Mult);
}

void init_stream(char *s);
void next_token();
bool is_token(int kind);
bool match_token(int kind);
void expect_token(int kind);


#endif // TOK_H
