#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "../include/ast.hpp"
#include "../include/lexer.hpp"

class Parser {
public:
    /**
     * Parser - Initializes the parser with the lexer.
     */
    Parser(Lexer lexer);

    /**
     * parse - Parses the entire program.
     * Returns: The parsed program node.
     */
    std::unique_ptr<Program> parse();

private:
    Lexer                lexer_;
    std::array<Token, 3> lookahead_;

    /* --------------------------------------------------
     * Type
     * -------------------------------------------------- */

    /**
     * parse_type_base - Parses "void" | "char" | "int" | "long" | "struct" id | "union" id | "enum" id.
     * Returns: The parsed type node.
     */
    Type parse_type_base();

    /**
     * parse_type - Parses type_base ("*")* ("[" integer "]")*.
     * Returns: The parsed type node.
     */
    Type parse_type();

    /**
     * parse_declarator - Parses ("*")* id ("[" integer "]")*.
     * @base: The base declarator type.
     * Returns: The parsed declarator node.
     */
    Declarator parse_declarator(Type base);

    /**
     * parse_array_suffix - Parses ("[" integer "]")*.
     * @type: The parent type node.
     */
    void parse_array_suffix(Type& type);

    /* --------------------------------------------------
     * Expressions
     * -------------------------------------------------- */

    /**
     * parse_expr - Parses assign_expr.
     * Returns: The parsed expression node.
     */
    std::unique_ptr<Expr> parse_expr();

    /**
     * parse_assign_expr - Parses bin_expr (assignop assign_expr)?.
     * Returns: The parsed expression node.
     */
    std::unique_ptr<Expr> parse_assign_expr();

    /**
     * parse_bin_expr - Parses un_expr (binop bin_expr)*.
     * @min: The minimum precedence level.
     * Returns: The parsed expression node.
     */
    std::unique_ptr<Expr> parse_bin_expr(int min);

    /**
     * parse_un_expr - Parses sizeof_expr | cast_expr | unop un_expr | postfix_expr.
     * Returns: The parsed expression node.
     */
    std::unique_ptr<Expr> parse_un_expr();

    /**
     * parse_sizeof_expr - Parses "sizeof" "(" type_name ")" | "sizeof" un_expr.
     * Returns: The parsed expression node.
     */
    std::unique_ptr<Expr> parse_sizeof_expr();

    /**
     * parse_cast_expr - Parses "(" type_name ")" un_expr.
     * Returns: The parsed expression node.
     */
    std::unique_ptr<Expr> parse_cast_expr();

    /**
     * parse_postfix_expr - Parses primary_expr ("[" expr "]"                |
     *                             "(" (assign_expr ("," assign_expr)*)? ")" |
     *                             "." id                                    |
     *                             "->" id                                   |
     *                             "++"                                      |
     *                             "--")*.
     * Returns: The parsed expression node.
     */
    std::unique_ptr<Expr> parse_postfix_expr();

    /**
     * parse_primary_expr - Parses character | string | integer | id | "(" expr ")".
     * Returns: The parsed expression node.
     */
    std::unique_ptr<Expr> parse_primary_expr();

    /* --------------------------------------------------
     * Statements
     * -------------------------------------------------- */

    /**
     * parse_stmt - Parses compound_stmt |
     *                     if_stmt       |
     *                     while_stmt    |
     *                     for_stmt      |
     *                     switch_stmt   |
     *                     case_stmt     |
     *                     default_stmt  |
     *                     break_stmt    |
     *                     continue_stmt |
     *                     ret_stmt      |
     *                     expr_stmt.
     * Returns: The parsed statement node.
     */
    std::unique_ptr<Stmt> parse_stmt();

    /**
     * parse_ret_stmt - Parses "return" (expr)? ";".
     * Returns: The parsed return statement node.
     */
    std::unique_ptr<RetStmt> parse_ret_stmt();

    /**
     * parse_compound_stmt - Parses "{" (block_decl | stmt)* "}".
     * Returns: The parsed compound statement node.
     */
    std::unique_ptr<CompoundStmt> parse_compound_stmt();

    /**
     * parse_if_stmt - Parses "if" "(" expr ")" stmt ("else" stmt)?.
     * Returns: The parsed if statement node.
     */
    std::unique_ptr<IfStmt> parse_if_stmt();

    /**
     * parse_while_stmt - Parses "while" "(" expr ")" stmt.
     * Returns: The parsed while statement node.
     */
    std::unique_ptr<WhileStmt> parse_while_stmt();

    /**
     * parse_for_stmt - Parses "for" "(" (type declarator var_decl | expr)? ";" (expr)? ";" (expr)? ")" stmt.
     * Returns: The parsed for statement node.
     */
    std::unique_ptr<ForStmt> parse_for_stmt();

    /**
     * parse_switch_stmt - Parses "switch" "(" expr ")" stmt.
     * Returns: The parsed switch statement node.
     */
    std::unique_ptr<SwitchStmt> parse_switch_stmt();

    /**
     * parse_case_stmt - Parses "case" expr ":" stmt.
     * Returns: The parsed case statement node.
     */
    std::unique_ptr<CaseStmt> parse_case_stmt();

    /**
     * parse_default_stmt - Parses "default" ":" stmt.
     * Returns: The parsed default statement node.
     */
    std::unique_ptr<DefaultStmt> parse_default_stmt();

    /**
     * parse_break_stmt - Parses "break" ";".
     * Returns: The parsed break statement node.
     */
    std::unique_ptr<BreakStmt> parse_break_stmt();

