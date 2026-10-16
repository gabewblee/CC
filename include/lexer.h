#pragma once

#include <stddef.h>

#include "error.h"
#include "token.h"

/**
 * Lexer - Represents a lexer.
 * @location: The source file location.
 * @source: The source file string.
 * @length: The source file string length.
 */
typedef struct {
    Location    location;
    const char* source;
    size_t      length;
} Lexer;

/**
 * lexer_next() - Gets the next token from @lexer.
 * @lexer: The lexer to get the next token from.
 * Returns: The next token.
 */
Token lexer_next(Lexer *lexer);

/**
 * lexer_init() - Initializes @lexer with @source and @length.
 * @lexer: The lexer to initialize.
 * @source: The source file string to initialize with.
 * @length: The source file string length to initialize with.
 */
void lexer_init(Lexer *lexer, const char *source, size_t length);