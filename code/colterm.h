#ifndef COLTERM_H
#define COLTERM_H
#include <stdio.h>


#if defined(__linux__)
#ifndef COLTERM_ANSI
#define COLTERM_ANSI
#endif // COLTERM_ANSI
#endif // LINUX

void set_term_color(int color);

#ifndef COLTERM_ANSI
#include <windows.h>
#ifdef COLTERM_IMPLEMENTATION
void set_term_color(int color) {
    static HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    static WORD default_attribs = 0;
    if (default_attribs == 0) {
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (GetConsoleScreenBufferInfo(console, &info)) {
            default_attribs = info.wAttributes;
        }
    }
    if (color == -1) {
        color = default_attribs;
    }
    SetConsoleTextAttribute(console, (WORD)color);
}
#endif // COLTERM_IMPLEMENTATION

#define COLTERM_PRINTF(CLR, FMT, ...) (set_term_color(CLR), printf(FMT, ##__VA_ARGS__), set_term_color(-1))

#endif // COLTERM_ANSI


#ifdef COLTERM_ANSI

#define __ANSI_COLOR_RED "1;31"
#define __ANSI_COLOR_GREEN "0;32"
#define __ANSI_COLOR_BLUE "1;34"
#define __ANSI_COLOR_CYAN "0;36"
#define __ANSI_COLOR_YELLOW "0;33"
#define __ANSI_COLOR_WHITE "0;37"

#define COLTERM_PRINTF(CLR, FMT, ...) printf("\033[%sm" FMT "\033[m\n", CLR, ##__VA_ARGS__)

#endif // COLTERM_ANSI

#endif // COLTERM_H
