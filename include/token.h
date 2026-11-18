#pragma once

#include "error.h"

typedef enum {
    TokenEndOfFile,
    TokenIdentifier,
    TokenInteger,
    TokenCharacter,

    /* Keywords */
    TokenBool,
    TokenBreak,
    TokenCase,
    TokenChar,
    TokenConst,
    TokenContinue,
    TokenDefault,
    TokenElse,
    TokenEnum,
    TokenExtern,
    TokenFor,
    TokenIf,
    TokenInt,
    TokenReturn,
    TokenSizeof,
    TokenStruct,
    TokenSwitch,
    TokenTypedef,
    TokenUnion,
    TokenVoid,
    TokenWhile,

    /* Operators */
    TokenAmpersand,
    TokenAmpersandAmpersand,
    TokenArrow,
    TokenBang,
    TokenBangEqual,
    TokenCaret,
    TokenColon,
    TokenDot,
    TokenEqual,
    TokenEqualEqual,
    TokenGreater,
    TokenGreaterEqual,
    TokenGreaterGreater,
    TokenLess,
    TokenLessEqual,
    TokenLessLess,
    TokenMinus,
    TokenMinusEqual,
    TokenPercent,
    TokenPercentEqual,
    TokenPipe,
    TokenPipePipe,
    TokenPlus,
    TokenPlusEqual,
    TokenQuestion,
    TokenSlash,
    TokenSlashEqual,
    TokenStar,
    TokenStarEqual,
    TokenTilde,

    /* Punctuation */
    TokenComma,
    TokenLeftBrace,
    TokenLeftBracket,
    TokenLeftParenthesis,
    TokenRightBrace,
    TokenRightBracket,
    TokenRightParenthesis,
    TokenSemicolon
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