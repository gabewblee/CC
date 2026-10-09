#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../include/ast.hpp"
#include "../include/parser.hpp"

Parser::Parser(Lexer lexer) : lexer_(std::move(lexer)) {
    lookahead_[0] = lexer_.next();
    lookahead_[1] = lexer_.next();
    lookahead_[2] = lexer_.next();
}

std::unique_ptr<Program> Parser::parse() {
    std::unique_ptr<Program> program = std::make_unique<Program>(peek().loc);
    while (!check(TokenKind::EndOfFile)) parse_external_decl(*program);
    consume(TokenKind::EndOfFile);
    return program;
}

/* --------------------------------------------------
 * Type
 * -------------------------------------------------- */

Type Parser::parse_type_base() {
    Token token = peek();
    TypeKind kind;
    switch (token.kind) {
        case TokenKind::Void:   kind = TypeKind::Void;   break;
        case TokenKind::Char:   kind = TypeKind::Char;   break;
        case TokenKind::Int:    kind = TypeKind::Int;    break;
        case TokenKind::Long:   kind = TypeKind::Long;   break;
        case TokenKind::Struct: kind = TypeKind::Struct; break;
        case TokenKind::Union:  kind = TypeKind::Union;  break;
        case TokenKind::Enum:   kind = TypeKind::Enum;   break;
        default:                error("expected type");
    }
    
    consume(token.kind);
    Type type(kind);
    if (kind == TypeKind::Struct || kind == TypeKind::Union || kind == TypeKind::Enum) type.tag = consume(TokenKind::Identifier).lexeme;
    return type;
}

Type Parser::parse_type() {
    Type type = parse_type_base();
    while (check(TokenKind::Star)) {
        consume(TokenKind::Star);
        type = Type::mk_ptr_to(std::move(type));
    }

    parse_array_suffix(type);
    return type;
}

Declarator Parser::parse_declarator(Type base) {
    while (check(TokenKind::Star)) {
        consume(TokenKind::Star);
        base = Type::mk_ptr_to(std::move(base));
    }

    Token id = consume(TokenKind::Identifier);
    parse_array_suffix(base);
    return Declarator{id.loc, std::move(id.lexeme), std::move(base)};
}

void Parser::parse_array_suffix(Type& type) {
    std::vector<std::size_t> dims;
    while (check(TokenKind::LeftBracket)) {
        consume(TokenKind::LeftBracket);
        Token sz = consume(TokenKind::Integer);
        dims.push_back(std::stoul(sz.lexeme));
        consume(TokenKind::RightBracket);
    }

    for (auto it = dims.rbegin(); it != dims.rend(); ++it) type = Type::mk_array_of(std::move(type), *it);
}

/* --------------------------------------------------
 * Expressions
 * -------------------------------------------------- */

std::unique_ptr<Expr> Parser::parse_expr() {
    return parse_assign_expr();
}

std::unique_ptr<Expr> Parser::parse_assign_expr() {
    std::unique_ptr<Expr> left = parse_bin_expr(1);
    if (!isassignop(peek().kind)) return left;
    TokenKind kind = peek().kind;
    Location loc = left->loc();
    consume(kind);
    std::unique_ptr<Expr> right = parse_assign_expr();
    return std::make_unique<AssignExpr>(loc, assignop(kind), std::move(left), std::move(right));
}

