#pragma once

#include "ast.h"
#include "class_object.h"
#include "common.h"
#include "environment.h"
#include "function.h"
#include "object.h"
#include "record.h"
#include "settings.h"

class RuntimeError : public std::runtime_error {
 public:
  RuntimeError(const std::string& message) : std::runtime_error(message) {}

  static RuntimeError make(const char* file_name, int line,
                           const std::string& msg);
};

class Evaluator {
 private:
  EnvironmentPtr globalCtx;

 public:
  Evaluator();

  ObjectPtr eval(ProgramPtr program);
  ObjectPtr getGlobalValue(const std::string& identifier) const {
    return globalCtx->get(identifier);
  }

 private:
  ObjectPtr evalStatement(EnvironmentPtr ctx, StatementPtr stmt);
  ObjectPtr evalVarDeclarationStatement(EnvironmentPtr ctx,
                                        VarDeclarationPtr stmt);
  ObjectPtr evalFuncDeclarationStatement(EnvironmentPtr ctx,
                                         FunctionDeclarationPtr stmt,
                                         FunctionType functionType);
  ObjectPtr evalClassDeclarationStatement(EnvironmentPtr ctx,
                                          ClassDeclarationPtr stmt);
  ObjectPtr evalIfStatement(EnvironmentPtr ctx, IfStatementPtr stmt);
  ObjectPtr evalForStatement(EnvironmentPtr ctx, ForStatementPtr stmt);
  ObjectPtr evalWhileStatement(EnvironmentPtr ctx, WhileStatementPtr stmt);
  ObjectPtr evalPrintStatement(EnvironmentPtr ctx, PrintStatementPtr stmt);
  ObjectPtr evalReturnStatement(EnvironmentPtr ctx, ReturnStatementPtr stmt);
  ObjectPtr evalBreakStatement(EnvironmentPtr ctx, BreakStatementPtr stmt);
  ObjectPtr evalContinueStatement(EnvironmentPtr ctx,
                                  ContinueStatementPtr stmt);
  ObjectPtr evalBlockStatement(EnvironmentPtr ctx, BlockPtr stmt);

  ObjectPtr evalExpression(EnvironmentPtr ctx, ExpressionPtr expr);
  ObjectPtr evalBinaryExpression(EnvironmentPtr ctx, BinaryExprPtr expr);
  ObjectPtr evalUnaryExpression(EnvironmentPtr ctx, UnaryExprPtr expr);
  ObjectPtr evalAssignExpression(EnvironmentPtr ctx, AssignmentPtr expr);
  ObjectPtr evalCallExpression(EnvironmentPtr ctx, CallExprPtr expr);
  ObjectPtr evalFunctionCall(EnvironmentPtr ctx, FunctionPtr callee,
                             CallExprPtr expr);
  ObjectPtr evalClassCall(EnvironmentPtr ctx, ClassObjectPtr callee,
                          CallExprPtr expr);
  ObjectPtr evalMemberExpr(EnvironmentPtr ctx, MemberExprPtr expr);
  IntegerObjectPtr evalIntegerLiteral(EnvironmentPtr ctx,
                                      IntegerLiteralPtr expr);
  BooleanObjectPtr evalBooleanLiteral(EnvironmentPtr ctx,
                                      BooleanLiteralPtr expr);
  NullObjectPtr evalNilLiteral(EnvironmentPtr ctx, NilLiteralPtr expr);
  StringObjectPtr evalStringLiteral(EnvironmentPtr ctx, StringLiteralPtr expr);
  ArrayObjectPtr evalArrayLiteral(EnvironmentPtr ctx, ArrayLiteralPtr expr);
  ObjectPtr evalArraySubscriptExpression(EnvironmentPtr ctx,
                                         ArraySubscriptExprPtr expr);
  ObjectPtr evalBinaryOperator(EnvironmentPtr ctx, ObjectPtr lhsValue,
                               TokenType operator_, ObjectPtr rhsValue);
  ObjectPtr evalLogicOperator(EnvironmentPtr ctx, ObjectPtr lhsValue,
                              TokenType operator_, ObjectPtr rhsValue);
  ObjectPtr evalComparisonOperator(EnvironmentPtr ctx, ObjectPtr lhsValue,
                                   TokenType operator_, ObjectPtr rhsValue);
  ObjectPtr evalMinusOperator(EnvironmentPtr ctx, ObjectPtr rhsValue);
  ObjectPtr evalBangOperator(EnvironmentPtr ctx, ObjectPtr rhsValue);
};