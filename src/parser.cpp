#include "../include/ast.hpp"
#include "../include/parser.hpp"

Parser::Parser(Lexer lexer) : lexer_(std::move(lexer)), lookahead_(lexer_.next()) {
    
}

std::unique_ptr<Program> Parser::parse() {

}

/* --------------------------------------------------
 * Statements
 * -------------------------------------------------- */

std::unique_ptr<Stmt> Parser::parse_stmt() {

}

std::unique_ptr<Stmt> Parser::parse_ret_stmt() {

}

std::unique_ptr<Stmt> Parser::parse_compound_stmt() {

}

std::unique_ptr<Stmt> Parser::parse_if_stmt() {
    
}

std::unique_ptr<Stmt> Parser::parse_while_stmt() {
    
}

std::unique_ptr<Stmt> Parser::parse_break_stmt() {
    
}

std::unique_ptr<Stmt> Parser::parse_continue_stmt() {
    
}

/* --------------------------------------------------
 * Declarations
 * -------------------------------------------------- */

std::unique_ptr<Decl> Parser::parse_decl() {
    return parse_function_decl();
}

std::unique_ptr<Decl> Parser::parse_var_decl() {

}

std::unique_ptr<Decl> Parser::parse_param_decl() {

}

std::unique_ptr<Decl> Parser::parse_function_decl() {

}

std::unique_ptr<Decl> Parser::parse_field_decl() {
    
}

std::unique_ptr<Decl> Parser::parse_struct_decl() {
    
}

std::unique_ptr<Decl> Parser::parse_enumerator_decl() {
    
}

std::unique_ptr<Decl> Parser::parse_enum_decl() {
    
}

/* --------------------------------------------------
 * Helper functions
 * -------------------------------------------------- */

void Parser::advance() {
    lookahead_ = lexer_.next();
}

bool Parser::match(TokenKind kind) {
    return lookahead_.kind == kind;
}

Token Parser::next(TokenKind kind) {
    if (!match(kind)) {
        std::fprintf(stderr, "%zu:%zu: error: unexpected token '%s'\n", lookahead_.loc.line, lookahead_.loc.col, lookahead_.lexeme.c_str());
        std::exit(1);
    }

    Token cur = lookahead_;
    advance();
    return cur;
}