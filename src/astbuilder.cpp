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

cpplox::ast::ClassDeclarationPtr ASTBuilderImpl::emitClassDeclaration(
    const Token &name,
    const std::vector<cpplox::ast::FunctionDeclarationPtr> &methods) {
  return ClassDeclaration::make(name.lexeme(), methods);
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

ArrayLiteralPtr ASTBuilderImpl::emitArrayLiteral(
    const std::vector<ExpressionPtr> &elements) {
  return ArrayLiteral::make(elements);
}

ArraySubscriptExprPtr ASTBuilderImpl::emitArraySubscript(ExpressionPtr array,
                                                         ExpressionPtr index) {
  return ArraySubscriptExpr::make(array, index);
}

VariableExprPtr ASTBuilderImpl::emitVarExpression(const Token &value) {
  return VariableExpr::make(value.lexeme());
}

MemberExprPtr ASTBuilderImpl::emitMemberExpression(
    cpplox::ast::VariableExprPtr object, const Token &member) {
  return MemberExpr::make(object, member.lexeme());
}

AssignmentPtr ASTBuilderImpl::emitAssignmentExpression(
    VariableExprPtr identifier, ExpressionPtr value) {
  return Assignment::make(identifier->identifier, value);
}

CallExprPtr ASTBuilderImpl::emitCallExpression(
    ExpressionPtr callee, const std::vector<ExpressionPtr> &arguments) {
  return CallExpr::make(callee, arguments);
}

UnaryExprPtr ASTBuilderImpl::emitUnaryOp(TokenType op, ExpressionPtr rhs) {
  return UnaryExpr::make(Token::make(op), rhs);
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

ContinueStatementPtr ASTBuilderImpl::emitContinueStatement() {
  return ContinueStatement::make();
}

BlockPtr ASTBuilderImpl::emitBlock(
    const std::vector<cpplox::ast::StatementPtr> &statements) {
  return Block::make(statements);
}