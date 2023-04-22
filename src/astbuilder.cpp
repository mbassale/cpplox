#include "astbuilder.h"

using namespace cpplox::ast;

ProgramPtr ASTBuilderImpl::emitProgram(
    const std::vector<StatementPtr> &statements) {
  LOG(INFO) << "Program: " << statements.size();
  program = Program::make(statements);
  return program;
}

ExpressionStatementPtr ASTBuilderImpl::emitExpressionStatement(
    cpplox::ast::ExpressionPtr expr) {
  return ExpressionStatement::make(expr);
}

IntegerLiteralPtr ASTBuilderImpl::emitIntegerLiteral(const Token &value) {
  return IntegerLiteral::make(std::stoll(value.lexeme()));
}

StringLiteralPtr ASTBuilderImpl::emitStringLiteral(const Token &value) {
  return StringLiteral::make(value.lexeme());
}

VariableExprPtr ASTBuilderImpl::emitIdentifier(const Token &value) {
  return VariableExpr::make(value.lexeme());
}

BinaryExprPtr ASTBuilderImpl::emitBinaryOp(TokenType op,
                                           cpplox::ast::ExpressionPtr lhs,
                                           cpplox::ast::ExpressionPtr rhs) {
  return BinaryExpr::make(lhs, Token::make(op), rhs);
}

IfStatementPtr ASTBuilderImpl::emitIfStatement(
    cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr body) {
  return IfStatement::make(condition, body);
}

WhileStatementPtr ASTBuilderImpl::emitWhileStatement(
    cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr body) {
  return WhileStatement::make(condition, body);
}

FunctionDeclarationPtr ASTBuilderImpl::emitDefStatement(
    const Token &name, cpplox::ast::BlockPtr body) {
  return FunctionDeclaration::make(name, {}, body);
}

BlockPtr ASTBuilderImpl::emitBlock(
    const std::vector<cpplox::ast::StatementPtr> &statements) {
  return Block::make(statements);
}