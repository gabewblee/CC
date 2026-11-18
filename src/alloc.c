#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "error.h"

#define CHUNK_SIZE (64 * 1024)

void arena_init(Arena *arena) {
    arena->head = NULL;
}

void *arena_alloc(Arena *arena, size_t size) {
    size_t align = _Alignof(max_align_t);
    if (size > SIZE_MAX - sizeof(struct Chunk) - align) oom();
    size = (size + align - 1) & ~(align - 1);
    struct Chunk *chunk = arena->head;
    if (!chunk || size > chunk->capacity - chunk->used) {
        size_t capacity = size > CHUNK_SIZE ? size : CHUNK_SIZE;
        chunk = malloc(sizeof *chunk + capacity);
        if (!chunk) oom();
        chunk->used = 0;
        chunk->capacity  = capacity;
        if (size > CHUNK_SIZE && arena->head) {
            chunk->next       = arena->head->next;
            arena->head->next = chunk;
        } else {
            chunk->next = arena->head;
            arena->head = chunk;
        }
    }

    void *pointer = (char *)chunk->data + chunk->used;
    chunk->used += size;
    return pointer;
}

char *arena_strndup(Arena *arena, const char *source, size_t length) {
    if (length == SIZE_MAX) oom();
    char *dest = arena_alloc(arena, length + 1);
    if (length > 0) memcpy(dest, source, length);
    dest[length] = '\0';
    return dest;
}

void arena_free(Arena *arena) {
    struct Chunk *chunk = arena->head;
    while (chunk) {
        struct Chunk *next = chunk->next;
        free(chunk);
        chunk = next;
    }

    arena->head = NULL;
}
