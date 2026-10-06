#include "../include/ast.hpp"
#include "../include/parser.hpp"

Parser::Parser(Lexer lexer) : lexer_(std::move(lexer)) {
    lookahead_[0] = lexer_.next();
    lookahead_[1] = lexer_.next();
    lookahead_[2] = lexer_.next();
}

std::unique_ptr<Program> Parser::parse() {
    std::unique_ptr<Program> program = std::make_unique<Program>(lookahead_[0].loc);
    while (!check(TokenKind::EndOfFile))
        program->add(parse_decl());
    
    return program;
}

/* --------------------------------------------------
 * Type
 * -------------------------------------------------- */

Type Parser::parse_type() {
    Type type{};
    type.depth = 0;
    if (check(TokenKind::Int)) {
        advance();
        type.kind = TypeKind::Int;
    } else if (check(TokenKind::Char)) {
        advance();
        type.kind = TypeKind::Char;
    } else if (check(TokenKind::Long)) {
        advance();
        type.kind = TypeKind::Long;
    } else if (check(TokenKind::Void)) {
        advance();
        type.kind = TypeKind::Void;
    } else if (check(TokenKind::Struct)) {
        advance();
        type.kind = TypeKind::Struct;
        type.tag  = consume(TokenKind::Identifier).lexeme;
    } else if (check(TokenKind::Union)) {
        advance();
        type.kind = TypeKind::Union;
        type.tag  = consume(TokenKind::Identifier).lexeme;
    } else if (check(TokenKind::Enum)) {
        advance();
        type.kind = TypeKind::Enum;
        type.tag  = consume(TokenKind::Identifier).lexeme;
    } else {
        std::fprintf(stderr, "%zu:%zu: error: unexpected token '%s'\n", lookahead_[0].loc.line, lookahead_[0].loc.col, lookahead_[0].lexeme.c_str());
        std::exit(1);
    }

    while (check(TokenKind::Star)) {
        advance();
        type.depth++;
    }
    return type;
}

/* --------------------------------------------------
 * Expressions
 * -------------------------------------------------- */

std::unique_ptr<Expr> Parser::parse_expr() {
    return parse_assign_expr();
}

std::unique_ptr<Expr> Parser::parse_assign_expr() {
    std::unique_ptr<Expr> left = parse_bin_expr(1);
    if (!check(TokenKind::Equal))
        return left;

    struct Location loc = left->loc();
    consume(TokenKind::Equal);
    std::unique_ptr<Expr> right = parse_assign_expr();
    return std::make_unique<AssignExpr>(loc, AssignOp::Assign, std::move(left), std::move(right));
}

