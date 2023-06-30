#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "common.h"
#include "log.h"
#include "darray.h"
#include "tok.h"

Token token;
char *stream;

int char_to_digit(char c) {
    if (c >= '0' || c <= '9') return c - '0';
    if (toupper(c) >= 'A' && toupper(c) <= 'F') return toupper(c) - 'A';
    return 0;
}

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
        int digit = char_to_digit(*stream);
        if (digit >= base) {
            // invalid digit in integer literal
        }
        result = result * base + digit;
        stream++;
    }
    return result;
}

char char_to_escape(char c) {
    switch (c) {
    case 't': return '\t';
    case 'n': return '\n';
    case 'r': return '\r';
    case '\\': return '\\';
    case '\'': return '\'';
    case '"': return '"';
    default:
        return c;
    }
}

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
        TOK1('\0', Token_EOF);
        TOK2('=', Token_Assign, '=', Token_Eq);

        TOK1('(', Token_Lparen);
        TOK1(')', Token_Rparen);
        TOK1('[', Token_Lbracket);
        TOK1(']', Token_Rbracket);
        TOK1('{', Token_Lbrace);
        TOK1('}', Token_Rbrace);
        
        TOK1(';', Token_Semi);
        TOK1(',', Token_Comma);
        TOK1('?', Token_Question);
        TOK1('#', Token_Hash);
        
        TOK2('!', Token_Bang, '=', Token_Neq);
        TOK2('^', Token_Xor,  '=', Token_XorAssign);
        TOK2('*', Token_Mult, '=', Token_MultAssign);
        TOK2('%', Token_Mod,  '=', Token_ModAssign);
        TOK2('.', Token_Dot,  '.', Token_Ellipsis);

    case ':': {
        token.kind = Token_Colon;
        stream++;
        if (*stream == '=') {
            token.kind = Token_AutoAssign;
            stream++;
        } else if (*stream == ':') {
            token.kind = Token_Colon2;
            stream++;
        }
    } break;
        
    case '<': {
        token.kind = Token_Lt;
        stream++;
        if (*stream == '<') {
            stream++;
            if (*stream == '=') {
                token.kind = Token_LshiftAssign;
                stream++;
            } else {
                token.kind = Token_Lshift;
            }
        } else if (*stream == '=') {
            stream++;
            token.kind = Token_Lteq;
        }
    } break;

    case '>': {
        token.kind = Token_Gt;
        stream++;
        if (*stream == '>') {
            stream++;
            if (*stream == '=') {
                token.kind = Token_RshiftAssign;
                stream++;
            } else {
                token.kind = Token_Rshift;
            }
        } else if (*stream == '=') {
            stream++;
            token.kind = Token_Gteq;
        }
    } break;
        
    case '-': {
        token.kind = Token_Minus;
        stream++;
        if (*stream == '>') {
            stream++;
            token.kind = Token_Arrow;
        } else if (*stream == '-') {
            stream++;
            token.kind = Token_Dec;
        } else if (*stream == '=') {
            stream++;
            token.kind = Token_MinusAssign;
        }
    } break;

    case '+': {
        token.kind = Token_Plus;
        stream++;
        if (*stream == '+') {
            stream++;
            token.kind = Token_Inc;
        } else if (*stream == '=') {
            stream++;
            token.kind = Token_PlusAssign;
        }
    } break;

    case '|': {
        token.kind = Token_Bar;
        stream++;
        if (*stream == '|') {
            token.kind = Token_Or;
            stream++;
        } else if (*stream == '=') {
            token.kind = Token_OrAssign;
            stream++;
        }
        
    } break;
        
    case '&': {
        token.kind = Token_Amper;
        stream++;
        if (*stream == '&') {
            stream++;
            token.kind = Token_And;
        } else if (*stream == '=') {
            stream++;
            token.kind = Token_AndAssign;
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
            ch = char_to_escape(*stream);
            // TODO: check valid escape
            stream++;
        } else {
            ch = *stream;
            stream++;
        }
        // TODO: expect terminating single quote
        stream++;
        token.kind = Token_Int;
        token.int_val = (int64_t)ch;
    } break;

    case '"': {
        String str_val;
        stream++;

        while (*stream) {
            char ch = *stream;
            if (ch == '"') {
                stream++;
                break;
            }
            
            if (ch == '\r' || ch == '\n') {
                // TODO: error for newline in string
                break;
            }

            str_val += ch;
            stream++;
        }

        token.kind = Token_Str;
        token.str_val = str_val;
    } break;

    case '/': {
        token.kind = Token_Div;
        stream++;

        if (*stream == '=') {
            token.kind = Token_DivAssign;
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
            token.kind = Token_Float;
            token.float_val = parse_float();
        } else {
            stream = start;
            token.kind = Token_Int;
            token.int_val = parse_int();
        }
    } break;
        
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case '_': {
        String ident;
        token.kind = Token_Ident;
        while (isalnum(*stream) || *stream == '_') {
            ident += *stream;
            stream++;
        }
        token.ident_val = ident;

        for (int kind = (int)Token_KeywordBegin + 1; kind < (int)Token_KeywordEnd; kind++) {
            String keyword = token_strings[kind];
            if (keyword == ident) {
                token.kind = (TokenKind)kind;
                break;
            }
        }
    } break;

    default:
        token.kind = (TokenKind)*stream;
        syntax_error("Unexpected character %c", *stream);
        stream++;
    }
    token.start = start;
}

bool is_token(TokenKind kind) {
    return token.kind == kind;
}
bool match_token(TokenKind kind) {
    if (token.kind == kind) {
        next_token();
        return true;
    } else {
        return false;
    }
}

Token advance_token() {
    Token result = token;
    next_token();
    return result;
}

void expect_token(TokenKind kind) {
    if (token.kind == kind) {
        next_token();
    } else {
        syntax_error("expected token %s, got %s", token_kind_string(kind).text, token_kind_string(token.kind).text);
    }
}

void init_stream(char *str) {
    stream = str;
    next_token();
}
