#ifndef LOG_H
#define LOG_H

#include "colterm.h"

#define FOREGROUND_YELLOW 6
#define FOREGROUND_WHITE 7

#define LOG(FMT, ...)     COLTERM_PRINTF(FOREGROUND_WHITE, FMT, ##__VA_ARGS__), printf("\n")
#define ALERT(FMT, ...)   COLTERM_PRINTF(FOREGROUND_RED, FMT, ##__VA_ARGS__), printf("\n")
#define WARNING(FMT, ...) COLTERM_PRINTF(FOREGROUND_YELLOW, FMT, ##__VA_ARGS__), printf("\n")

#endif // LOG_H
