#pragma once

#include <array>
#include <memory>
#include <string>

#include "../include/ast.hpp"
#include "../include/lexer.hpp"

class Parser {
public:
    Parser(Lexer lexer);
    std::unique_ptr<Program> parse();

private:
    Lexer                lexer_;
    std::array<Token, 3> lookahead_;

    /* --------------------------------------------------
     * Type
     * -------------------------------------------------- */

    Type parse_type();

    /* --------------------------------------------------
     * Expressions
     * -------------------------------------------------- */

    std::unique_ptr<Expr> parse_expr();
    std::unique_ptr<Expr> parse_assign_expr();
    std::unique_ptr<Expr> parse_bin_expr(int min);
    std::unique_ptr<Expr> parse_un_expr();
    std::unique_ptr<Expr> parse_postfix_expr();
    std::unique_ptr<Expr> parse_primary_expr();

    /* --------------------------------------------------
     * Statements
     * -------------------------------------------------- */
    
    std::unique_ptr<Stmt> parse_stmt();
    std::unique_ptr<RetStmt> parse_ret_stmt();
    std::unique_ptr<CompoundStmt> parse_compound_stmt();
    std::unique_ptr<IfStmt> parse_if_stmt();
    std::unique_ptr<WhileStmt> parse_while_stmt();
    std::unique_ptr<BreakStmt> parse_break_stmt();
    std::unique_ptr<ContinueStmt> parse_continue_stmt();
    std::unique_ptr<ExprStmt> parse_expr_stmt();

    /* --------------------------------------------------
     * Declarations
     * -------------------------------------------------- */

    std::unique_ptr<Decl> parse_decl();
    std::unique_ptr<VarDecl> parse_var_decl(struct Location loc, Type type, std::string id);
    std::unique_ptr<ParamDecl> parse_param_decl();
    std::unique_ptr<FunctionDecl> parse_function(struct Location loc, Type type, std::string id);
    std::unique_ptr<FieldDecl> parse_field_decl();
    std::unique_ptr<StructDecl> parse_struct_def();
    std::unique_ptr<EnumeratorDecl> parse_enumerator();
    std::unique_ptr<EnumDecl> parse_enum_def();
    
    /* --------------------------------------------------
     * Helper functions
     * -------------------------------------------------- */
    
    int precedence(TokenKind kind);
    BinOp binop(TokenKind kind);
    void advance();
    bool decl();
    bool check(TokenKind kind, std::size_t offset = 0);
    Token& peek(std::size_t offset = 0);
    Token consume(TokenKind kind);
};