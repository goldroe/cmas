#ifndef TOK_H
#define TOK_H

#include <stdint.h>

#include "string.h"
#include "darray.h"

enum TokenKind;
struct Token;

extern Token token;
extern char *stream;

struct Token {
    char *start;
    TokenKind kind;

    union {
        int64_t int_val;
        double float_val;
        String str_val;
        String ident_val;
    };

    Token() {
        kind = (TokenKind)0;
    }
};

#define TOKENS() \
    TOK(Token_EOF, ""), \
 \
    TOK(Token_ConstantBegin, "beginning of constant tokens"), \
    TOK(Token_Ident, "identifier"), \
    TOK(Token_Int,   "integer"), \
    TOK(Token_Float, "float"), \
    TOK(Token_Str,   "string"), \
    TOK(Token_ConstantEnd, "end of constant tokens"), \
 \
    TOK(Token_Eq,   "=="), \
    TOK(Token_Neq,  "!="), \
    TOK(Token_Lt,   "<"), \
    TOK(Token_Gt,   ">"), \
    TOK(Token_Lteq, "<="), \
    TOK(Token_Gteq, ">="), \
     \
    TOK(Token_Lparen,   "("), \
    TOK(Token_Rparen,   ")"), \
    TOK(Token_Lbrace,   "{"), \
    TOK(Token_Rbrace,   "}"), \
    TOK(Token_Lbracket, "["), \
    TOK(Token_Rbracket, "]"), \
    \
    TOK(Token_Lshift, "<<"), \
    TOK(Token_Rshift, ">>"), \
    TOK(Token_Plus,   "+"), \
    TOK(Token_Minus,  "-"), \
    TOK(Token_Div,    "/"), \
    TOK(Token_Mult,   "*"), \
    TOK(Token_Mod,    "%"), \
    \
    TOK(Token_Inc, "++"), \
    TOK(Token_Dec, "--"), \
     \
    TOK(Token_Bang, "!"), \
    TOK(Token_Amper, "&"), \
    TOK(Token_And, "&&"), \
    TOK(Token_Bar, "|"), \
    TOK(Token_Or, "||"), \
    TOK(Token_Xor, "^"), \
     \
    TOK(Token_Assign,       "="), \
    TOK(Token_AutoAssign,   ":="), \
    TOK(Token_PlusAssign,   "+="), \
    TOK(Token_MinusAssign,  "-="), \
    TOK(Token_MultAssign,   "*="), \
    TOK(Token_DivAssign,    "/="), \
    TOK(Token_LshiftAssign, "<<="), \
    TOK(Token_RshiftAssign, ">>="), \
    TOK(Token_ModAssign,    "%="), \
    TOK(Token_AndAssign,    "&="), \
    TOK(Token_OrAssign,     "|="), \
    TOK(Token_XorAssign,    "^="), \
    \
    TOK(Token_Semi,     ";"), \
    TOK(Token_Colon,    ":"), \
    TOK(Token_Colon2,   "::"), \
    TOK(Token_Dot,      "."), \
    TOK(Token_Comma,    ","), \
    TOK(Token_Ellipsis, ".."), \
    TOK(Token_Arrow,    "->"), \
    TOK(Token_Hash,     "#"), \
    TOK(Token_Question, "?"), \
    \
    TOK(Token_KeywordBegin, "beginning of keywords"), \
    TOK(Token_If,       "if"), \
    TOK(Token_Else,     "else"), \
    TOK(Token_While,    "while"), \
    TOK(Token_For,      "for"), \
    TOK(Token_Do,       "do"), \
    TOK(Token_Struct,   "struct"), \
    TOK(Token_Union,    "union"), \
    TOK(Token_Enum,     "enum"), \
    TOK(Token_Proc,     "proc"), \
    TOK(Token_Return,   "return"), \
    TOK(Token_Break,    "break"), \
    TOK(Token_Goto,     "goto"), \
    TOK(Token_Continue, "continue"), \
    TOK(Token_True,     "true"), \
    TOK(Token_False,    "false"), \
    TOK(Token_Include,  "include"), \
    TOK(Token_KeywordEnd, "end of keywords"), \
    
enum TokenKind {
#define TOK(id, str) id
    TOKENS()
#undef TOK
};

static String token_strings[] = {
#define TOK(id, str) str_make(str)
    TOKENS()
#undef TOK
};

inline String token_kind_string(TokenKind kind) {
    return token_strings[kind];
}

inline bool is_keyword() {
    return token.kind > Token_KeywordBegin && token.kind < Token_KeywordEnd;
}

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
bool is_token(TokenKind kind);
bool match_token(TokenKind kind);
void expect_token(TokenKind kind);
Token advance_token();

#endif // TOK_H
