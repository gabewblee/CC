#include <cctype>
#include <cstdio>
#include <string>
#include <unordered_map>

#include "lexer.hpp"

std::unordered_map<std::string, TokenKind> keywords = {
    /* Types */
    {"char",     TokenKind::Char},
    {"enum",     TokenKind::Enum},
    {"int",      TokenKind::Int},
    {"long",     TokenKind::Long},
    {"struct",   TokenKind::Struct},
    {"union",    TokenKind::Union},
    {"void",     TokenKind::Void},

    /* Keywords */
    {"break",    TokenKind::Break},
    {"case",     TokenKind::Case},
    {"continue", TokenKind::Continue},
    {"default",  TokenKind::Default},
    {"else",     TokenKind::Else},
    {"for",      TokenKind::For},
    {"if",       TokenKind::If},
    {"return",   TokenKind::Return},
    {"sizeof",   TokenKind::Sizeof},
    {"switch",   TokenKind::Switch},
    {"while",    TokenKind::While}
};

Lexer::Lexer(std::string src) : src_(src) {
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
    
    if (c == '\'' || c == '"')
        return handle_quote(c);

    struct Location loc = loc_;
    advance();
    switch(c) {
        /* Single character tokens */
        case ':': return tokenize(TokenKind::Colon, ":", loc);
        case ',': return tokenize(TokenKind::Comma, ",", loc);
        case '.': return tokenize(TokenKind::Dot, ".", loc);
        case '{': return tokenize(TokenKind::LeftBrace, "{", loc);
        case '[': return tokenize(TokenKind::LeftBracket, "[", loc);
        case '(': return tokenize(TokenKind::LeftParenthesis, "(", loc);
        case '}': return tokenize(TokenKind::RightBrace, "}", loc);
        case ']': return tokenize(TokenKind::RightBracket, "]", loc);
        case ')': return tokenize(TokenKind::RightParenthesis, ")", loc);
        case ';': return tokenize(TokenKind::Semicolon, ";", loc);
        case '~': return tokenize(TokenKind::Tilde, "~", loc);

        /* Unambiguous two character tokens */
        case '*': return either('=', TokenKind::StarEqual, TokenKind::Star, loc);
        case '/': return either('=', TokenKind::SlashEqual, TokenKind::Slash, loc);
        case '%': return either('=', TokenKind::PercentEqual, TokenKind::Percent, loc);
        case '^': return either('=', TokenKind::CaretEqual, TokenKind::Caret, loc);
        case '=': return either('=', TokenKind::EqualEqual, TokenKind::Equal, loc);
        case '!': return either('=', TokenKind::BangEqual, TokenKind::Bang, loc);

        /* Ambiguous two character tokens */
        case '+':
            if (peek() == '+') return either('+', TokenKind::PlusPlus, TokenKind::Plus, loc);
            return either('=', TokenKind::PlusEqual, TokenKind::Plus, loc);
        case '-':
            if (peek() == '>') return either('>', TokenKind::Arrow, TokenKind::Minus, loc);
            if (peek() == '-') return either('-', TokenKind::MinusMinus, TokenKind::Minus, loc);
            return either('=', TokenKind::MinusEqual, TokenKind::Minus, loc);
        case '&':
            if (peek() == '&') return either('&', TokenKind::AmpersandAmpersand, TokenKind::Ampersand, loc);
            return either('=', TokenKind::AmpersandEqual, TokenKind::Ampersand, loc);
        case '|':
            if (peek() == '|') return either('|', TokenKind::PipePipe, TokenKind::Pipe, loc);
            return either('=', TokenKind::PipeEqual, TokenKind::Pipe, loc);
        case '<':
            if (peek() == '<') {
                advance();
                return either('=', TokenKind::LessLessEqual, TokenKind::LessLess, loc);
            }
            return either('=', TokenKind::LessEqual, TokenKind::Less, loc);
        case '>':
            if (peek() == '>') {
                advance();
                return either('=', TokenKind::GreaterGreaterEqual, TokenKind::GreaterGreater, loc);
            }
            return either('=', TokenKind::GreaterEqual, TokenKind::Greater, loc);
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

Token Lexer::tokenize(TokenKind kind, std::string lexeme, struct Location loc) {
    return Token{
        kind,
        lexeme,
        loc
    };
}

Token Lexer::either(char c, TokenKind yes, TokenKind no, struct Location loc) {
    TokenKind kind = no;
    if (peek() == c) {
        advance();
        kind = yes;
    }

    return tokenize(kind, src_.substr(loc.offset, loc_.offset - loc.offset), loc);
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
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
            continue;
        }

        if (c == '/' && peek(1) == '/') {
            while (!eof() && peek() != '\n') advance();
            continue;
        }

        if (c == '/' && peek(1) == '*') {
            struct Location loc = loc_;
            advance();
            advance();
            while (!eof() && !(peek() == '*' && peek(1) == '/'))
                advance();

            if (eof()) {
                std::fprintf(stderr, "%zu:%zu: error: unterminated comment\n", loc.line, loc.col);
                return;
            }

            advance();
            advance();
            continue;
        }

        break;
    }
}

Token Lexer::handle_id() {
    std::size_t start = loc_.offset;
    struct Location loc{start, loc_.line, loc_.col};
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
    struct Location loc{start, loc_.line, loc_.col};
    while (std::isdigit(static_cast<unsigned char>(peek())))
        advance();

    std::string lexeme = src_.substr(start, loc_.offset - start);
    return tokenize(TokenKind::Integer, lexeme, loc);
}

Token Lexer::handle_quote(char quote) {
    std::size_t start = loc_.offset;
    struct Location loc{start, loc_.line, loc_.col};
    TokenKind kind = (quote == '\'') ? TokenKind::Character : TokenKind::String;
    const char *what = (quote == '\'') ? "character" : "string";
    advance();
    std::size_t len = 0;
    while (!eof() && peek() != quote && peek() != '\n') {
        if (peek() == '\\') {
            advance();
            char e = peek();
            if (eof() || e == '\n')
                break;

            if (e != 'n' && e != 't' && e != 'r' && e != '0' && e != '\\' && e != '\'' && e != '"')
                std::fprintf(stderr, "%zu:%zu: error: unknown escape sequence '\\%c'\n", loc_.line, loc_.col - 1, e);
        }

        advance();
        len++;
    }

    if (peek() != quote) {
        std::fprintf(stderr, "%zu:%zu: error: unterminated %s literal\n", loc.line, loc.col, what);
        return tokenize(TokenKind::Invalid, src_.substr(start, loc_.offset - start), loc);
    }

    advance();
    std::string lexeme = src_.substr(start, loc_.offset - start);
    if (kind == TokenKind::Character && len != 1) {
        std::fprintf(stderr, "%zu:%zu: error: characters must be one character long\n", loc.line, loc.col);
        return tokenize(TokenKind::Invalid, lexeme, loc);
    }

    return tokenize(kind, lexeme, loc);
}