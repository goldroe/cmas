#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

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

enum {
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
    TOKEN_COMMA,
    TOKEN_ARROW,
    TOKEN_HASH,
    TOKEN_QUESTION,
} Token_Kind;

typedef struct {
    enum Token_Kind kind;

    union {
        int64_t int_val;
        double float_val;
        char *string_val;
        char *ident_val;
    };
} Token;

char *stream;
Token token;

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
        int digit = char_to_digit[toupper(*stream)];
        if (digit >= base) {
            // invalid digit in integer literal
        }
        result = result * base + digit;
        stream++;
    }
    return result;
}


void next_token() {
begin:
    char *start = stream;
    
    switch (*stream) {
    case ' ': case '\r': case '\n': case '\t': case '\f': { // eat whitespace
        while (*stream == ' ' || *stream == '\n' || *stream == '\r' || *stream == '\t' || *stream == '\f') {
            stream++;
        }
        goto begin;
    }break;
        
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

void main(int argc, char **argv) {
    stream = "1002 + 0b10";
    for (;;) {
        next_token();
        if (token.kind == TOKEN_EOF) {
            break;
        }
    }
}
