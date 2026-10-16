#include <stdio.h>
#include <stdarg.h>

#include "../include/error.h"

int errorf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    return ret;
}