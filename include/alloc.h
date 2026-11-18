#pragma once

#include <stddef.h>

/**
 * Chunk - Represents an arena block.
 * @next: The previously allocated chunk.
 * @used: The number of bytes in use.
 * @capacity: The number of bytes available.
 * @data: The area memory.
 */
typedef struct Chunk {
    struct Chunk *next;
    size_t        used;
    size_t        capacity;
    max_align_t   data[];
} Chunk;

/**
 * Arena - Represents a bump allocator.
 * @head: The head chunk.
 */
typedef struct {
    struct Chunk *head;
} Arena;

/**
 * arena_init() - Initializes @arena.
 * @arena: The arena to initialize.
 */
void arena_init(Arena *arena);

/**
 * arena_alloc() - Allocates @size bytes from @arena.
 * @arena: The arena to allocate from.
 * @size: The number of bytes to allocate.
 * Returns: The allocated memory.
 */
void *arena_alloc(Arena *arena, size_t size);

/**
 * arena_strndup() - Duplicates @source.
 * @arena: The arena to allocate from.
 * @source: The string to duplicate.
 * @length: The number of bytes to duplicate.
 * Returns: The duplicated string.
 */
char *arena_strndup(Arena *arena, const char *source, size_t length);

/**
 * arena_free() - Frees @arena.
 * @arena: The arena to free.
 */
void arena_free(Arena *arena);
