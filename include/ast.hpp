#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "token.hpp"

/* --------------------------------------------------
 * Types
 * -------------------------------------------------- */

enum class TypeKind {
    Char,   /* "char"   */
    Enum,   /* "enum"   */
    Int,    /* "int"    */
    Long,   /* "long"   */
    Struct, /* "struct" */
    Union,  /* "union"  */
    Void,   /* "void"   */
};

class Type {
public:
    TypeKind                 kind;       /* The type kind, e.g. TypeKind::Struct for structs   */
    std::string              tag;        /* The type tag, e.g. "foo" for "struct foo"          */
    std::size_t              depth;      /* The type depth, e.g. 2 for int**                   */
    std::vector<std::size_t> dimensions; /* The type dimensions, e.g. {10, 20} for 10x20 array */
};

class Declarator {
public:
    struct Location loc;
    std::string     id;
    Type            type;
};

/* --------------------------------------------------
 * Node
 * -------------------------------------------------- */

class Node {
public:
    Node(struct Location loc);
    virtual ~Node() = default;
    struct Location loc();

protected:
    struct Location loc_;
};

/* --------------------------------------------------
 * Block node
 * -------------------------------------------------- */

class BlockEntry : public Node {
public:
    BlockEntry(struct Location loc);
    virtual ~BlockEntry() = default;
};

/* --------------------------------------------------
 * Expression nodes
 * -------------------------------------------------- */

class Expr : public Node {
public:
    Expr(struct Location loc);
    virtual ~Expr() = default;
};

class CharExpr : public Expr {
public:
    CharExpr(struct Location loc, std::string val);
    std::string& val();

private:
    std::string val_;
};

class StringExpr : public Expr {
public:
    StringExpr(struct Location loc, std::string val);
    std::string& val();

private:
    std::string val_;
};

class IntExpr : public Expr {
public:
    IntExpr(struct Location loc, long val);
    long val();

private:
    long val_;
};

class IdExpr : public Expr {
public:
    IdExpr(struct Location loc, std::string id);
    std::string& id();

private:
    std::string id_;
};

class SizeofExpr : public Expr {
public:
    SizeofExpr(struct Location loc, std::unique_ptr<Expr> operand);

private:
    std::unique_ptr<Expr> operand_;
};

class SizeofTypeExpr : public Expr {
public:
    SizeofTypeExpr(struct Location loc, Type type);

private:
    Type type_;
};

class CastExpr : public Expr {
public:
    CastExpr(struct Location loc, Type type, std::unique_ptr<Expr> operand);

private:
    Type type_;
    std::unique_ptr<Expr> operand_;
};

enum class BinOp {
    Add,          /* "+"  */
    Divide,       /* "/"  */
    Modulo,       /* "%"  */
    Multiply,     /* "*"  */
    Subtract,     /* "-"  */

    Equal,        /* "==" */
    Greater,      /* ">"  */
    GreaterEqual, /* ">=" */
    Less,         /* "<"  */
    LessEqual,    /* "<=" */
    NotEqual,     /* "!=" */

    LogicalAnd,   /* "&&" */
    LogicalOr,    /* "||" */

    BitAnd,       /* "&"  */
    BitOr,        /* "|"  */
    BitXor,       /* "^"  */

    ShiftLeft,    /* "<<" */
    ShiftRight,   /* ">>" */
};

class BinExpr : public Expr {
public:
    BinExpr(struct Location loc, BinOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);
    BinOp op();
    Expr *left();
    Expr *right();

private:
    BinOp                 op_;
    std::unique_ptr<Expr> left_;
    std::unique_ptr<Expr> right_;
};

enum class UnOp {
    AddressOf,     /* "&"  */
    BitNot,        /* "~"  */
    Dereference,   /* "*"  */
    LogicalNot,    /* "!"  */
    Negate,        /* "-"  */
    Plus,          /* "+"  */
    PostDecrement, /* "--" */
    PostIncrement, /* "++" */
    PreDecrement,  /* "--" */
    PreIncrement,  /* "++" */
};

class UnExpr : public Expr {
public:
    UnExpr(struct Location loc, UnOp op, std::unique_ptr<Expr> operand);

private:
    UnOp                  op_;
    std::unique_ptr<Expr> operand_;
};

enum class AssignOp {
    AddAssign,        /* "+="  */
    AndAssign,        /* "&="  */
    Assign,           /* "="   */
    DivAssign,        /* "/="  */
    LeftShiftAssign,  /* "<<=" */
    ModAssign,        /* "%="  */
    MulAssign,        /* "*="  */
    OrAssign,         /* "|="  */
    RightShiftAssign, /* ">>=" */
    SubAssign,        /* "-="  */
    XorAssign,        /* "^="  */
};

class AssignExpr : public Expr {
public:
    AssignExpr(struct Location loc, AssignOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);

private:
    AssignOp              op_;
    std::unique_ptr<Expr> left_;
    std::unique_ptr<Expr> right_;
};

class CallExpr : public Expr {
public:
    CallExpr(struct Location loc, std::unique_ptr<Expr> callee);
    void add(std::unique_ptr<Expr> arg);

private:
    std::unique_ptr<Expr>              callee_;
    std::vector<std::unique_ptr<Expr>> args_;
};

class IndexExpr : public Expr {
public:
    IndexExpr(struct Location loc, std::unique_ptr<Expr> id, std::unique_ptr<Expr> index);

private:
    std::unique_ptr<Expr> id_;
    std::unique_ptr<Expr> index_;
};

