#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <stdio.h>

#define __LOG(CLR, FMT, ...) printf("\033[%sm" FMT "\033[m", CLR, ##__VA_ARGS__)

#define __COLOR_RED "1;31"
#define __COLOR_GREEN "0;32"
#define __COLOR_BLUE "1;34"
#define __COLOR_CYAN "0;36"
#define __COLOR_YELLOW "0;33"
#define __COLOR_WHITE "0;37"

#define LOG(FMT, ...)     __LOG(__COLOR_WHITE,  FMT, ##__VA_ARGS__)
#define ALERT(FMT, ...)   __LOG(__COLOR_RED,    FMT, ##__VA_ARGS__)
#define WARNING(FMT, ...) __LOG(__COLOR_YELLOW, FMT, ##__VA_ARGS__)

#ifdef DEVELOPER
#define ASSERT(X) { \
   if (!(X)) {         \
       __debugbreak(); \
   }                   \
}
#else
#define ASSERT(X)
#endif

void syntax_error(const char *fmt, ...) {
    ALERT("syntax error: ");

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

#ifdef DEBUG
    __debugbreak();
#endif

    exit(EXIT_FAILURE);
}

typedef struct {
    size_t len;
    size_t cap;
    char buf[1];
} Buf_Header_T;

#define _buf_header(B) ((Buf_Header_T *)(B) - 1)

#define _buf_len(B) ((B) ? (_buf_header(B))->len : 0)
#define _buf_cap(B) ((B) ? (_buf_header(B))->cap : 0)

void *buf_grow(void *buf, size_t num, size_t elem_size) {
    Buf_Header_T *head = NULL;
    size_t cap = 0;
    if (buf == NULL) {
        head = malloc(sizeof(Buf_Header_T) + num * elem_size);
        head->len = 0;
        head->cap = num * elem_size;
    } else {
        head = _buf_header(buf);
        cap = head->cap;
        size_t new_cap = 0;
        while (new_cap <= cap + num * elem_size) {
            new_cap = 2 * new_cap + 1;
        }
        head = realloc(head, new_cap + sizeof(Buf_Header_T));
        head->cap = new_cap;
    }

    return ((Buf_Header_T *)(head) + 1);
}

#define _buf_maybe_grow(B, X) ((_buf_len(B) + sizeof(*(B)) > _buf_cap(B)) ? (B) = buf_grow((B), 1, sizeof(*(B))) : 0)
#define buf_push(B, X) (_buf_maybe_grow(B, X), (B)[_buf_header(B)->len++] = (X))


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
typedef enum Token_Kind Token_Kind;

struct Token {
    Token_Kind kind;

    union {
        int64_t int_val;
        double float_val;
        char *str_val;
        char *ident_val;
    };
};
typedef struct Token Token;

char *stream;
Token token;

void next_token();

bool is_token(Token_Kind kind) {
    return token.kind == kind;
}

bool match_token(Token_Kind kind) {
    if (token.kind == kind) {
        next_token();
        return true;
    } else {
        return false;
    }
}

void expect_token(Token_Kind kind) {
    if (token.kind == kind) {
        next_token();
    } else {
        syntax_error("expected token %d, got %d\n", kind, token.kind);
    }
}

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

typedef enum Expr_Kind Expr_Kind;

typedef struct Expr Expr;

struct Expr {
    Expr_Kind kind;

    union {
        int64_t int_val;
        double float_val;
        char *str_val;
        char *ident;
        
        struct {
            Expr *expr;
            Token_Kind op;
        } unary;
        
