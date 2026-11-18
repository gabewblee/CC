#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../include/lexer.h"

#define isalpha(c) ((('a' <= (c)) && ((c) <= 'z')) || (('A' <= (c)) && ((c) <= 'Z')) || (c) == '_')
#define isdigit(c) ('0' <= (c) && (c) <= '9')
#define ishex(c)   (isdigit(c) || ('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))
#define isalnum(c) (isalpha(c) || isdigit(c))

static char peek(Lexer *lexer, size_t ahead) {
    size_t offset = lexer->location.offset;
    return offset >= lexer->length || ahead >= lexer->length - offset ? '\0' : lexer->source[offset + ahead];
}

static char advance(Lexer *lexer) {
    if (lexer->location.offset >= lexer->length) return '\0';

    char c = lexer->source[lexer->location.offset++];
    if (c == '\n') {
        lexer->location.line++;
        lexer->location.column = 1;
    } else {
        lexer->location.column++;
    }

    return c;
}

static bool eof(Lexer *lexer) {
    return lexer->location.offset >= lexer->length;
}

static bool same(const char *lexeme, size_t length, const char *keyword) {
    return strlen(keyword) == length && memcmp(lexeme, keyword, length) == 0;
}

static bool accept(Lexer *lexer, char expected) {
    if (eof(lexer) || peek(lexer, 0) != expected) return false;
    advance(lexer);
    return true;
}

static Token token(Lexer *lexer, Location start, TokenKind kind) {
    return (Token){.location = start, .kind = kind, .lexeme = lexer->source + start.offset, .length = lexer->location.offset - start.offset};
}

static void handle_blank(Lexer *lexer) {
    while (!eof(lexer)) {
        char c = peek(lexer, 0);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            advance(lexer);
            continue;
        }

        if (c == '/' && peek(lexer, 1) == '/') {
            while (!eof(lexer) && peek(lexer, 0) != '\n') advance(lexer);
            continue;
        }

        if (c == '/' && peek(lexer, 1) == '*') {
            Location start = lexer->location;
            advance(lexer); /* '/' */
            advance(lexer); /* '*' */
            while (!eof(lexer) && !(peek(lexer, 0) == '*' && peek(lexer, 1) == '/')) advance(lexer);
            if (eof(lexer)) {
                errorf("%zu:%zu: error: unterminated comment", start.line, start.column);
                exit(EXIT_FAILURE);
            }

            advance(lexer); /* '*' */
            advance(lexer); /* '/' */
            continue;
        }

        break;
    }
}

static Token handle_id(Lexer *lexer) {
    Location start = lexer->location;
    while (isalnum(peek(lexer, 0))) advance(lexer);

    const char *lexeme = lexer->source + start.offset;
    size_t length = lexer->location.offset - start.offset;
    TokenKind kind = TokenIdentifier;
    switch (lexeme[0]) {
        case '_':
            if (same(lexeme, length, "_Bool")) kind = TokenBool;
            break;
        case 'b':
            if (same(lexeme, length, "break")) kind = TokenBreak;
            break;
        case 'c':
            if      (same(lexeme, length, "case"))     kind = TokenCase;
            else if (same(lexeme, length, "char"))     kind = TokenChar;
            else if (same(lexeme, length, "const"))    kind = TokenConst;
            else if (same(lexeme, length, "continue")) kind = TokenContinue;
            break;
        case 'd':
            if (same(lexeme, length, "default")) kind = TokenDefault;
            break;
        case 'e':
            if      (same(lexeme, length, "else"))   kind = TokenElse;
            else if (same(lexeme, length, "enum"))   kind = TokenEnum;
            else if (same(lexeme, length, "extern")) kind = TokenExtern;
            break;
        case 'f':
            if (same(lexeme, length, "for")) kind = TokenFor;
            break;
        case 'i':
            if      (same(lexeme, length, "if"))  kind = TokenIf;
            else if (same(lexeme, length, "int")) kind = TokenInt;
            break;
        case 'r':
            if (same(lexeme, length, "return")) kind = TokenReturn;
            break;
        case 's':
            if      (same(lexeme, length, "sizeof")) kind = TokenSizeof;
            else if (same(lexeme, length, "struct")) kind = TokenStruct;
            else if (same(lexeme, length, "switch")) kind = TokenSwitch;
            break;
        case 't':
            if (same(lexeme, length, "typedef")) kind = TokenTypedef;
            break;
        case 'u':
            if (same(lexeme, length, "union")) kind = TokenUnion;
            break;
        case 'v':
            if (same(lexeme, length, "void")) kind = TokenVoid;
            break;
        case 'w':
            if (same(lexeme, length, "while")) kind = TokenWhile;
            break;
    }

    return token(lexer, start, kind);
}

