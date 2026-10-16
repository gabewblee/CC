#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "error.h"
#include "lexer.h"

static char *read_file(Arena *arena, char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        errorf("error: failed to open '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    long len = -1;
    if (fseek(file, 0, SEEK_END) == 0)
        len = ftell(file);

    if (len < 0 || fseek(file, 0, SEEK_SET) != 0) {
        errorf("error: failed to read '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    char *src = arena_alloc(arena, (size_t)len + 1);
    if (fread(src, 1, (size_t)len, file) != (size_t)len) {
        errorf("error: failed to read '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    src[len] = '\0';
    fclose(file);
    return src;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        errorf("usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Arena arena;
    arena_init(&arena);

    char *src = read_file(&arena, argv[1]);
    Lexer lexer;
    lexer_init(&lexer, src, strlen(src));

    arena_free(&arena);
    return EXIT_SUCCESS;
}
