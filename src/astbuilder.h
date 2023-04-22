#pragma once

#include "ast.h"
#include "common.h"
#include "python_parser.hpp"

class ASTBuilderImpl : public ASTBuilder {
 public:
  ASTBuilderImpl() = default;
  ~ASTBuilderImpl() = default;

  cpplox::ast::ProgramPtr emitProgram(
      const std::vector<cpplox::ast::StatementPtr> &statements) override;
  cpplox::ast::ExpressionStatementPtr emitExpressionStatement(
      cpplox::ast::ExpressionPtr expr) override;
  cpplox::ast::IntegerLiteralPtr emitIntegerLiteral(
      const Token &value) override;
  cpplox::ast::StringLiteralPtr emitStringLiteral(const Token &value) override;
  cpplox::ast::VariableExprPtr emitIdentifier(const Token &value) override;
  cpplox::ast::BinaryExprPtr emitBinaryOp(
      TokenType op, cpplox::ast::ExpressionPtr lhs,
      cpplox::ast::ExpressionPtr rhs) override;
  cpplox::ast::IfStatementPtr emitIfStatement(
      cpplox::ast::ExpressionPtr condition,
      cpplox::ast::BlockPtr body) override;
  cpplox::ast::WhileStatementPtr emitWhileStatement(
      cpplox::ast::ExpressionPtr condition,
      cpplox::ast::BlockPtr body) override;
  cpplox::ast::FunctionDeclarationPtr emitDefStatement(
      const Token &name, cpplox::ast::BlockPtr body) override;
  cpplox::ast::BlockPtr emitBlock(
      const std::vector<cpplox::ast::StatementPtr> &statements) override;

  cpplox::ast::ProgramPtr getProgram() const { return program; }

 private:
  cpplox::ast::ProgramPtr program;
};