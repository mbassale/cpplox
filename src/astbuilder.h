#pragma once

#include "ast.h"
#include "common.h"
#include "parser.h"

class ASTBuilderImpl : public ASTBuilder {
 public:
  ASTBuilderImpl() = default;
  ~ASTBuilderImpl() = default;

  ProgramPtr emitProgram(const std::vector<StatementPtr> &statements) override;
  VarDeclarationPtr emitVarDeclaration(VariableExprPtr identifier,
                                       ExpressionPtr initializer) override;
  ClassDeclarationPtr emitClassDeclaration(
      const Token &name, const std::vector<StatementPtr> &definitions) override;
  ExpressionStatementPtr emitExpressionStatement(ExpressionPtr expr) override;
  IntegerLiteralPtr emitIntegerLiteral(const Token &value) override;
  StringLiteralPtr emitStringLiteral(const Token &value) override;
  BooleanLiteralPtr emitBooleanLiteral(bool value) override;
  NilLiteralPtr emitNilLiteral() override;
  ArrayLiteralPtr emitArrayLiteral(
      const std::vector<ExpressionPtr> &elements) override;
  ArraySubscriptExprPtr emitArraySubscript(ExpressionPtr array,
                                           ExpressionPtr index) override;
  VariableExprPtr emitVarExpression(const Token &value) override;
  MemberExprPtr emitMemberExpression(VariableExprPtr object,
                                     const Token &member) override;
  AssignmentPtr emitAssignmentExpression(ExpressionPtr lhs,
                                         ExpressionPtr rhs) override;
  CallExprPtr emitCallExpression(
      ExpressionPtr callee,
      const std::vector<ExpressionPtr> &arguments) override;
  UnaryExprPtr emitUnaryOp(TokenType op, ExpressionPtr rhs) override;
  BinaryExprPtr emitBinaryOp(TokenType op, ExpressionPtr lhs,
                             ExpressionPtr rhs) override;
  StatementPtr emitEmptyStatement() override;
  IfStatementPtr emitIfStatement(ExpressionPtr condition, BlockPtr thenBody,
                                 BlockPtr elseBody) override;
  WhileStatementPtr emitWhileStatement(ExpressionPtr condition,
                                       BlockPtr body) override;
  ForStatementPtr emitForStatement(StatementPtr initialization,
                                   ExpressionStatementPtr condition,
                                   ExpressionStatementPtr increment,
                                   BlockPtr body) override;
  FunctionDeclarationPtr emitDefStatement(
      VariableExprPtr name, const std::vector<VariableExprPtr> &arguments,
      BlockPtr body) override;
  PrintStatementPtr emitPrintStatement(ExpressionPtr expr) override;
  ReturnStatementPtr emitReturnStatement(ExpressionPtr expr) override;
  BreakStatementPtr emitBreakStatement() override;
  ContinueStatementPtr emitContinueStatement() override;
  BlockPtr emitBlock(const std::vector<StatementPtr> &statements) override;

  ProgramPtr getProgram() const { return program; }

 private:
  ProgramPtr program;
};