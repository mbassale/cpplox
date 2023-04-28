#pragma once

#include "ast.h"
#include "common.h"
#include "parser.h"

class ASTBuilderImpl : public ASTBuilder {
 public:
  ASTBuilderImpl() = default;
  ~ASTBuilderImpl() = default;

  cpplox::ast::ProgramPtr emitProgram(
      const std::vector<cpplox::ast::StatementPtr> &statements) override;
  cpplox::ast::VarDeclarationPtr emitVarDeclaration(
      cpplox::ast::VariableExprPtr identifier,
      cpplox::ast::ExpressionPtr initializer) override;
  cpplox::ast::ExpressionStatementPtr emitExpressionStatement(
      cpplox::ast::ExpressionPtr expr) override;
  cpplox::ast::IntegerLiteralPtr emitIntegerLiteral(
      const Token &value) override;
  cpplox::ast::StringLiteralPtr emitStringLiteral(const Token &value) override;
  cpplox::ast::BooleanLiteralPtr emitBooleanLiteral(bool value) override;
  cpplox::ast::NilLiteralPtr emitNilLiteral() override;
  cpplox::ast::ArrayLiteralPtr emitArrayLiteral(
      const std::vector<cpplox::ast::ExpressionPtr> &elements) override;
  cpplox::ast::ArraySubscriptExprPtr emitArraySubscript(
      cpplox::ast::ExpressionPtr array,
      cpplox::ast::ExpressionPtr index) override;
  cpplox::ast::VariableExprPtr emitVarExpression(const Token &value) override;
  cpplox::ast::AssignmentPtr emitAssignmentExpression(
      cpplox::ast::VariableExprPtr identifier,
      cpplox::ast::ExpressionPtr value) override;
  cpplox::ast::CallExprPtr emitCallExpression(
      cpplox::ast::ExpressionPtr callee,
      const std::vector<cpplox::ast::ExpressionPtr> &arguments) override;
  cpplox::ast::UnaryExprPtr emitUnaryOp(
      TokenType op, cpplox::ast::ExpressionPtr rhs) override;
  cpplox::ast::BinaryExprPtr emitBinaryOp(
      TokenType op, cpplox::ast::ExpressionPtr lhs,
      cpplox::ast::ExpressionPtr rhs) override;
  cpplox::ast::StatementPtr emitEmptyStatement() override;
  cpplox::ast::IfStatementPtr emitIfStatement(
      cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr thenBody,
      cpplox::ast::BlockPtr elseBody) override;
  cpplox::ast::WhileStatementPtr emitWhileStatement(
      cpplox::ast::ExpressionPtr condition,
      cpplox::ast::BlockPtr body) override;
  cpplox::ast::ForStatementPtr emitForStatement(
      cpplox::ast::StatementPtr initialization,
      cpplox::ast::ExpressionStatementPtr condition,
      cpplox::ast::ExpressionStatementPtr increment,
      cpplox::ast::BlockPtr body) override;
  cpplox::ast::FunctionDeclarationPtr emitDefStatement(
      cpplox::ast::VariableExprPtr name,
      const std::vector<cpplox::ast::VariableExprPtr> &arguments,
      cpplox::ast::BlockPtr body) override;
  cpplox::ast::PrintStatementPtr emitPrintStatement(
      cpplox::ast::ExpressionPtr expr) override;
  cpplox::ast::ReturnStatementPtr emitReturnStatement(
      cpplox::ast::ExpressionPtr expr) override;
  cpplox::ast::BreakStatementPtr emitBreakStatement() override;
  cpplox::ast::ContinueStatementPtr emitContinueStatement() override;
  cpplox::ast::BlockPtr emitBlock(
      const std::vector<cpplox::ast::StatementPtr> &statements) override;

  cpplox::ast::ProgramPtr getProgram() const { return program; }

 private:
  cpplox::ast::ProgramPtr program;
};