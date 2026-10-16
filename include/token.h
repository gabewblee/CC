#pragma once

#include "error.h"

typedef enum {
    EndOfFile,
    Identifier,
    Integer,
    Character,

    /* Keywords */
    Bool,
    Break,
    Case,
    Char,
    Const,
    Continue,
    Default,
    Else,
    Enum,
    Extern,
    For,
    If,
    Int,
    Return,
    Sizeof,
    Struct,
    Switch,
    Typedef,
    Union,
    Void,
    While,

    /* Operators */
    Ampersand,
    AmpersandAmpersand,
    Arrow,
    Bang,
    BangEqual,
    Caret,
    Colon,
    Dot,
    Equal,
    EqualEqual,
    Greater,
    GreaterEqual,
    GreaterGreater,
    Less,
    LessEqual,
    LessLess,
    Minus,
    MinusEqual,
    Percent,
    PercentEqual,
    Pipe,
    PipePipe,
    Plus,
    PlusEqual,
    Question,
    Slash,
    SlashEqual,
    Star,
    StarEqual,
    Tilde,

    /* Punctuation */
    Comma,
    LeftBrace,
    LeftBracket,
    LeftParenthesis,
    RightBrace,
    RightBracket,
    RightParenthesis,
    Semicolon
} TokenKind;

/**
 * Token - Represents a token.
 * @location: The source file location.
 * @kind: The token kind.
 * @lexeme: The token lexeme.
 * @length: The token lexeme length.
 */
typedef struct {
    Location    location;
    TokenKind   kind;
    const char* lexeme;
    size_t      length;
} Token;