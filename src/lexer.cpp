#include <cctype>
#include <cstdio>
#include <string>
#include <unordered_map>

#include "lexer.hpp"

std::unordered_map<std::string, TokenKind> keywords = {
    {"int",    TokenKind::Int},
    {"void",   TokenKind::Void},
    {"return", TokenKind::Return},
};

Lexer::Lexer(std::string src_) : src_(src_) {
    loc_.offset = 0;
    loc_.line   = 1;
    loc_.col    = 1;
}

Token Lexer::next() {
    handle_blank();

    if (eof())
        return tokenize(TokenKind::EndOfFile, "", loc_);

    char c = peek();
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_')
        return handle_id();
    
    if (std::isdigit(static_cast<unsigned char>(c)))
        return handle_digit();
    
    Location loc = loc_;
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

struct Location Lexer::loc() {
    return loc_;
}

bool Lexer::eof() {
    return loc_.offset >= src_.size();
}

Token Lexer::tokenize(TokenKind kind, std::string lexeme, struct Location loc_) {
    return Token{
        kind,
        lexeme,
        loc_
    };
}

char Lexer::peek(std::size_t ahead) {
    return (loc_.offset + ahead >= src_.size()) ? '\0' : src_[loc_.offset + ahead];
}

void Lexer::advance() {
    if (eof())
        return;

    if (src_[loc_.offset] == '\n') {
        loc_.line++;
        loc_.col = 1;
    } else {
        loc_.col++;
    }

    loc_.offset++;
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
    std::size_t start = loc_.offset;
    Location loc{start, loc_.line, loc_.col};
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')
        advance();

    std::string lexeme = src_.substr(start, loc_.offset - start);
    auto keyword = keywords.find(lexeme);
    if (keyword != keywords.end())
        return tokenize(keyword->second, lexeme, loc);

    return tokenize(TokenKind::Identifier, lexeme, loc);
}

Token Lexer::handle_digit() {
    std::size_t start = loc_.offset;
    Location loc{start, loc_.line, loc_.col};
    while (std::isdigit(static_cast<unsigned char>(peek())))
        advance();

    std::string lexeme = src_.substr(start, loc_.offset - start);
    return tokenize(TokenKind::Integer, lexeme, loc);
}