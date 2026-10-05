#include <cctype>
#include <cstdio>
#include <string>
#include <unordered_map>

#include "lexer.hpp"

std::unordered_map<std::string, TokenKind> keywords = {
    {"int",    TokenKind::Int},
    {"return", TokenKind::Return},
};

Lexer::Lexer(std::string src) : src(src) {
    location.offset = 0;
    location.line   = 1;
    location.col    = 1;
}

Token Lexer::next() {
    handle_blank();

    if (eof())
        return tokenize(TokenKind::EndOfFile, "", location);

    char c = peek();
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_')
        return handle_id();
    
    if (std::isdigit(static_cast<unsigned char>(c)))
        return handle_digit();
    
    Location loc = location;
    advance();
    switch(c) {
        case '(': return tokenize(TokenKind::LeftParenthesis, "(", loc);
        case ')': return tokenize(TokenKind::RightParenthesis, ")", loc);
        case '{': return tokenize(TokenKind::LeftBrace, "{", loc);
        case '}': return tokenize(TokenKind::RightBrace, "}", loc);
        case ';': return tokenize(TokenKind::Semicolon, ";", loc);
    }

    std::fprintf(stderr, "%zu:%zu: error: unexpected character '%c'\n", loc.line, loc.col, c);
    return tokenize(TokenKind::Invalid, std::string(1, c), loc);
}

bool Lexer::eof() {
    return location.offset >= src.size();
}

Token Lexer::tokenize(TokenKind kind, std::string lexeme, struct Location location) {
    return Token{
        kind,
        lexeme,
        location
    };
}

char Lexer::peek(std::size_t ahead) {
    return (location.offset + ahead >= src.size()) ? '\0' : src[location.offset + ahead];
}

void Lexer::advance() {
    if (eof())
        return;

    if (src[location.offset] == '\n') {
        location.line++;
        location.col = 1;
    } else {
        location.col++;
    }

    location.offset++;
}

void Lexer::handle_blank() {
    while (!eof()) {
        char c = peek();
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            break;
        
        advance();
    }
}

Token Lexer::handle_id() {
    std::size_t start = location.offset;
    Location loc{start, location.line, location.col};
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')
        advance();

    std::string lexeme = src.substr(start, location.offset - start);
    auto keyword = keywords.find(lexeme);
    if (keyword != keywords.end())
        return tokenize(keyword->second, lexeme, loc);

    return tokenize(TokenKind::Identifier, lexeme, loc);
}

Token Lexer::handle_digit() {
    std::size_t start = location.offset;
    Location loc{start, location.line, location.col};
    while (std::isdigit(static_cast<unsigned char>(peek())))
        advance();

    std::string lexeme = src.substr(start, location.offset - start);
    return tokenize(TokenKind::Integer, lexeme, loc);
}