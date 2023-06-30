#ifndef COMMON_H
#define COMMON_H

//#include "log.h"

#ifdef DEVELOPER
#define ASSERT(X) { \
   if (!(X)) {         \
       __debugbreak(); \
   }                   \
}
#else
#define ASSERT(X)
#endif

void syntax_error(const char *fmt, ...);

#endif // COMMON_H
