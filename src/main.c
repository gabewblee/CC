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

    long length = -1;
    if (fseek(file, 0, SEEK_END) == 0)
        length = ftell(file);

    if (length < 0 || fseek(file, 0, SEEK_SET) != 0) {
        errorf("error: failed to read '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    char *source = arena_alloc(arena, (size_t)length + 1);
    if (fread(source, 1, (size_t)length, file) != (size_t)length) {
        errorf("error: failed to read '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    source[length] = '\0';
    fclose(file);
    return source;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        errorf("usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Arena arena;
    arena_init(&arena);

    char *source = read_file(&arena, argv[1]);
    Lexer lexer;
    lexer_init(&lexer, source, strlen(source));

    arena_free(&arena);
    return EXIT_SUCCESS;
}
