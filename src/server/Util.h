#ifndef UTILS
#define UTILS

#include <string.h>

inline void strncpy_sec(char* destination, const char* source, size_t num) {
    strncpy(destination, source, num);
    destination[num - 1] = '\0';
}

#endif // UTILS