std::unique_ptr<Expr> Parser::parse_bin_expr(int min) {
    std::unique_ptr<Expr> left = parse_un_expr();
    while (true) {
        int prec = precedence(peek().kind);
        if (prec == 0 || prec < min)
            break;

        Token op = peek();
        advance();
        std::unique_ptr<Expr> right = parse_bin_expr(prec + 1);
        struct Location loc = left->loc();
        left = std::make_unique<BinExpr>(loc, binop(op.kind), std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::parse_un_expr() {
    Token token = peek();
    UnOp op;
    switch (token.kind) {
    case TokenKind::Plus:
        op = UnOp::Plus;
        break;
    case TokenKind::Minus:
        op = UnOp::Negate;
        break;
    case TokenKind::Bang:
        op = UnOp::LogicalNot;
        break;
    case TokenKind::Tilde:
        op = UnOp::BitNot;
        break;
    case TokenKind::Star:
        op = UnOp::Dereference;
        break;
    case TokenKind::Ampersand:
        op = UnOp::AddressOf;
        break;
    case TokenKind::PlusPlus:
        op = UnOp::PreIncrement;
        break;
    case TokenKind::MinusMinus:
        op = UnOp::PreDecrement;
        break;
    default:
        return parse_postfix_expr();
    }

    advance();
    std::unique_ptr<Expr> operand = parse_un_expr();
    return std::make_unique<UnExpr>(token.loc, op, std::move(operand));
}

std::unique_ptr<Expr> Parser::parse_postfix_expr() {
    std::unique_ptr<Expr> expr = parse_primary_expr();
    while (true) {
        struct Location loc = expr->loc();

        /* Function call */
        if (check(TokenKind::LeftParenthesis)) {
            consume(TokenKind::LeftParenthesis);
            std::unique_ptr<CallExpr> call = std::make_unique<CallExpr>(loc, std::move(expr));
            if (!check(TokenKind::RightParenthesis)) {
                while (true) {
                    call->add(parse_assign_expr());
                    if (!check(TokenKind::Comma))
                        break;

                    consume(TokenKind::Comma);
                }
            }

            consume(TokenKind::RightParenthesis);
            expr = std::move(call);
            continue;
        }

        /* Array indexing */
        if (check(TokenKind::LeftBracket)) {
            consume(TokenKind::LeftBracket);
            std::unique_ptr<Expr> index = parse_expr();
            consume(TokenKind::RightBracket);
            expr = std::make_unique<IndexExpr>(loc, std::move(expr), std::move(index));
            continue;
        }

        /* Struct member */
        if (check(TokenKind::Dot)) {
            consume(TokenKind::Dot);
            Token member = consume(TokenKind::Identifier);
            expr = std::make_unique<MemberExpr>(loc, std::move(expr), member.lexeme, false);
            continue;
        }

        /* Pointer-to-struct member */
        if (check(TokenKind::Arrow)) {
            consume(TokenKind::Arrow);
            Token member = consume(TokenKind::Identifier);
            expr = std::make_unique<MemberExpr>(loc, std::move(expr), member.lexeme, true);
            continue;
        }

        /* Postfix ++ */
        if (check(TokenKind::PlusPlus)) {
            consume(TokenKind::PlusPlus);
            expr = std::make_unique<UnExpr>(loc, UnOp::PostIncrement, std::move(expr));
            continue;
        }

        /* Postfix -- */
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
    if (check(TokenKind::Integer)) {
        Token token = consume(TokenKind::Integer);
        long val;
        try {
            val = std::stol(token.lexeme);
        } catch (...) {
            std::fprintf(stderr, "%zu:%zu: error: integer '%s' out of range\n", token.loc.line, token.loc.col, token.lexeme.c_str());
            std::exit(1);
        }

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

    Token& token = peek();
    std::fprintf(stderr, "%zu:%zu: error: expected expression, got '%s'\n", token.loc.line, token.loc.col, token.lexeme.c_str());
    std::exit(1);
}

/* --------------------------------------------------
 * Statements
 * -------------------------------------------------- */

std::unique_ptr<Stmt> Parser::parse_stmt() {
    if (check(TokenKind::Return))
        return parse_ret_stmt();

    if (check(TokenKind::LeftBrace))
        return parse_compound_stmt();

    if (check(TokenKind::If))
        return parse_if_stmt();

    if (check(TokenKind::While))
        return parse_while_stmt();

    if (check(TokenKind::Break))
        return parse_break_stmt();

    if (check(TokenKind::Continue))
        return parse_continue_stmt();

    return parse_expr_stmt();
}

std::unique_ptr<RetStmt> Parser::parse_ret_stmt() {
    struct Location loc = lookahead_[0].loc;
    consume(TokenKind::Return);
    std::unique_ptr<Expr> expr;
    if (!check(TokenKind::Semicolon))
        expr = parse_expr();

    consume(TokenKind::Semicolon);
    return std::make_unique<RetStmt>(loc, std::move(expr));
}

std::unique_ptr<CompoundStmt> Parser::parse_compound_stmt() {
    struct Location loc = lookahead_[0].loc;
    consume(TokenKind::LeftBrace);
    std::unique_ptr<CompoundStmt> stmt = std::make_unique<CompoundStmt>(loc);
    while (!check(TokenKind::RightBrace)) {
        if (decl())
            stmt->add(parse_decl());
        else
            stmt->add(parse_stmt());
    }

    consume(TokenKind::RightBrace);
    return stmt;
}

std::unique_ptr<IfStmt> Parser::parse_if_stmt() {
    struct Location loc = lookahead_[0].loc;
    consume(TokenKind::If);
    consume(TokenKind::LeftParenthesis);
    std::unique_ptr<Expr> cond = parse_expr();
    consume(TokenKind::RightParenthesis);
    std::unique_ptr<Stmt> then_branch = parse_stmt();
    std::unique_ptr<Stmt> else_branch;
    if (check(TokenKind::Else)) {
        advance();
        else_branch = parse_stmt();
    }

    return std::make_unique<IfStmt>(loc, std::move(cond), std::move(then_branch), std::move(else_branch));
}   

std::unique_ptr<WhileStmt> Parser::parse_while_stmt() {
    struct Location loc = lookahead_[0].loc;
    consume(TokenKind::While);
    consume(TokenKind::LeftParenthesis);
    std::unique_ptr<Expr> cond = parse_expr();
    consume(TokenKind::RightParenthesis);
    std::unique_ptr<Stmt> body = parse_stmt();
    return std::make_unique<WhileStmt>(loc, std::move(cond), std::move(body));
}

std::unique_ptr<BreakStmt> Parser::parse_break_stmt() {
    struct Location loc = lookahead_[0].loc;
    consume(TokenKind::Break);
    consume(TokenKind::Semicolon);
    return std::make_unique<BreakStmt>(loc);
}

std::unique_ptr<ContinueStmt> Parser::parse_continue_stmt() {
    struct Location loc = lookahead_[0].loc;
    consume(TokenKind::Continue);
    consume(TokenKind::Semicolon);
    return std::make_unique<ContinueStmt>(loc);
}

std::unique_ptr<ExprStmt> Parser::parse_expr_stmt() {
    struct Location loc = lookahead_[0].loc;
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

std::unique_ptr<Decl> Parser::parse_decl() {
    if (check(TokenKind::Struct) && check(TokenKind::Identifier, 1) && check(TokenKind::LeftBrace, 2))
        return parse_struct_def();
    
    if (check(TokenKind::Enum) && check(TokenKind::Identifier, 1) && check(TokenKind::LeftBrace, 2))
        return parse_enum_def();

    Type type = parse_type();
    Token id = consume(TokenKind::Identifier);
    if (check(TokenKind::LeftParenthesis))
        return parse_function(id.loc, std::move(type), id.lexeme);
    return parse_var_decl(id.loc, std::move(type), id.lexeme);
}

std::unique_ptr<VarDecl> Parser::parse_var_decl(struct Location loc, Type type, std::string id) {
    /* Arrays */
    while (check(TokenKind::LeftBracket)) {
        consume(TokenKind::LeftBracket);
        Token sz = consume(TokenKind::Integer);
        type.dimensions.push_back(std::stoul(sz.lexeme));
        consume(TokenKind::RightBracket);
    }

    /* "=" expr? */
    std::unique_ptr<Expr> init;
    if (check(TokenKind::Equal)) {
        consume(TokenKind::Equal);
        init = parse_expr();
    }

    consume(TokenKind::Semicolon);
    return std::make_unique<VarDecl>(loc, std::move(id), std::move(type), std::move(init));
}

std::unique_ptr<ParamDecl> Parser::parse_param_decl() {
    Type type = parse_type();
    Token id = consume(TokenKind::Identifier);
    return std::make_unique<ParamDecl>(id.loc, std::move(type), id.lexeme);
}

std::unique_ptr<FunctionDecl> Parser::parse_function(struct Location loc, Type type, std::string id) {
    std::unique_ptr<FunctionDecl> function = std::make_unique<FunctionDecl>(loc, type, id);
    consume(TokenKind::LeftParenthesis);
    if (check(TokenKind::Void) && check(TokenKind::RightParenthesis, 1)) {
        /* Edge case: "void" ")" */
        consume(TokenKind::Void);
        goto end;
    }

    while (!check(TokenKind::RightParenthesis)) {
        function->add(parse_param_decl());

        if (!check(TokenKind::Comma))
            break;

        consume(TokenKind::Comma);
    }

end:
    consume(TokenKind::RightParenthesis);
    if (check(TokenKind::Semicolon)) {
        consume(TokenKind::Semicolon);
        return function;
    }

    if (check(TokenKind::LeftBrace)) {
        function->add(parse_compound_stmt());
        return function;
    }

    std::fprintf(stderr, "%zu:%zu: error: expected ';' or '{' after function declaration\n", lookahead_[0].loc.line, lookahead_[0].loc.col);
    std::exit(1);
}

std::unique_ptr<FieldDecl> Parser::parse_field_decl() {
    Type type = parse_type();
    Token id = consume(TokenKind::Identifier);
    consume(TokenKind::Semicolon);
    return std::make_unique<FieldDecl>(id.loc, std::move(type), id.lexeme);
}

std::unique_ptr<StructDecl> Parser::parse_struct_def() {
    Token keyword = consume(TokenKind::Struct);
    Token id = consume(TokenKind::Identifier);
    std::unique_ptr<StructDecl> decl = std::make_unique<StructDecl>(keyword.loc, id.lexeme);
    consume(TokenKind::LeftBrace);
    while (!check(TokenKind::RightBrace))
        decl->add(parse_field_decl());

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
    consume(TokenKind::LeftBrace);
    std::unique_ptr<EnumDecl> decl = std::make_unique<EnumDecl>(keyword.loc, id.lexeme);
    while (!check(TokenKind::RightBrace)) {
        decl->add(parse_enumerator());
        if (!check(TokenKind::Comma))
            break;

        consume(TokenKind::Comma);
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
        case TokenKind::PipePipe:           return BinOp::LogicalOr;
        case TokenKind::AmpersandAmpersand: return BinOp::LogicalAnd;
        case TokenKind::Pipe:               return BinOp::BitOr;
        case TokenKind::Caret:              return BinOp::BitXor;
        case TokenKind::Ampersand:          return BinOp::BitAnd;
        case TokenKind::EqualEqual:         return BinOp::Equal;
        case TokenKind::BangEqual:          return BinOp::NotEqual;
        case TokenKind::Less:               return BinOp::Less;
        case TokenKind::LessEqual:          return BinOp::LessEqual;
        case TokenKind::Greater:            return BinOp::Greater;
        case TokenKind::GreaterEqual:       return BinOp::GreaterEqual;
        case TokenKind::LessLess:           return BinOp::ShiftLeft;
        case TokenKind::GreaterGreater:     return BinOp::ShiftRight;
        case TokenKind::Plus:               return BinOp::Add;
        case TokenKind::Minus:              return BinOp::Subtract;
        case TokenKind::Star:               return BinOp::Multiply;
        case TokenKind::Slash:              return BinOp::Divide;
        case TokenKind::Percent:            return BinOp::Modulo;
        default:                            std::abort();
    }
}

void Parser::advance() {
    lookahead_[0] = std::move(lookahead_[1]);
    lookahead_[1] = std::move(lookahead_[2]);
    lookahead_[2] = lexer_.next();
}

bool Parser::decl() {
    return check(TokenKind::Int)
        || check(TokenKind::Char)
        || check(TokenKind::Long)
        || check(TokenKind::Void)
        || check(TokenKind::Struct)
        || check(TokenKind::Union)
        || check(TokenKind::Enum);
}

bool Parser::check(TokenKind kind, std::size_t offset) {
    return peek(offset).kind == kind;
}

Token& Parser::peek(std::size_t offset) {
    if (offset >= lookahead_.size())
        std::abort();

    return lookahead_[offset];
}

Token Parser::consume(TokenKind kind) {
    if (!check(kind)) {
        std::fprintf(stderr, "%zu:%zu: error: unexpected token '%s'\n", lookahead_[0].loc.line, lookahead_[0].loc.col, lookahead_[0].lexeme.c_str());
        std::exit(1);
    }

    Token cur = lookahead_[0];
    advance();
    return cur;
}