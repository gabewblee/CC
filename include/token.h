#pragma once

#include "error.h"

typedef enum {
    TokenEndOfFile,          /* EOF        */
    TokenIdentifier,         /* Identifier */
    TokenInteger,            /* Integer    */
    TokenCharacter,          /* Character  */

    /* Keywords */
    TokenBool,               /* Bool       */
    TokenBreak,              /* Break      */
    TokenCase,               /* Case       */
    TokenChar,               /* Char       */
    TokenConst,              /* Const      */
    TokenContinue,           /* Continue   */
    TokenDefault,            /* Default    */
    TokenElse,               /* Else       */
    TokenEnum,               /* Enum       */
    TokenExtern,             /* Extern     */
    TokenFor,                /* For        */
    TokenIf,                 /* If         */
    TokenInt,                /* Int        */
    TokenReturn,             /* Return     */
    TokenSizeof,             /* Sizeof     */
    TokenStruct,             /* Struct     */
    TokenSwitch,             /* Switch     */
    TokenTypedef,            /* Typedef    */
    TokenUnion,              /* Union      */
    TokenVoid,               /* Void       */
    TokenWhile,              /* While      */

    /* Operators */
    TokenAmpersand,          /* &          */
    TokenAmpersandAmpersand, /* &&         */
    TokenArrow,              /* ->         */
    TokenBang,               /* !          */
    TokenBangEqual,          /* !=         */
    TokenCaret,              /* ^          */
    TokenColon,              /* :          */
    TokenDot,                /* .          */
    TokenEqual,              /* =          */
    TokenEqualEqual,         /* ==         */
    TokenGreater,            /* >          */
    TokenGreaterEqual,       /* >=         */
    TokenGreaterGreater,     /* >>         */
    TokenLess,               /* <          */
    TokenLessEqual,          /* <=         */
    TokenLessLess,           /* <<         */
    TokenMinus,              /* -          */
    TokenMinusEqual,         /* -=         */
    TokenPercent,            /* %          */
    TokenPercentEqual,       /* %=         */
    TokenPipe,               /* |          */
    TokenPipePipe,           /* ||         */
    TokenPlus,               /* +          */
    TokenPlusEqual,          /* +=         */
    TokenQuestion,           /* ?          */
    TokenSlash,              /* /          */
    TokenSlashEqual,         /* /=         */
    TokenStar,               /* *          */
    TokenStarEqual,          /* *=         */
    TokenTilde,              /* ~          */

    /* Punctuation */
    TokenComma,              /* ,          */
    TokenLeftBrace,          /* {          */
    TokenLeftBracket,        /* [          */
    TokenLeftParenthesis,    /* (          */
    TokenRightBrace,         /* }          */
    TokenRightBracket,       /* ]          */
    TokenRightParenthesis,   /* )          */
    TokenSemicolon,          /* ;          */
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