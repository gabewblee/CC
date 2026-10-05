#include "token.hpp"

std::string describe(TokenKind kind) {
    switch (kind) {
        case TokenKind::Identifier:       return "identifier";
        case TokenKind::Integer:          return "integer";
        case TokenKind::Int:              return "'int'";
        case TokenKind::Return:           return "'return'";
        case TokenKind::LeftParenthesis:  return "')'";
        case TokenKind::RightParenthesis: return "'('";
        case TokenKind::LeftBrace:        return "'{'";
        case TokenKind::RightBrace:       return "'}'";
        case TokenKind::Semicolon:        return "';'";
        default:                          return "";
    }
}