    /**
     * parse_continue_stmt - Parses "continue" ";".
     * Returns: The parsed continue statement node.
     */
    std::unique_ptr<ContinueStmt> parse_continue_stmt();

    /**
     * parse_expr_stmt - Parses (expr)? ";".
     * Returns: The parsed expression statement node.
     */
    std::unique_ptr<ExprStmt> parse_expr_stmt();

    /* --------------------------------------------------
     * Declarations
     * -------------------------------------------------- */

    /**
     * parse_external_decl - Parses struct_def | union_def | enum_def | type declarator function | type declarator var_decl.
     * @program: The parent program declaration node.
     */
    void parse_external_decl(Program& program);

    /**
     * parse_block_decl - Parses struct_def | union_def | enum_def | type declarator var_decl.
     * @block: The parent compound statement declaration node.
     */
    void parse_block_decl(CompoundStmt& block);

    /**
     * parse_var_decl - Parses ("=" assign_expr)? ("," declarator ("=" assign_expr)?)* (";")?. The first declarator is already parsed.
     * @base: The base type of every declarator node.
     * @first: The first declarator node.
     * @semicolon: Whether a trailing ";" is expected.
     * Returns: The parsed variable declaration nodes.
     */
    std::vector<std::unique_ptr<VarDecl>> parse_var_decl(Type base, Declarator first, bool semicolon);

    /**
     * parse_param - Parses type declarator.
     * Returns: The parsed parameter declaration node.
     */
    std::unique_ptr<ParamDecl> parse_param();

    /**
     * parse_param_clause - Parses ("void" | param ("," param)*)?.
     * @function: The parent function declaration node.
     */
    void parse_param_clause(FunctionDecl& function);

    /**
     * parse_function - Parses "(" param_clause ")" (";" | compound_stmt). The return type and id are already parsed.
     * @decl: The parent declarator node.
     * Returns: The parsed function declaration node.
     */
    std::unique_ptr<FunctionDecl> parse_function(Declarator decl);

    /**
     * parse_struct_members - Parses type declarator ("," declarator)* ";".
     * @decl: The parent struct declaration node.
     */
    void parse_struct_members(StructDecl& decl);

    /**
     * parse_struct_def - Parses "struct" id "{" (struct_members)+ "}" ";".
     * Returns: The parsed struct declaration node.
     */
    std::unique_ptr<StructDecl> parse_struct_def();

    /**
     * parse_union_members - Parses type declarator ("," declarator)* ";".
     * @decl: The parent union declaration node.
     */
    void parse_union_members(UnionDecl& decl);

    /**
     * parse_union_def - Parses "union" id "{" (union_members)+ "}" ";".
     * Returns: The parsed union definition node.
     */
    std::unique_ptr<UnionDecl> parse_union_def();

    /**
     * parse_enumerator - Parses id ("=" expr)?.
     * Returns: The parsed enumerator node.
     */
    std::unique_ptr<EnumeratorDecl> parse_enumerator();

    /**
     * parse_enum_def - Parses "enum" id "{" enumerator ("," enumerator)* (",")? "}" ";".
     * Returns: The parsed enum definition node.
     */
    std::unique_ptr<EnumDecl> parse_enum_def();

    /* --------------------------------------------------
     * Helper functions
     * -------------------------------------------------- */

    /**
     * precedence - Gets the precedence level associated with @kind.
     * @kind: The token kind to associate by.
     * Returns: The precedence level.
     */
    int precedence(TokenKind kind);

    /**
     * binop - Gets the binary operator associated with @kind.
     * @kind: The token kind to associate by.
     * Returns: The associated binary operator.
     */
    BinOp binop(TokenKind kind);

    /**
     * isassignop - Checks if the current token is an assignment operator token.
     * @kind: The token kind to check against.
     * Returns: True if the current token is an assignment operator token.
     */
    bool isassignop(TokenKind kind);

    /**
     * assignop - Gets the assignment operator associated with @kind.
     * @kind: The token kind to associate by.
     * Returns: The associated assignment operator.
     */
    AssignOp assignop(TokenKind kind);

    /**
     * istype - Checks if the @offset-th lookahead is a type token.
     * @offset: The lookahead number, 0-based.
     * Returns: True if the @offset-th lookahead is a type token, false otherwise.
     */
    bool istype(std::size_t offset = 0);

    /**
     * cast - Checks if the current token is a cast token.
     * Returns: True if the current token is a cast token, false otherwise.
     */
    bool cast();

    /**
     * check - Checks if the @offset-th lookahead's kind matches @kind.
     * @kind: The token kind to match against.
     * @offset: The lookahead number, 0-based.
     * Returns: True if matched, false otherwise.
     */
    bool check(TokenKind kind, std::size_t offset = 0);

    /**
     * peek - Peeks forward @offset tokens.
     * @offset: The number of tokens to peek forward by.
     * Returns: The peeked token.
     */
    Token& peek(std::size_t offset = 0);

    /**
     * advance - Advances all lookahead tokens by one token.
     */
    void advance();

    /**
     * consume - Consumes the current token if its kind matches @kind. Errors if the current token's
     *           kind does not match @kind.
     * @kind: The token kind to match against.
     * Returns: The consumed token.
     */
    Token consume(TokenKind kind);

    /**
     * error - Reports an error message.
     * @msg: The error message to report.
     */
    [[noreturn]] void error(const char* msg);
};