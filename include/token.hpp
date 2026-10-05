#pragma once

#include <cstddef>
#include <string>

enum class TokenKind {
    Identifier,
    Integer,

    /* Types */
    Char,             /* "char"   */
    Int,              /* "int"    */
    Long,             /* "long"   */
    Void,             /* "void"   */
    Struct,           /* "struct" */
    Union,            /* "union"  */
    Enum,             /* "enum"   */

    /* Keywords */
    Return,           /* "return" */

    /* Punctuations */
    LeftParenthesis,  /* "("      */
    RightParenthesis, /* ")"      */
    LeftBrace,        /* "{"      */
    RightBrace,       /* "}"      */
    Semicolon,        /* ";"      */
    EndOfFile,
    Invalid
};

struct Location {
    std::size_t offset = 0; /* Source file offset */
    std::size_t line   = 1; /* Source file line   */
    std::size_t col    = 1; /* Source file column */
};

struct Token {
    TokenKind       kind;   /* Token kind     */
    std::string     lexeme; /* Token lexeme   */
    struct Location loc;    /* Token location */
};

/**
 * describe - Describes a token based on @kind.
 * @kind: The token kind to describe by.
 * Returns: The token description.
 */
std::string describe(TokenKind kind);