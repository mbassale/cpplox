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
  cpplox::ast::VariableExprPtr emitVarExpression(const Token &value) override;
  cpplox::ast::BinaryExprPtr emitBinaryOp(
      TokenType op, cpplox::ast::ExpressionPtr lhs,
      cpplox::ast::ExpressionPtr rhs) override;
  cpplox::ast::StatementPtr emitEmptyStatement() override;
  cpplox::ast::IfStatementPtr emitIfStatement(
      cpplox::ast::ExpressionPtr condition,
      cpplox::ast::BlockPtr body) override;
  cpplox::ast::WhileStatementPtr emitWhileStatement(
      cpplox::ast::ExpressionPtr condition,
      cpplox::ast::BlockPtr body) override;
  cpplox::ast::FunctionDeclarationPtr emitDefStatement(
      cpplox::ast::VariableExprPtr name,
      const std::vector<cpplox::ast::VariableExprPtr> &arguments,
      cpplox::ast::BlockPtr body) override;
  cpplox::ast::BlockPtr emitBlock(
      const std::vector<cpplox::ast::StatementPtr> &statements) override;

  cpplox::ast::ProgramPtr getProgram() const { return program; }

 private:
  cpplox::ast::ProgramPtr program;
};