        struct {
            Expr *left;
            Expr *right;
            enum Token_Kind op;
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




int char_to_digit[256] = { ['0'] = 0, ['1'] = 1, ['2'] = 2, ['3'] = 3, ['4'] = 4, ['5'] = 5, ['6'] = 6, ['7'] = 7, ['8'] = 8, ['9'] = 9, ['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15};


double parse_float() {
    char *start = stream;
    
    while (isdigit(*stream)) {
        stream++;
    }
    if (*stream == '.') {
        stream++;
    } else {
        // TODO: invalid character in float
    }

    while (isdigit(*stream)) {
        stream++;
    }
    if (*stream == 'f') {
        stream++;
    }

    double result = strtod(start, NULL);
    return result;
}

int64_t parse_int() {
    uint64_t result = 0;
    uint64_t base = 10;
    if (*stream == '0') {
        stream++;
        if (toupper(*stream) == 'B') {
            stream++;
            base = 2;
        } else if (toupper(*stream) == 'X') {
            stream++;
            base = 16;
        } else {
            // TODO: validate edge case for invalid suffix
            base = 8;
        }
    }

    while (isalnum(*stream)) {
        int digit = char_to_digit[(int)toupper(*stream)];
        if (digit >= base) {
            // invalid digit in integer literal
        }
        result = result * base + digit;
        stream++;
    }
    return result;
}

char char_to_escape[256] = {['t'] = '\t', ['n'] = '\n', ['r'] = '\r', ['\\'] = '\\', ['\''] = '\'', ['"'] = '"'};

#define TOK1(CH, TOK)                           \
    case CH:                                    \
    token.kind = TOK;                           \
    stream++;                                   \
    break;

#define TOK2(CH1, TOK1, CH2, TOK2)              \
    case CH1:                                   \
    stream++;                                   \
    if (*stream == CH2) {                       \
        token.kind = TOK2;                      \
        stream++;                               \
    } else {                                    \
        token.kind = TOK1;                      \
    }                                           \
    break;                                      \


void next_token() {
begin:
    char *start = stream;
    
    switch (*stream) {
        TOK2('=', TOKEN_ASSIGN, '=', TOKEN_EQ);

        TOK1('(', TOKEN_LPAREN);
        TOK1(')', TOKEN_RPAREN);
        TOK1('[', TOKEN_LBRACKET);
        TOK1(']', TOKEN_RBRACKET);
        TOK1('{', TOKEN_LBRACE);
        TOK1('}', TOKEN_RBRACE);
        
        TOK1(';', TOKEN_SEMI);
        TOK1(',', TOKEN_COMMA);
        TOK1('?', TOKEN_QUESTION);
        TOK1('#', TOKEN_HASH);
        
        TOK2('!', TOKEN_BANG, '=', TOKEN_NEQ);
        TOK2('^', TOKEN_XOR,  '=', TOKEN_XOR_ASSIGN);
        TOK2('*', TOKEN_MULT, '=', TOKEN_MULT_ASSIGN);
        TOK2('%', TOKEN_MOD,  '=', TOKEN_MOD_ASSIGN);
        TOK2('.', TOKEN_DOT,  '.', TOKEN_ELLIPSIS);

    case ':': {
        token.kind = TOKEN_COLON;
        stream++;
        if (*stream == '=') {
            token.kind = TOKEN_AUTO_ASSIGN;
            stream++;
        } else if (*stream == ':') {
            token.kind = TOKEN_COLON2;
            stream++;
        }
    } break;
        
    case '<': {
        token.kind = TOKEN_LT;
        stream++;
        if (*stream == '<') {
            stream++;
            if (*stream == '=') {
                token.kind = TOKEN_LSHIFT_ASSIGN;
                stream++;
            } else {
                token.kind = TOKEN_LSHIFT;
            }
        } else if (*stream == '=') {
            stream++;
            token.kind = TOKEN_LTEQ;
        }
    } break;

    case '>': {
        token.kind = TOKEN_GT;
        stream++;
        if (*stream == '>') {
            stream++;
            if (*stream == '=') {
                token.kind = TOKEN_RSHIFT_ASSIGN;
                stream++;
            } else {
                token.kind = TOKEN_RSHIFT;
            }
        } else if (*stream == '=') {
            stream++;
            token.kind = TOKEN_GTEQ;
        }
    } break;
        
    case '-': {
        token.kind = TOKEN_MINUS;
        stream++;
        if (*stream == '>') {
            stream++;
            token.kind = TOKEN_ARROW;
        } else if (*stream == '-') {
            stream++;
            token.kind = TOKEN_DEC;
        } else if (*stream == '=') {
            stream++;
            token.kind = TOKEN_MINUS_ASSIGN;
        }
    } break;

    case '+': {
        token.kind = TOKEN_PLUS;
        stream++;
        if (*stream == '+') {
            stream++;
            token.kind = TOKEN_INC;
        } else if (*stream == '=') {
            stream++;
            token.kind = TOKEN_PLUS_ASSIGN;
        }
    } break;

    case '|': {
        token.kind = TOKEN_BAR;
        stream++;
        if (*stream == '|') {
            token.kind = TOKEN_OR;
            stream++;
        } else if (*stream == '=') {
            token.kind = TOKEN_OR_ASSIGN;
            stream++;
        }
        
    } break;
        
    case '&': {
        token.kind = TOKEN_AMPER;
        stream++;
        if (*stream == '&') {
            stream++;
            token.kind = TOKEN_AND;
        } else if (*stream == '=') {
            stream++;
            token.kind = TOKEN_AND_ASSIGN;
        }
    } break;
        
    case ' ': case '\r': case '\n': case '\t': case '\f': { // eat whitespace
        while (*stream == ' ' || *stream == '\n' || *stream == '\r' || *stream == '\t' || *stream == '\f') {
            stream++;
        }
        goto begin;
    }break;

    case '\'': {
        char ch;
        stream++;
        if (*stream == '\\') {
            stream++;
            ch = char_to_escape[*stream];
            // TODO: check valid escape
            stream++;
        } else {
            ch = *stream;
            stream++;
        }
        // TODO: expect terminating single quote
        stream++;
        token.kind = TOKEN_INT;
        token.int_val = (int64_t)ch;
    } break;

    case '"': {
        char *str_val = NULL;
        stream++;

        while (*stream) {
            char ch = *stream;
            if (ch == '"') {
                buf_push(str_val, '\0');
                stream++;
                break;
            }
            
            if (ch == '\r' || ch == '\n') {
                // TODO: error for newline in string
                break;
            }

            buf_push(str_val, ch);
            stream++;
        }

        token.kind = TOKEN_STR;
        token.str_val = str_val;
    } break;

    case '/': {
        token.kind = TOKEN_DIV;
        stream++;

        if (*stream == '=') {
            token.kind = TOKEN_DIV_ASSIGN;
            stream++;
        } else if (*stream == '/') {
            while (*stream) {
                if (*stream == '\r' || *stream == '\n') {
                    goto begin;
                }

                stream++;
            }
        } else if (*stream == '*') {
            stream++;
            for (;;) {
                if (*stream == '*') {
                    stream++;
                    if (*stream == '\\') {
                        stream++;
                        goto begin;
                    }
                }
                if (*stream == 0) {
                    // TODO: Report no multiline comment terminator
                }

                stream++;
            }
        }
    } break;
        
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
        while (isalnum(*stream)) {
            stream++;
        }
        if (*stream == '.') {
            stream = start;
            token.kind = TOKEN_FLOAT;
            token.float_val = parse_float();
        } else {
            stream = start;
            token.kind = TOKEN_INT;
            token.int_val = parse_int();
        }
    } break;
        
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case '_': {
        char *ident = NULL;
        while (isalnum(*stream) || *stream == '_') {
            buf_push(ident, *stream);
            stream++;
        }
        buf_push(ident, '\0');
        token.kind = TOKEN_IDENT;
        token.ident_val = ident;
    } break;

    default:
        token.kind = *stream;
        stream++;
    }
}

void init_stream(char *str) {
    stream = str;
    next_token();
}

Expr *expr_init(Expr_Kind kind) {
    Expr *expr = malloc(sizeof(Expr));
    expr->kind = kind;
    return expr;
}

Expr *expr_unary(Expr *expr, Token_Kind op) {
    Expr *result = expr_init(EXPR_UNARY);
    result->unary.expr = expr;
    result->unary.op = op;
    return expr;
}

Expr *expr_binary(Expr *left, Expr *right, Token_Kind op) {
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

inline bool is_cmp_op(Token_Kind kind) {
    return (kind == TOKEN_EQ || kind == TOKEN_NEQ || kind == TOKEN_LT || kind == TOKEN_GT || kind == TOKEN_LTEQ || kind == TOKEN_GTEQ);
}

inline bool is_add_op(Token_Kind kind) {
    return (kind == TOKEN_PLUS || kind == TOKEN_MINUS || kind == TOKEN_XOR || kind == TOKEN_BAR);
}

inline bool is_mul_op(Token_Kind kind) {
    return (kind == TOKEN_MULT || kind == TOKEN_DIV || kind == TOKEN_MOD || kind == TOKEN_AMPER || kind == TOKEN_LSHIFT || kind == TOKEN_RSHIFT);
}

inline bool is_unary_op(Token_Kind op) {
    return (op == TOKEN_PLUS || op == TOKEN_MINUS || op == TOKEN_AMPER || op == TOKEN_MULT);
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
    Token_Kind op = token.kind;
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
    Token_Kind op = token.kind;
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
    Token_Kind op = token.kind;
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
    Token_Kind op = token.kind;
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

Stmt_Block *parse_stmt_block();


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

// main :: proc(x : **[2]int, f :float) -> int {
    
// }

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

void main(int argc, char **argv) {
    Expr *expr = parse_expr_stream("10 + 20 * 5.0f - foo");
    Decl *decl =  parse_decl_stream("foo :: proc(x :int, y :int) -> int, float");
}
