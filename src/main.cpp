#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "../include/lexer.hpp"
#include "../include/token.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s <file.c>\n", argv[0]);
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::fprintf(stderr, "error: failed to open '%s'\n", argv[1]);
        return 1;
    }

    std::stringstream buf;
    buf << file.rdbuf();

    std::string src = buf.str();
    Lexer lexer(src);
    return 0;
}