std::unique_ptr<Expr> Parser::parse_bin_expr(int min) {
    std::unique_ptr<Expr> left = parse_un_expr();
    while (true) {
        int prec = precedence(peek().kind);
        if (prec == 0 || prec < min) break;
        Token op = consume(peek().kind);
        std::unique_ptr<Expr> right = parse_bin_expr(prec + 1);
        Location loc = left->loc();
        left = std::make_unique<BinExpr>(loc, binop(op.kind), std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expr> Parser::parse_un_expr() {
    if (check(TokenKind::Sizeof)) return parse_sizeof_expr();
    if (cast()) return parse_cast_expr();
    Token token = peek();
    UnOp op;
    switch (token.kind) {
        case TokenKind::Plus:       op = UnOp::Plus;         break;
        case TokenKind::Minus:      op = UnOp::Negate;       break;
        case TokenKind::Bang:       op = UnOp::LogicalNot;   break;
        case TokenKind::Tilde:      op = UnOp::BitNot;       break;
        case TokenKind::Star:       op = UnOp::Dereference;  break;
        case TokenKind::Ampersand:  op = UnOp::AddressOf;    break;
        case TokenKind::PlusPlus:   op = UnOp::PreIncrement; break;
        case TokenKind::MinusMinus: op = UnOp::PreDecrement; break;
        default:                    return parse_postfix_expr();
    }

    consume(token.kind);
    std::unique_ptr<Expr> operand = parse_un_expr();
    return std::make_unique<UnExpr>(token.loc, op, std::move(operand));
}

std::unique_ptr<Expr> Parser::parse_sizeof_expr() {
    Token keyword = consume(TokenKind::Sizeof);
    if (check(TokenKind::LeftParenthesis) && istype(1)) {
        consume(TokenKind::LeftParenthesis);
        Type type = parse_type();
        consume(TokenKind::RightParenthesis);
        return std::make_unique<SizeofTypeExpr>(keyword.loc, std::move(type));
    }
    
    return std::make_unique<SizeofExpr>(keyword.loc, parse_un_expr());
}

std::unique_ptr<Expr> Parser::parse_cast_expr() {
    Token left = consume(TokenKind::LeftParenthesis);
    Type type = parse_type();
    consume(TokenKind::RightParenthesis);
    std::unique_ptr<Expr> operand = parse_un_expr();
    return std::make_unique<CastExpr>(left.loc, std::move(type), std::move(operand));
}

std::unique_ptr<Expr> Parser::parse_postfix_expr() {
    std::unique_ptr<Expr> expr = parse_primary_expr();
    while (true) {
        Location loc = expr->loc();
        if (check(TokenKind::LeftBracket)) {
            consume(TokenKind::LeftBracket);
            std::unique_ptr<Expr> index = parse_expr();
            consume(TokenKind::RightBracket);
            expr = std::make_unique<IndexExpr>(loc, std::move(expr), std::move(index));
            continue;
        }

        if (check(TokenKind::LeftParenthesis)) {
            consume(TokenKind::LeftParenthesis);
            std::unique_ptr<CallExpr> call = std::make_unique<CallExpr>(loc, std::move(expr));
            if (!check(TokenKind::RightParenthesis)) {
                while (true) {
                    call->add(parse_assign_expr());
                    if (!check(TokenKind::Comma)) break;
                    consume(TokenKind::Comma);
                }
            }

            consume(TokenKind::RightParenthesis);
            expr = std::move(call);
            continue;
        }

        if (check(TokenKind::Dot)) {
            consume(TokenKind::Dot);
            Token member = consume(TokenKind::Identifier);
            expr = std::make_unique<MemberExpr>(loc, std::move(expr), std::move(member.lexeme), false);
            continue;
        }

        if (check(TokenKind::Arrow)) {
            consume(TokenKind::Arrow);
            Token member = consume(TokenKind::Identifier);
            expr = std::make_unique<MemberExpr>(loc, std::move(expr), std::move(member.lexeme), true);
            continue;
        }

        if (check(TokenKind::PlusPlus)) {
            consume(TokenKind::PlusPlus);
            expr = std::make_unique<UnExpr>(loc, UnOp::PostIncrement, std::move(expr));
            continue;
        }

        if (check(TokenKind::MinusMinus)) {
            consume(TokenKind::MinusMinus);
            expr = std::make_unique<UnExpr>(loc, UnOp::PostDecrement, std::move(expr));
            continue;
        }

        break;
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parse_primary_expr() {
    if (check(TokenKind::Character)) {
        Token token = consume(TokenKind::Character);
        return std::make_unique<CharExpr>(token.loc, token.lexeme);
    }

    if (check(TokenKind::String)) {
        Token token = consume(TokenKind::String);
        return std::make_unique<StringExpr>(token.loc, token.lexeme);
    }

    if (check(TokenKind::Integer)) {
        Token token = consume(TokenKind::Integer);
        long val = std::stol(token.lexeme);
        return std::make_unique<IntExpr>(token.loc, val);
    }

    if (check(TokenKind::Identifier)) {
        Token token = consume(TokenKind::Identifier);
        return std::make_unique<IdExpr>(token.loc, token.lexeme);
    }

    if (check(TokenKind::LeftParenthesis)) {
        consume(TokenKind::LeftParenthesis);
        std::unique_ptr<Expr> expr = parse_expr();
        consume(TokenKind::RightParenthesis);
        return expr;
    }

    error("expected expression");
}

/* --------------------------------------------------
 * Statements
 * -------------------------------------------------- */

std::unique_ptr<Stmt> Parser::parse_stmt() {
    switch (peek().kind) {
        case TokenKind::LeftBrace: return parse_compound_stmt();
        case TokenKind::If:        return parse_if_stmt();
        case TokenKind::While:     return parse_while_stmt();
        case TokenKind::For:       return parse_for_stmt();
        case TokenKind::Switch:    return parse_switch_stmt();
        case TokenKind::Case:      return parse_case_stmt();
        case TokenKind::Default:   return parse_default_stmt();
        case TokenKind::Break:     return parse_break_stmt();
        case TokenKind::Continue:  return parse_continue_stmt();
        case TokenKind::Return:    return parse_ret_stmt();
        default:                   return parse_expr_stmt();
    }
}

std::unique_ptr<RetStmt> Parser::parse_ret_stmt() {
    Token keyword = consume(TokenKind::Return);
    std::unique_ptr<Expr> val;
    if (!check(TokenKind::Semicolon)) val = parse_expr();
    consume(TokenKind::Semicolon);
    return std::make_unique<RetStmt>(keyword.loc, std::move(val));
}

std::unique_ptr<CompoundStmt> Parser::parse_compound_stmt() {
    Token brace = consume(TokenKind::LeftBrace);
    std::unique_ptr<CompoundStmt> stmt = std::make_unique<CompoundStmt>(brace.loc);
    while (!check(TokenKind::RightBrace)) {
        if (check(TokenKind::EndOfFile)) error("expected '}'");
        if (istype()) parse_block_decl(*stmt);
        else          stmt->add(parse_stmt());
    }

    consume(TokenKind::RightBrace);
    return stmt;
}

std::unique_ptr<IfStmt> Parser::parse_if_stmt() {
    Token keyword = consume(TokenKind::If);
    consume(TokenKind::LeftParenthesis);
    std::unique_ptr<Expr> cond = parse_expr();
    consume(TokenKind::RightParenthesis);
    std::unique_ptr<Stmt> then_branch = parse_stmt();
    std::unique_ptr<Stmt> else_branch;
    if (check(TokenKind::Else)) {
        consume(TokenKind::Else);
        else_branch = parse_stmt();
    }

    return std::make_unique<IfStmt>(keyword.loc, std::move(cond), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<WhileStmt> Parser::parse_while_stmt() {
    Token keyword = consume(TokenKind::While);
    consume(TokenKind::LeftParenthesis);
    std::unique_ptr<Expr> cond = parse_expr();
    consume(TokenKind::RightParenthesis);
    std::unique_ptr<Stmt> body = parse_stmt();
    return std::make_unique<WhileStmt>(keyword.loc, std::move(cond), std::move(body));
}

std::unique_ptr<ForStmt> Parser::parse_for_stmt() {
    Token keyword = consume(TokenKind::For);
    consume(TokenKind::LeftParenthesis);
    std::vector<std::unique_ptr<VarDecl>> init_decls;
    std::unique_ptr<Expr> init_expr;
    if (!check(TokenKind::Semicolon)) {
        if (istype()) {
            Type base = parse_type_base();
            Declarator first = parse_declarator(base);
            init_decls = parse_var_decl(std::move(base), std::move(first), false);
        } else init_expr = parse_expr();
    }

    consume(TokenKind::Semicolon);
    std::unique_ptr<Expr> cond;
    if (!check(TokenKind::Semicolon)) cond = parse_expr();
    consume(TokenKind::Semicolon);
    std::unique_ptr<Expr> step;
    if (!check(TokenKind::RightParenthesis)) step = parse_expr();
    consume(TokenKind::RightParenthesis);
    std::unique_ptr<Stmt> body = parse_stmt();
    return std::make_unique<ForStmt>(keyword.loc, std::move(init_decls), std::move(init_expr), std::move(cond), std::move(step), std::move(body));
}

std::unique_ptr<SwitchStmt> Parser::parse_switch_stmt() {
    Token keyword = consume(TokenKind::Switch);
    consume(TokenKind::LeftParenthesis);
    std::unique_ptr<Expr> expr = parse_expr();
    consume(TokenKind::RightParenthesis);
    std::unique_ptr<Stmt> body = parse_stmt();
    return std::make_unique<SwitchStmt>(keyword.loc, std::move(expr), std::move(body));
}

std::unique_ptr<CaseStmt> Parser::parse_case_stmt() {
    Token keyword = consume(TokenKind::Case);
    std::unique_ptr<Expr> val = parse_expr();
    consume(TokenKind::Colon);
    std::unique_ptr<Stmt> stmt = parse_stmt();
    return std::make_unique<CaseStmt>(keyword.loc, std::move(val), std::move(stmt));
}

std::unique_ptr<DefaultStmt> Parser::parse_default_stmt() {
    Token keyword = consume(TokenKind::Default);
    consume(TokenKind::Colon);
    std::unique_ptr<Stmt> stmt = parse_stmt();
    return std::make_unique<DefaultStmt>(keyword.loc, std::move(stmt));
}

std::unique_ptr<BreakStmt> Parser::parse_break_stmt() {
    Token keyword = consume(TokenKind::Break);
    consume(TokenKind::Semicolon);
    return std::make_unique<BreakStmt>(keyword.loc);
}

std::unique_ptr<ContinueStmt> Parser::parse_continue_stmt() {
    Token keyword = consume(TokenKind::Continue);
    consume(TokenKind::Semicolon);
    return std::make_unique<ContinueStmt>(keyword.loc);
}

std::unique_ptr<ExprStmt> Parser::parse_expr_stmt() {
    Location loc = peek().loc;
    if (check(TokenKind::Semicolon)) {
        consume(TokenKind::Semicolon);
        return std::make_unique<ExprStmt>(loc, nullptr);
    }

    std::unique_ptr<Expr> expr = parse_expr();
    consume(TokenKind::Semicolon);
    return std::make_unique<ExprStmt>(loc, std::move(expr));
}

/* --------------------------------------------------
 * Declarations
 * -------------------------------------------------- */

void Parser::parse_external_decl(Program& program) {
    if (check(TokenKind::Struct) && check(TokenKind::Identifier, 1) && check(TokenKind::LeftBrace, 2)) {
        program.add(parse_struct_def());
        return;
    }

    if (check(TokenKind::Union) && check(TokenKind::Identifier, 1) && check(TokenKind::LeftBrace, 2)) {
        program.add(parse_union_def());
        return;
    }

    if (check(TokenKind::Enum) && check(TokenKind::Identifier, 1) && check(TokenKind::LeftBrace, 2)) {
        program.add(parse_enum_def());
        return;
    }

    Type base = parse_type_base();
    Declarator decl = parse_declarator(base);
    if (check(TokenKind::LeftParenthesis)) {
        if (decl.type.kind == TypeKind::Array) error("expected non-array return type");
        program.add(parse_function(std::move(decl)));
        return;
    }

    std::vector<std::unique_ptr<VarDecl>> vars = parse_var_decl(std::move(base), std::move(decl), true);
    for (std::unique_ptr<VarDecl>& var : vars) program.add(std::move(var));
}

void Parser::parse_block_decl(CompoundStmt& block) {
    if (check(TokenKind::Struct) && check(TokenKind::Identifier, 1) && check(TokenKind::LeftBrace, 2)) {
        block.add(parse_struct_def());
        return;
    }

    if (check(TokenKind::Union) && check(TokenKind::Identifier, 1) && check(TokenKind::LeftBrace, 2)) {
        block.add(parse_union_def());
        return;
    }

    if (check(TokenKind::Enum) && check(TokenKind::Identifier, 1) && check(TokenKind::LeftBrace, 2)) {
        block.add(parse_enum_def());
        return;
    }

    Type base = parse_type_base();
    Declarator first = parse_declarator(base);
    std::vector<std::unique_ptr<VarDecl>> vars = parse_var_decl(std::move(base), std::move(first), true);
    for (std::unique_ptr<VarDecl>& var : vars) block.add(std::move(var));
}

std::vector<std::unique_ptr<VarDecl>> Parser::parse_var_decl(Type base, Declarator first, bool semicolon) {
    std::vector<std::unique_ptr<VarDecl>> vars;
    Declarator decl = std::move(first);
    while (true) {
        std::unique_ptr<Expr> init;
        if (check(TokenKind::Equal)) {
            consume(TokenKind::Equal);
            init = parse_assign_expr();
        }

        vars.push_back(std::make_unique<VarDecl>(decl.loc, std::move(decl.id), std::move(decl.type), std::move(init)));
        if (!check(TokenKind::Comma)) break;
        consume(TokenKind::Comma);
        decl = parse_declarator(base);
    }

    if (semicolon) consume(TokenKind::Semicolon);
    return vars;
}

std::unique_ptr<ParamDecl> Parser::parse_param() {
    Type base = parse_type_base();
    Declarator decl = parse_declarator(std::move(base));
    return std::make_unique<ParamDecl>(decl.loc, std::move(decl.type), std::move(decl.id));
}

void Parser::parse_param_clause(FunctionDecl& function) {
    if (check(TokenKind::RightParenthesis)) return;

    if (check(TokenKind::Void) && check(TokenKind::RightParenthesis, 1)) {
        consume(TokenKind::Void);
        return;
    }

    while (true) {
        function.add(parse_param());
        if (!check(TokenKind::Comma)) break;
        consume(TokenKind::Comma);
    }
}

std::unique_ptr<FunctionDecl> Parser::parse_function(Declarator decl) {
    std::unique_ptr<FunctionDecl> function = std::make_unique<FunctionDecl>(decl.loc, std::move(decl.type), std::move(decl.id));
    consume(TokenKind::LeftParenthesis);
    parse_param_clause(*function);
    consume(TokenKind::RightParenthesis);
    if (check(TokenKind::Semicolon)) {
        consume(TokenKind::Semicolon);
        return function;
    }

    if (check(TokenKind::LeftBrace)) {
        function->add(parse_compound_stmt());
        return function;
    }

    error("expected ';' or function body");
}

void Parser::parse_struct_members(StructDecl& decl) {
    Type base = parse_type_base();
    while (true) {
        Declarator field = parse_declarator(base);
        decl.add(std::make_unique<FieldDecl>(field.loc, std::move(field.type), std::move(field.id)));
        if (!check(TokenKind::Comma)) break;
        consume(TokenKind::Comma);
    }

    consume(TokenKind::Semicolon);
}

std::unique_ptr<StructDecl> Parser::parse_struct_def() {
    Token keyword = consume(TokenKind::Struct);
    Token id = consume(TokenKind::Identifier);
    std::unique_ptr<StructDecl> decl = std::make_unique<StructDecl>(keyword.loc, id.lexeme);
    consume(TokenKind::LeftBrace);
    if (check(TokenKind::RightBrace)) error("expected struct member");
    while (!check(TokenKind::RightBrace)) parse_struct_members(*decl);
    consume(TokenKind::RightBrace);
    consume(TokenKind::Semicolon);
    return decl;
}

void Parser::parse_union_members(UnionDecl& decl) {
    Type base = parse_type_base();
    while (true) {
        Declarator field = parse_declarator(base);
        decl.add(std::make_unique<FieldDecl>(field.loc, std::move(field.type), std::move(field.id)));
        if (!check(TokenKind::Comma)) break;
        consume(TokenKind::Comma);
    }

    consume(TokenKind::Semicolon);
}

std::unique_ptr<UnionDecl> Parser::parse_union_def() {
    Token keyword = consume(TokenKind::Union);
    Token id = consume(TokenKind::Identifier);
    std::unique_ptr<UnionDecl> decl = std::make_unique<UnionDecl>(keyword.loc, id.lexeme);
    consume(TokenKind::LeftBrace);
    if (check(TokenKind::RightBrace)) error("expected union member");
    while (!check(TokenKind::RightBrace)) parse_union_members(*decl);
    consume(TokenKind::RightBrace);
    consume(TokenKind::Semicolon);
    return decl;
}

std::unique_ptr<EnumeratorDecl> Parser::parse_enumerator() {
    Token id = consume(TokenKind::Identifier);
    std::unique_ptr<Expr> val;
    if (check(TokenKind::Equal)) {
        consume(TokenKind::Equal);
        val = parse_expr();
    }

    return std::make_unique<EnumeratorDecl>(id.loc, id.lexeme, std::move(val));
}

std::unique_ptr<EnumDecl> Parser::parse_enum_def() {
    Token keyword = consume(TokenKind::Enum);
    Token id = consume(TokenKind::Identifier);
    std::unique_ptr<EnumDecl> decl = std::make_unique<EnumDecl>(keyword.loc, id.lexeme);
    consume(TokenKind::LeftBrace);
    decl->add(parse_enumerator());
    while (check(TokenKind::Comma)) {
        consume(TokenKind::Comma);
        if (check(TokenKind::RightBrace)) break;
        decl->add(parse_enumerator());
    }

    consume(TokenKind::RightBrace);
    consume(TokenKind::Semicolon);
    return decl;
}

/* --------------------------------------------------
 * Helper functions
 * -------------------------------------------------- */

int Parser::precedence(TokenKind kind) {
    switch (kind) {
        case TokenKind::PipePipe:           return 1;
        case TokenKind::AmpersandAmpersand: return 2;
        case TokenKind::Pipe:               return 3;
        case TokenKind::Caret:              return 4;
        case TokenKind::Ampersand:          return 5;
        case TokenKind::EqualEqual:
        case TokenKind::BangEqual:          return 6;
        case TokenKind::Less:
        case TokenKind::LessEqual:
        case TokenKind::Greater:
        case TokenKind::GreaterEqual:       return 7;
        case TokenKind::LessLess:
        case TokenKind::GreaterGreater:     return 8;
        case TokenKind::Plus:
        case TokenKind::Minus:              return 9;
        case TokenKind::Star:
        case TokenKind::Slash:
        case TokenKind::Percent:            return 10;
        default:                            return 0;
    }
}

BinOp Parser::binop(TokenKind kind) {
    switch (kind) {
        case TokenKind::Plus:               return BinOp::Add;
        case TokenKind::Minus:              return BinOp::Subtract;
        case TokenKind::Star:               return BinOp::Multiply;
        case TokenKind::Slash:              return BinOp::Divide;
        case TokenKind::Percent:            return BinOp::Modulo;
        case TokenKind::LessLess:           return BinOp::ShiftLeft;
        case TokenKind::GreaterGreater:     return BinOp::ShiftRight;
        case TokenKind::Less:               return BinOp::Less;
        case TokenKind::LessEqual:          return BinOp::LessEqual;
        case TokenKind::Greater:            return BinOp::Greater;
        case TokenKind::GreaterEqual:       return BinOp::GreaterEqual;
        case TokenKind::EqualEqual:         return BinOp::Equal;
        case TokenKind::BangEqual:          return BinOp::NotEqual;
        case TokenKind::Ampersand:          return BinOp::BitAnd;
        case TokenKind::Caret:              return BinOp::BitXor;
        case TokenKind::Pipe:               return BinOp::BitOr;
        case TokenKind::AmpersandAmpersand: return BinOp::LogicalAnd;
        case TokenKind::PipePipe:           return BinOp::LogicalOr;
        default:                            std::abort();
    }
}

bool Parser::isassignop(TokenKind kind) {
    switch (kind) {
        case TokenKind::Equal:
        case TokenKind::PlusEqual:
        case TokenKind::MinusEqual:
        case TokenKind::StarEqual:
        case TokenKind::SlashEqual:
        case TokenKind::PercentEqual:
        case TokenKind::LessLessEqual:
        case TokenKind::GreaterGreaterEqual:
        case TokenKind::AmpersandEqual:
        case TokenKind::CaretEqual:
        case TokenKind::PipeEqual: return true;
        default:                   return false;
    }
}

AssignOp Parser::assignop(TokenKind kind) {
    switch (kind) {
        case TokenKind::Equal:               return AssignOp::Assign;
        case TokenKind::PlusEqual:           return AssignOp::AddAssign;
        case TokenKind::MinusEqual:          return AssignOp::SubAssign;
        case TokenKind::StarEqual:           return AssignOp::MulAssign;
        case TokenKind::SlashEqual:          return AssignOp::DivAssign;
        case TokenKind::PercentEqual:        return AssignOp::ModAssign;
        case TokenKind::LessLessEqual:       return AssignOp::LeftShiftAssign;
        case TokenKind::GreaterGreaterEqual: return AssignOp::RightShiftAssign;
        case TokenKind::AmpersandEqual:      return AssignOp::AndAssign;
        case TokenKind::CaretEqual:          return AssignOp::XorAssign;
        case TokenKind::PipeEqual:           return AssignOp::OrAssign;
        default:                             std::abort();
    }
}

bool Parser::istype(std::size_t offset) {
    TokenKind kind = peek(offset).kind;
    return kind == TokenKind::Void   ||
           kind == TokenKind::Char   ||
           kind == TokenKind::Int    ||
           kind == TokenKind::Long   ||
           kind == TokenKind::Struct ||
           kind == TokenKind::Union  ||
           kind == TokenKind::Enum;
}

bool Parser::cast() {
    return check(TokenKind::LeftParenthesis) && istype(1);
}

bool Parser::check(TokenKind kind, std::size_t offset) {
    return peek(offset).kind == kind;
}

Token& Parser::peek(std::size_t offset) {
    if (offset >= lookahead_.size()) std::abort();
    return lookahead_[offset];
}

void Parser::advance() {
    lookahead_[0] = std::move(lookahead_[1]);
    lookahead_[1] = std::move(lookahead_[2]);
    lookahead_[2] = lexer_.next();
}

Token Parser::consume(TokenKind kind) {
    if (!check(kind)) {
        std::fprintf(stderr, "%zu:%zu: error: unexpected token '%s', expected %s\n", peek().loc.line, peek().loc.col, peek().lexeme.c_str(), describe(kind).c_str());
        std::exit(1);
    }

    Token token = std::move(lookahead_[0]);
    advance();
    return token;
}

[[noreturn]] void Parser::error(const char* msg) {
    std::fprintf(stderr, "%zu:%zu: error: %s, got '%s'\n", peek().loc.line, peek().loc.col, msg, peek().lexeme.c_str());
    std::exit(1);
}