static Token handle_digit(Lexer *lexer) {
    Location start = lexer->location;
    while (isdigit(peek(lexer, 0))) advance(lexer);
    if (isalpha(peek(lexer, 0))) {
        errorf("%zu:%zu: error: invalid decimal", start.line, start.column);
        exit(EXIT_FAILURE);
    }

    return token(lexer, start, TokenInteger);
}

Token lexer_next(Lexer *lexer) {
    handle_blank(lexer);
    if (eof(lexer))
        return token(lexer, lexer->location, TokenEndOfFile);

    char c = peek(lexer, 0);
    if (isalpha(c))
        return handle_id(lexer);

    if (isdigit(c))
        return handle_digit(lexer);

    Location start = lexer->location;
    TokenKind kind;
    switch (advance(lexer)) {
        case '&': kind = accept(lexer, '&') ? TokenAmpersandAmpersand : TokenAmpersand;                                          break;
        case '!': kind = accept(lexer, '=') ? TokenBangEqual          : TokenBang;                                               break;
        case '^': kind = TokenCaret;                                                                                             break;
        case ':': kind = TokenColon;                                                                                             break;
        case ',': kind = TokenComma;                                                                                             break;
        case '.': kind = TokenDot;                                                                                               break;
        case '=': kind = accept(lexer, '=') ? TokenEqualEqual         : TokenEqual;                                              break;
        case '>': kind = accept(lexer, '=') ? TokenGreaterEqual       : accept(lexer, '>') ? TokenGreaterGreater : TokenGreater; break;
        case '{': kind = TokenLeftBrace;                                                                                         break;
        case '[': kind = TokenLeftBracket;                                                                                       break;
        case '(': kind = TokenLeftParenthesis;                                                                                   break;
        case '<': kind = accept(lexer, '=') ? TokenLessEqual          : accept(lexer, '<') ? TokenLessLess       : TokenLess;    break;
        case '-': kind = accept(lexer, '>') ? TokenArrow              : accept(lexer, '=') ? TokenMinusEqual     : TokenMinus;   break;
        case '%': kind = accept(lexer, '=') ? TokenPercentEqual       : TokenPercent;                                            break;
        case '|': kind = accept(lexer, '|') ? TokenPipePipe           : TokenPipe;                                               break;
        case '+': kind = accept(lexer, '=') ? TokenPlusEqual          : TokenPlus;                                               break;
        case '?': kind = TokenQuestion;                                                                                          break;
        case '}': kind = TokenRightBrace;                                                                                        break;
        case ']': kind = TokenRightBracket;                                                                                      break;
        case ')': kind = TokenRightParenthesis;                                                                                  break;
        case ';': kind = TokenSemicolon;                                                                                         break;
        case '/': kind = accept(lexer, '=') ? TokenSlashEqual         : TokenSlash;                                              break;
        case '*': kind = accept(lexer, '=') ? TokenStarEqual          : TokenStar;                                               break;
        case '~': kind = TokenTilde;                                                                                             break;
        default:
            errorf("%zu:%zu: error: unexpected character '%c'", start.line, start.column, c);
            exit(EXIT_FAILURE);
    }

    return token(lexer, start, kind);
}

void lexer_init(Lexer *lexer, const char *source, size_t length) {
    lexer->location = (Location){.offset = 0, .line = 1, .column = 1};
    lexer->source   = source;
    lexer->length   = length;
}