#pragma once

#include <cstddef>
#include <string>

enum class TokenKind {
    Identifier,
    Integer,

    /* Types */
    Char,                /* "char"     */
    Int,                 /* "int"      */
    Long,                /* "long"     */
    Void,                /* "void"     */
    Struct,              /* "struct"   */
    Union,               /* "union"    */
    Enum,                /* "enum"     */

    /* Keywords */
    Return,              /* "return"   */
    While,               /* "while"    */
    If,                  /* "if"       */
    Else,                /* "else"     */
    Break,               /* "break"    */
    Continue,            /* "continue" */

    /* Punctuation */
    LeftParenthesis,     /* "("        */
    RightParenthesis,    /* ")"        */
    LeftBracket,         /* "["        */
    RightBracket,        /* "]"        */
    LeftBrace,           /* "{"        */
    RightBrace,          /* "}"        */
    Semicolon,           /* ";"        */
    Comma,               /* ","        */
    Dot,                 /* "."        */
    Arrow,               /* "->"       */

    /* Assignment operators */
    Equal,               /* "="        */
    PlusEqual,           /* "+="       */
    MinusEqual,          /* "-="       */
    StarEqual,           /* "*="       */
    SlashEqual,          /* "/="       */
    PercentEqual,        /* "%="       */
    AmpersandEqual,      /* "&="       */
    PipeEqual,           /* "|="       */
    CaretEqual,          /* "^="       */
    LessLessEqual,       /* "<<="      */
    GreaterGreaterEqual, /* ">>="      */

    /* Arithmetic operators */
    Plus,                /* "+"        */
    Minus,               /* "-"        */
    Star,                /* "*"        */
    Slash,               /* "/"        */
    Percent,             /* "%"        */

    /* Increment / decrement */
    PlusPlus,            /* "++"       */
    MinusMinus,          /* "--"       */

    /* Comparison operators */
    EqualEqual,          /* "=="       */
    BangEqual,           /* "!="       */
    Less,                /* "<"        */
    LessEqual,           /* "<="       */
    Greater,             /* ">"        */
    GreaterEqual,        /* ">="       */

    /* Logical operators */
    Bang,                /* "!"        */
    AmpersandAmpersand,  /* "&&"       */
    PipePipe,            /* "||"       */

    /* Bitwise operators */
    Ampersand,           /* "&"        */
    Pipe,                /* "|"        */
    Caret,               /* "^"        */
    Tilde,               /* "~"        */

    /* Shift operators */
    LessLess,            /* "<<"       */
    GreaterGreater,      /* ">>"       */

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