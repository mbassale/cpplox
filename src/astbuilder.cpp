#include "astbuilder.h"

using namespace cpplox::ast;

ProgramPtr ASTBuilderImpl::emitProgram(
    const std::vector<StatementPtr> &statements) {
  program = Program::make(statements);
  return program;
}

VarDeclarationPtr ASTBuilderImpl::emitVarDeclaration(
    VariableExprPtr identifier, ExpressionPtr initializer) {
  Token nameToken(TokenType::TOKEN_IDENTIFIER, identifier->identifier);
  if (!initializer) {
    initializer = NilLiteral::make();
  }
  return VarDeclaration::make(nameToken, initializer);
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

BooleanLiteralPtr ASTBuilderImpl::emitBooleanLiteral(bool value) {
  return BooleanLiteral::make(value);
}

NilLiteralPtr ASTBuilderImpl::emitNilLiteral() { return NilLiteral::make(); }

VariableExprPtr ASTBuilderImpl::emitVarExpression(const Token &value) {
  return VariableExpr::make(value.lexeme());
}

AssignmentPtr ASTBuilderImpl::emitAssignmentExpression(
    VariableExprPtr identifier, ExpressionPtr value) {
  return Assignment::make(identifier->identifier, value);
}

BinaryExprPtr ASTBuilderImpl::emitBinaryOp(TokenType op,
                                           cpplox::ast::ExpressionPtr lhs,
                                           cpplox::ast::ExpressionPtr rhs) {
  return BinaryExpr::make(lhs, Token::make(op), rhs);
}

StatementPtr ASTBuilderImpl::emitEmptyStatement() { return Statement::make(); }

IfStatementPtr ASTBuilderImpl::emitIfStatement(
    cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr thenBody,
    cpplox::ast::BlockPtr elseBody) {
  if (elseBody == nullptr) {
    return IfStatement::make(condition, thenBody);
  }
  return IfStatement::make(condition, thenBody, elseBody);
}

WhileStatementPtr ASTBuilderImpl::emitWhileStatement(
    cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr body) {
  return WhileStatement::make(condition, body);
}

ForStatementPtr ASTBuilderImpl::emitForStatement(
    cpplox::ast::StatementPtr initialization,
    cpplox::ast::ExpressionStatementPtr condition,
    cpplox::ast::ExpressionStatementPtr increment, cpplox::ast::BlockPtr body) {
  ExpressionPtr conditionExpr;
  if (condition != nullptr) {
    conditionExpr = condition->expression;
  }
  if (conditionExpr == nullptr) {
    conditionExpr = BooleanLiteral::makeTrue();
  }
  ExpressionPtr incrementExpr;
  if (increment != nullptr) {
    incrementExpr = increment->expression;
  }
  if (incrementExpr == nullptr) {
    incrementExpr = NilLiteral::make();
  }
  return ForStatement::make(initialization, conditionExpr, incrementExpr, body);
}

FunctionDeclarationPtr ASTBuilderImpl::emitDefStatement(
    cpplox::ast::VariableExprPtr name,
    const std::vector<cpplox::ast::VariableExprPtr> &arguments,
    cpplox::ast::BlockPtr body) {
  Token nameToken(TokenType::TOKEN_IDENTIFIER, name->identifier);
  std::vector<Token> argumentTokens;
  for (const auto &arg : arguments) {
    argumentTokens.emplace_back(TokenType::TOKEN_IDENTIFIER, arg->identifier);
  }
  return FunctionDeclaration::make(nameToken, argumentTokens, body);
}

PrintStatementPtr ASTBuilderImpl::emitPrintStatement(ExpressionPtr expr) {
  return PrintStatement::make(expr);
}

ReturnStatementPtr ASTBuilderImpl::emitReturnStatement(ExpressionPtr expr) {
  return ReturnStatement::make(expr);
}

BreakStatementPtr ASTBuilderImpl::emitBreakStatement() {
  return BreakStatement::make();
}

BlockPtr ASTBuilderImpl::emitBlock(
    const std::vector<cpplox::ast::StatementPtr> &statements) {
  return Block::make(statements);
}