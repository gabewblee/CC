#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/error.h"

void oom(void) {
    errorf("error: out of memory\n");
    exit(EXIT_FAILURE);
}

int errorf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    return ret;
}