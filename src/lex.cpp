#include "lex.h"
#include "common.h"
#include <stb_ds.h>
#include <ctype.h>
#include <stdlib.h>

char *stream;
Token token;

double scan_float() {
    char *start = stream;

    if (*stream == '0') {
        stream++;
        if (*stream == '.') {
            stream++;
            if (isdigit(*stream)) {
                stream++;
                while (isdigit(*stream)) {
                    stream++;
                }
                if (*stream == 'f') stream++;
            } else {
                syntax_error("expected numbers after decimal, instead got %c", *stream);
            }
        }
    } else {
        while (isdigit(*stream)) {
            stream++;
        }
        if (*stream == '.') {
            stream++;
            if (isdigit(*stream)) {
                stream++;
                while (isdigit(*stream)) {
                    stream++;
                }
                if (*stream == 'f') stream++;
            } else {
                syntax_error("expected numbers after decimal, instead got %c", *stream);
            }
        }
    }

    double float_val = strtod(start, NULL);
    return float_val;
}

int64_t to_digit(char ch) {
    if (isdigit(ch)) return ch - '0';
    else if (isalnum(ch)) return toupper(ch) - 'A' + 10;
    else return 0;
}

int64_t scan_int() {
    int64_t base = 10;
    int64_t int_val = 0;

    if (*stream == '0') {
        stream++;
        if (toupper(*stream) == 'X') {
            base = 16;
            stream++;
        } else if (toupper(*stream) == 'B') {
            base = 2;
            stream++;
        } else if (isdigit(*stream)) {
            base = 8;
        } else { // 0
            return 0;
        }
    }
    
    while (isalnum(*stream)) {
        int64_t digit = to_digit(*stream);
        if (digit > base) syntax_error("invalid digit in integer literal, got %c", *stream);
        int_val *= base;
        int_val += digit;

        stream++;
    }

    return int_val;
}

void next_token() {
    char char_to_escape[128] = {};
    char_to_escape['n'] = '\n';
    char_to_escape['t'] = '\t';
    char_to_escape['f'] = '\f';
    char_to_escape['r'] = '\r';
    char_to_escape['0'] = '\0';
begin:
    char *start = stream;

    switch (*stream) {
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
        while (isdigit(*stream)) {
            stream++;
        }
        if (*stream == '.') {
            stream = start;
            double float_val = scan_float();
            token.kind = TOKEN_FLOAT;
            token.float_lit = float_val;
        } else {
            stream = start;
            int64_t int_val = scan_int();
            token.kind = TOKEN_INT;
            token.int_lit = int_val;
        }

    } break;

    case '_':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': {
        char *ident = nullptr;
        stream++;
        while (isalnum(*stream) || *stream == '_') {
            arrpush(ident, *stream);
            stream++;
        }
        arrpush(ident, '\0');
        token.kind = TOKEN_IDENT;
        token.ident = ident;
    } break;

    case '"': {
        char *str = nullptr;
        stream++;

        while (*stream != '"') {
            char ch = *stream;
            if (ch == '\\') {
                stream++;
                ch = char_to_escape[*stream];
                if (ch == 0) {
                    syntax_error("invalid escape character, %c", *stream);
                }
            }
            stream++;
            arrpush(str, ch);
        }

        arrpush(str, '\0');
        stream++;

        token.kind = TOKEN_STR;
        token.str_lit = str;
    } break;

    case '\'': {
        stream++;

        char val = 0;
        if (*stream == '\\') {
            stream++;
            char ch = char_to_escape[*stream];
            if (ch) {
                stream++;
            } else {
                syntax_error("invalid escape character, %c", *stream);
            }
            val = ch;
        } else {
            if (*stream == '\'') {
                syntax_error("expected a character in char literal");
            }
            val = *stream;
            stream++;
        }
        stream++;
        
        token.kind = TOKEN_CHAR;
        token.char_lit = val;
    } break;
    }
}
