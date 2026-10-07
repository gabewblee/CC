#include "token.hpp"

std::string describe(TokenKind kind) {
    switch (kind) {
        case TokenKind::Identifier:          return "identifier";
        case TokenKind::Integer:             return "integer";
        case TokenKind::Character:           return "character literal";
        case TokenKind::String:              return "string literal";

        /* Types */
        case TokenKind::Char:                return "'char'";
        case TokenKind::Enum:                return "'enum'";
        case TokenKind::Int:                 return "'int'";
        case TokenKind::Long:                return "'long'";
        case TokenKind::Struct:              return "'struct'";
        case TokenKind::Union:               return "'union'";
        case TokenKind::Void:                return "'void'";

        /* Keywords */
        case TokenKind::Break:               return "'break'";
        case TokenKind::Case:                return "'case'";
        case TokenKind::Continue:            return "'continue'";
        case TokenKind::Default:             return "'default'";
        case TokenKind::Else:                return "'else'";
        case TokenKind::For:                 return "'for'";
        case TokenKind::If:                  return "'if'";
        case TokenKind::Return:              return "'return'";
        case TokenKind::Sizeof:              return "'sizeof'";
        case TokenKind::Switch:              return "'switch'";
        case TokenKind::While:               return "'while'";

        /* Punctuation */
        case TokenKind::Arrow:               return "'->'";
        case TokenKind::Colon:               return "':'";
        case TokenKind::Comma:               return "','";
        case TokenKind::Dot:                 return "'.'";
        case TokenKind::LeftBrace:           return "'{'";
        case TokenKind::LeftBracket:         return "'['";
        case TokenKind::LeftParenthesis:     return "'('";
        case TokenKind::RightBrace:          return "'}'";
        case TokenKind::RightBracket:        return "']'";
        case TokenKind::RightParenthesis:    return "')'";
        case TokenKind::Semicolon:           return "';'";

        /* Assignment operators */
        case TokenKind::AmpersandEqual:      return "'&='";
        case TokenKind::CaretEqual:          return "'^='";
        case TokenKind::Equal:               return "'='";
        case TokenKind::GreaterGreaterEqual: return "'>>='";
        case TokenKind::LessLessEqual:       return "'<<='";
        case TokenKind::MinusEqual:          return "'-='";
        case TokenKind::PercentEqual:        return "'%='";
        case TokenKind::PipeEqual:           return "'|='";
        case TokenKind::PlusEqual:           return "'+='";
        case TokenKind::SlashEqual:          return "'/='";
        case TokenKind::StarEqual:           return "'*='";

        /* Arithmetic operators */
        case TokenKind::Minus:               return "'-'";
        case TokenKind::Percent:             return "'%'";
        case TokenKind::Plus:                return "'+'";
        case TokenKind::Slash:               return "'/'";
        case TokenKind::Star:                return "'*'";

        /* Increment / decrement */
        case TokenKind::MinusMinus:          return "'--'";
        case TokenKind::PlusPlus:            return "'++'";

        /* Comparison operators */
        case TokenKind::BangEqual:           return "'!='";
        case TokenKind::EqualEqual:          return "'=='";
        case TokenKind::Greater:             return "'>'";
        case TokenKind::GreaterEqual:        return "'>='";
        case TokenKind::Less:                return "'<'";
        case TokenKind::LessEqual:           return "'<='";

        /* Logical operators */
        case TokenKind::AmpersandAmpersand:  return "'&&'";
        case TokenKind::Bang:                return "'!'";
        case TokenKind::PipePipe:            return "'||'";

        /* Bitwise operators */
        case TokenKind::Ampersand:           return "'&'";
        case TokenKind::Caret:               return "'^'";
        case TokenKind::Pipe:                return "'|'";
        case TokenKind::Tilde:               return "'~'";

        /* Shift operators */
        case TokenKind::GreaterGreater:      return "'>>'";
        case TokenKind::LessLess:            return "'<<'";

        case TokenKind::EndOfFile:           return "end of file";
        case TokenKind::Invalid:             return "invalid token";
    }

    return "";
}
