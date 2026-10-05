#pragma once

#include <memory>
#include <string>

#include "../include/ast.hpp"
#include "../include/lexer.hpp"

class Parser {
public:
    Parser(Lexer lexer);
    std::unique_ptr<Program> parse();

private:
    Lexer lexer_;
    Token lookahead_;

    /* --------------------------------------------------
     * Block
     * -------------------------------------------------- */

    std::unique_ptr<BlockEntry> parse_block_entry();

    /* --------------------------------------------------
     * Expressions
     * -------------------------------------------------- */

    std::unique_ptr<Expr> parse_expr();
    std::unique_ptr<Expr> parse_int_expr();
    std::unique_ptr<Expr> parse_id_expr();
    std::unique_ptr<Expr> parse_bin_expr();
    std::unique_ptr<Expr> parse_un_expr();
    std::unique_ptr<Expr> parse_assign_expr();
    std::unique_ptr<Expr> parse_call_expr();
    std::unique_ptr<Expr> parse_index_expr();
    std::unique_ptr<Expr> parse_member_expr();

    /* --------------------------------------------------
     * Statements
     * -------------------------------------------------- */
    
    std::unique_ptr<Stmt> parse_stmt();
    std::unique_ptr<Stmt> parse_ret_stmt();
    std::unique_ptr<Stmt> parse_compound_stmt();
    std::unique_ptr<Stmt> parse_if_stmt();
    std::unique_ptr<Stmt> parse_while_stmt();
    std::unique_ptr<Stmt> parse_break_stmt();
    std::unique_ptr<Stmt> parse_continue_stmt();

    /* --------------------------------------------------
     * Declarations
     * -------------------------------------------------- */

    std::unique_ptr<Decl> parse_decl();
    std::unique_ptr<Decl> parse_var_decl();
    std::unique_ptr<Decl> parse_param_decl();
    std::unique_ptr<Decl> parse_function_decl();
    std::unique_ptr<Decl> parse_field_decl();
    std::unique_ptr<Decl> parse_struct_decl();
    std::unique_ptr<Decl> parse_enumerator_decl();
    std::unique_ptr<Decl> parse_enum_decl();
    
    /* --------------------------------------------------
     * Helper functions
     * -------------------------------------------------- */
    
    void advance();
    bool match(TokenKind kind);
    Token next(TokenKind kind);
};