class MemberExpr : public Expr {
public:
    MemberExpr(struct Location loc, std::unique_ptr<Expr> id, std::string member, bool ptr);

private:
    std::unique_ptr<Expr> id_;
    std::string           member_;
    bool                  ptr_;
};

/* --------------------------------------------------
 * Statement nodes
 * -------------------------------------------------- */

class VarDecl;

class Stmt : public BlockEntry {
public:
    Stmt(struct Location loc);
    virtual ~Stmt() = default;
};

class RetStmt : public Stmt {
public:
    RetStmt(struct Location loc, std::unique_ptr<Expr> val);

private:
    std::unique_ptr<Expr> val_;
};

class CompoundStmt : public Stmt {
public:
    CompoundStmt(struct Location loc);
    void add(std::unique_ptr<BlockEntry> entry);

private:
    std::vector<std::unique_ptr<BlockEntry>> entries_;
};

class IfStmt : public Stmt {
public:
    IfStmt(struct Location loc, std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch);

private:
    std::unique_ptr<Expr> cond_;
    std::unique_ptr<Stmt> then_branch_;
    std::unique_ptr<Stmt> else_branch_;
};

class SwitchStmt : public Stmt {
public:
    SwitchStmt(struct Location loc, std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body);

private:
    std::unique_ptr<Expr> expr_;
    std::unique_ptr<Stmt> body_;
};

class CaseStmt : public Stmt {
public:
    CaseStmt(struct Location loc, std::unique_ptr<Expr> val, std::unique_ptr<Stmt> stmt);

private:
    std::unique_ptr<Expr> val_;
    std::unique_ptr<Stmt> stmt_;
};

class DefaultStmt : public Stmt {
public:
    DefaultStmt(struct Location loc, std::unique_ptr<Stmt> stmt);

private:
    std::unique_ptr<Stmt> stmt_;
};

class ForStmt : public Stmt {
public:
    ForStmt(struct Location loc, std::vector<std::unique_ptr<VarDecl>> decls, std::unique_ptr<Expr> init, std::unique_ptr<Expr> cond, std::unique_ptr<Expr> step, std::unique_ptr<Stmt> body);

private:
    std::vector<std::unique_ptr<VarDecl>> decls_;
    std::unique_ptr<Expr>                 init_;
    std::unique_ptr<Expr>                 cond_;
    std::unique_ptr<Expr>                 step_;
    std::unique_ptr<Stmt>                 body_;
};

class WhileStmt : public Stmt {
public:
    WhileStmt(struct Location loc, std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> body);

private:
    std::unique_ptr<Expr> cond_;
    std::unique_ptr<Stmt> body_;
};

class BreakStmt : public Stmt {
public:
    BreakStmt(struct Location loc);
};

class ContinueStmt : public Stmt {
public:
    ContinueStmt(struct Location loc);
};

class ExprStmt : public Stmt {
public:
    ExprStmt(struct Location loc, std::unique_ptr<Expr> expr);
    Expr *expr();

private:
    std::unique_ptr<Expr> expr_;
};

/* --------------------------------------------------
 * Declaration nodes
 * -------------------------------------------------- */

class Decl : public BlockEntry {
public:
    Decl(struct Location loc);
    virtual ~Decl() = default;
};

class VarDecl : public Decl {
public:
    VarDecl(struct Location loc, std::string id, Type type, std::unique_ptr<Expr> init);

private:
    std::string           id_;
    Type                  type_;
    std::unique_ptr<Expr> init_;
};

class ParamDecl : public Decl {
public:
    ParamDecl(struct Location loc, Type type, std::string id);

private:
    Type        type_;
    std::string id_;
};

class FunctionDecl : public Decl {
public:
    FunctionDecl(struct Location loc, Type type, std::string id);
    void add(std::unique_ptr<ParamDecl> param);
    void add(std::unique_ptr<CompoundStmt> body);

private:
    Type                                    type_;
    std::string                             id_;
    std::vector<std::unique_ptr<ParamDecl>> params_;
    std::unique_ptr<CompoundStmt>           body_;
};

class FieldDecl : public Decl {
public:
    FieldDecl(struct Location loc, Type type, std::string id);

private:
    Type        type_;
    std::string id_;
};

class StructDecl : public Decl {
public:
    StructDecl(struct Location loc, std::string id);
    void add(std::unique_ptr<FieldDecl> field);

private:
    std::string                             id_;
    std::vector<std::unique_ptr<FieldDecl>> fields_;
};

class UnionDecl : public Decl {
public:
    UnionDecl(struct Location loc, std::string id);
    void add(std::unique_ptr<FieldDecl> field);

private:
    std::string                             id_;
    std::vector<std::unique_ptr<FieldDecl>> fields_;
};

class EnumeratorDecl : public Decl {
public:
    EnumeratorDecl(struct Location loc, std::string id, std::unique_ptr<Expr> val);

private:
    std::string           id_;
    std::unique_ptr<Expr> val_;
};

class EnumDecl : public Decl {
public:
    EnumDecl(struct Location loc, std::string id);
    void add(std::unique_ptr<EnumeratorDecl> enumerator);

private:
    std::string                                  id_;
    std::vector<std::unique_ptr<EnumeratorDecl>> enumerators_;
};

/* --------------------------------------------------
 * Program node
 * -------------------------------------------------- */

class Program : public Node {
public:
    Program(struct Location loc);
    void add(std::unique_ptr<Decl> decl);

private:
    std::vector<std::unique_ptr<Decl>> decls_;
};