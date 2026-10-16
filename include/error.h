#pragma once

#include <stddef.h>

/**
 * Location - Represents a source file location.
 * @offset: The source file offset.
 * @line: The source file line.
 * @column: The source file column.
 */
typedef struct {
    size_t offset;
    size_t line;
    size_t column;
} Location;

/**
 * oom() - Reports an out of memory error.
 */
void oom(void);

/**
 * errorf() - Writes @format to stderr.
 * @format: The formatted string to write.
 * Returns: The number of characters written on success, negative otherwise.
 */
int errorf(const char *format, ...);