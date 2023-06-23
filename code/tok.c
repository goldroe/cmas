#include <stdlib.h>
#include <ctype.h>

#include "common.h"
#include "tok.h"

Token token;
char *stream;

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

bool is_token(int kind) {
    return token.kind == kind;
}

bool match_token(int kind) {
    if (token.kind == kind) {
        next_token();
        return true;
    } else {
        return false;
    }
}

void expect_token(int kind) {
    if (token.kind == kind) {
        next_token();
    } else {
        syntax_error("expected token %d, got %d\n", kind, token.kind);
    }
}

void init_stream(char *str) {
    stream = str;
    next_token();
}
