#include "../include/ast.hpp"

/* --------------------------------------------------
 * Node
 * -------------------------------------------------- */

Node::Node(struct Location loc) : loc_(loc) {

}

struct Location Node::loc() {
    return loc_;
}

/* --------------------------------------------------
 * Block
 * -------------------------------------------------- */

BlockEntry::BlockEntry(struct Location loc) : Node(loc) {

}

/* --------------------------------------------------
 * Expression nodes
 * -------------------------------------------------- */

Expr::Expr(struct Location loc) : Node(loc) {

}

CharExpr::CharExpr(struct Location loc, std::string val) : Expr(loc), val_(val) {

}

std::string& CharExpr::val() {
    return val_;
}

StringExpr::StringExpr(struct Location loc, std::string val) : Expr(loc), val_(val) {

}

std::string& StringExpr::val() {
    return val_;
}

IntExpr::IntExpr(struct Location loc, long val) : Expr(loc), val_(val) {

}

long IntExpr::val() {
    return val_;
}

IdExpr::IdExpr(struct Location loc, std::string id) : Expr(loc), id_(std::move(id)) {

}

std::string& IdExpr::id() {
    return id_;
}

SizeofExpr::SizeofExpr(struct Location loc, std::unique_ptr<Expr> operand) : Expr(loc), operand_(std::move(operand)) {

}

SizeofTypeExpr::SizeofTypeExpr(struct Location loc, Type type) : Expr(loc), type_(type) {
    
}

CastExpr::CastExpr(struct Location loc, Type type, std::unique_ptr<Expr> operand) : Expr(loc), type_(type), operand_(std::move(operand)) {

}

BinExpr::BinExpr(struct Location loc, BinOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) : Expr(loc), op_(op), left_(std::move(left)), right_(std::move(right)) {

}

BinOp BinExpr::op() {
    return op_;
}

Expr* BinExpr::left() {
    return left_.get();
}

Expr* BinExpr::right() {
    return right_.get();
}

UnExpr::UnExpr(struct Location loc, UnOp op, std::unique_ptr<Expr> operand) : Expr(loc), op_(op), operand_(std::move(operand)) {

}

AssignExpr::AssignExpr(struct Location loc, AssignOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) : Expr(loc), op_(op), left_(std::move(left)), right_(std::move(right)) {

}

CallExpr::CallExpr(struct Location loc, std::unique_ptr<Expr> callee) : Expr(loc), callee_(std::move(callee)) {

}

void CallExpr::add(std::unique_ptr<Expr> arg) {
    args_.push_back(std::move(arg));
}

IndexExpr::IndexExpr(struct Location loc, std::unique_ptr<Expr> id, std::unique_ptr<Expr> index) : Expr(loc), id_(std::move(id)), index_(std::move(index)) {

}

MemberExpr::MemberExpr(struct Location loc, std::unique_ptr<Expr> id, std::string member, bool ptr) : Expr(loc), id_(std::move(id)), member_(std::move(member)), ptr_(ptr) {

}

/* --------------------------------------------------
 * Statement nodes
 * -------------------------------------------------- */

Stmt::Stmt(struct Location loc) : BlockEntry(loc) {

}

RetStmt::RetStmt(struct Location loc, std::unique_ptr<Expr> val) : Stmt(loc), val_(std::move(val)) {

}

CompoundStmt::CompoundStmt(struct Location loc) : Stmt(loc) {

}

void CompoundStmt::add(std::unique_ptr<BlockEntry> entry) {
    entries_.push_back(std::move(entry));
}

IfStmt::IfStmt(struct Location loc, std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch) : Stmt(loc), cond_(std::move(cond)), then_branch_(std::move(then_branch)), else_branch_(std::move(else_branch)) {

}

SwitchStmt::SwitchStmt(struct Location loc, std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body) : Stmt(loc), expr_(std::move(expr)), body_(std::move(body)) {

}

CaseStmt::CaseStmt(struct Location loc, std::unique_ptr<Expr> val, std::unique_ptr<Stmt> stmt) : Stmt(loc), val_(std::move(val)), stmt_(std::move(stmt)) {

}

DefaultStmt::DefaultStmt(struct Location loc, std::unique_ptr<Stmt> stmt) : Stmt(loc), stmt_(std::move(stmt)) {

}

ForStmt::ForStmt(struct Location loc, std::vector<std::unique_ptr<VarDecl>> decls, std::unique_ptr<Expr> init, std::unique_ptr<Expr> cond, std::unique_ptr<Expr> step, std::unique_ptr<Stmt> body) : Stmt(loc), decls_(std::move(decls)), init_(std::move(init)), cond_(std::move(cond)), step_(std::move(step)), body_(std::move(body)) {

}

WhileStmt::WhileStmt(struct Location loc, std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> body) : Stmt(loc), cond_(std::move(cond)), body_(std::move(body)) {

}

BreakStmt::BreakStmt(struct Location loc) : Stmt(loc) {

}

ContinueStmt::ContinueStmt(struct Location loc) : Stmt(loc) {

}

ExprStmt::ExprStmt(struct Location loc, std::unique_ptr<Expr> expr) : Stmt(loc), expr_(std::move(expr)) {

}

Expr *ExprStmt::expr() {
    return expr_.get();
}

/* --------------------------------------------------
 * Declaration nodes
 * -------------------------------------------------- */

Decl::Decl(struct Location loc) : BlockEntry(loc) {

}

VarDecl::VarDecl(struct Location loc, std::string id, Type type, std::unique_ptr<Expr> init) : Decl(loc), id_(std::move(id)), type_(std::move(type)), init_(std::move(init)) {

}

ParamDecl::ParamDecl(struct Location loc, Type type, std::string id) : Decl(loc), type_(std::move(type)), id_(std::move(id)) {

}

FunctionDecl::FunctionDecl(struct Location loc, Type type, std::string id) : Decl(loc), type_(std::move(type)), id_(std::move(id)) {

}

void FunctionDecl::add(std::unique_ptr<ParamDecl> param) {
    params_.push_back(std::move(param));
}

void FunctionDecl::add(std::unique_ptr<CompoundStmt> body) {
    this->body_ = std::move(body);
}

FieldDecl::FieldDecl(struct Location loc, Type type, std::string id) : Decl(loc), type_(std::move(type)), id_(std::move(id)) {

}

StructDecl::StructDecl(struct Location loc, std::string id) : Decl(loc), id_(std::move(id)) {

}

void StructDecl::add(std::unique_ptr<FieldDecl> field) {
    fields_.push_back(std::move(field));
}

UnionDecl::UnionDecl(struct Location loc, std::string id) : Decl(loc), id_(std::move(id)) {

}

void UnionDecl::add(std::unique_ptr<FieldDecl> field) {
    fields_.push_back(std::move(field));
}

EnumeratorDecl::EnumeratorDecl(struct Location loc, std::string id, std::unique_ptr<Expr> val) : Decl(loc), id_(std::move(id)), val_(std::move(val)) {

}

EnumDecl::EnumDecl(struct Location loc, std::string id) : Decl(loc), id_(std::move(id)) {

}

void EnumDecl::add(std::unique_ptr<EnumeratorDecl> enumerator) {
    enumerators_.push_back(std::move(enumerator));
}

/* --------------------------------------------------
 * Program node
 * -------------------------------------------------- */

Program::Program(struct Location loc) : Node(loc) {

}

void Program::add(std::unique_ptr<Decl> decl) {
    decls_.push_back(std::move(decl));
}