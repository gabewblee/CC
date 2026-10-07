#pragma once

#include <cstddef>
#include <string>

enum class TokenKind {
    Identifier,          /* Identifier literal */
    Integer,             /* Integer literal    */
    Character,           /* Character literal  */
    String,              /* String literal     */

    /* Types */
    Char,                /* "char"             */
    Enum,                /* "enum"             */
    Int,                 /* "int"              */
    Long,                /* "long"             */
    Struct,              /* "struct"           */
    Union,               /* "union"            */
    Void,                /* "void"             */

    /* Keywords */
    Break,               /* "break"            */
    Case,                /* "case"             */
    Continue,            /* "continue"         */
    Default,             /* "default"          */
    Else,                /* "else"             */
    For,                 /* "for"              */
    If,                  /* "if"               */
    Return,              /* "return"           */
    Sizeof,              /* "sizeof"           */
    Switch,              /* "switch"           */
    While,               /* "while"            */

    /* Punctuation */
    Arrow,               /* "->"               */
    Colon,               /* ":"                */
    Comma,               /* ","                */
    Dot,                 /* "."                */
    LeftBrace,           /* "{"                */
    LeftBracket,         /* "["                */
    LeftParenthesis,     /* "("                */
    RightBrace,          /* "}"                */
    RightBracket,        /* "]"                */
    RightParenthesis,    /* ")"                */
    Semicolon,           /* ";"                */

    /* Assignment operators */
    AmpersandEqual,      /* "&="               */
    CaretEqual,          /* "^="               */
    Equal,               /* "="                */
    GreaterGreaterEqual, /* ">>="              */
    LessLessEqual,       /* "<<="              */
    MinusEqual,          /* "-="               */
    PercentEqual,        /* "%="               */
    PipeEqual,           /* "|="               */
    PlusEqual,           /* "+="               */
    SlashEqual,          /* "/="               */
    StarEqual,           /* "*="               */

    /* Arithmetic operators */
    Minus,               /* "-"                */
    Percent,             /* "%"                */
    Plus,                /* "+"                */
    Slash,               /* "/"                */
    Star,                /* "*"                */

    /* Increment / decrement */
    MinusMinus,          /* "--"               */
    PlusPlus,            /* "++"               */

    /* Comparison operators */
    BangEqual,           /* "!="               */
    EqualEqual,          /* "=="               */
    Greater,             /* ">"                */
    GreaterEqual,        /* ">="               */
    Less,                /* "<"                */
    LessEqual,           /* "<="               */

    /* Logical operators */
    AmpersandAmpersand,  /* "&&"               */
    Bang,                /* "!"                */
    PipePipe,            /* "||"               */

    /* Bitwise operators */
    Ampersand,           /* "&"                */
    Caret,               /* "^"                */
    Pipe,                /* "|"                */
    Tilde,               /* "~"                */

    /* Shift operators */
    GreaterGreater,      /* ">>"               */
    LessLess,            /* "<<"               */

    EndOfFile,
    Invalid
};

/**
 * Location - Represents a source file location.
 * @offset: The source file offset, beginning at 0.
 * @line: The source file line, beginning at 1.
 * @col: The source file column, beginning at 1.
 */
struct Location {
    std::size_t offset = 0;
    std::size_t line   = 1;
    std::size_t col    = 1;
};

/**
 * Token - Represents a source file token.
 * @kind: The token kind.
 * @lexeme: The token lexeme.
 * @loc: The token location.
 */
struct Token {
    TokenKind       kind;
    std::string     lexeme;
    struct Location loc;
};

/**
 * describe - Describes a token based on its kind.
 * @kind: The token kind to describe by.
 * Returns: The token description.
 */
std::string describe(TokenKind kind);