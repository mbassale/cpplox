#ifndef __cpplox_evaluator_h
#define __cpplox_evaluator_h

#include "ast.h"
#include "class_object.h"
#include "common.h"
#include "environment.h"
#include "function.h"
#include "record.h"
#include "object.h"
#include "settings.h"

namespace cpplox {

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

  ObjectPtr eval(ast::ProgramPtr program);
  ObjectPtr getGlobalValue(const std::string& identifier) const {
    return globalCtx->get(identifier);
  }

 private:
  ObjectPtr evalStatement(EnvironmentPtr ctx, ast::StatementPtr stmt);
  ObjectPtr evalVarDeclarationStatement(EnvironmentPtr ctx,
                                        ast::VarDeclarationPtr stmt);
  ObjectPtr evalFuncDeclarationStatement(EnvironmentPtr ctx,
                                         ast::FunctionDeclarationPtr stmt);
  ObjectPtr evalClassDeclarationStatement(EnvironmentPtr ctx,
                                          ast::ClassDeclarationPtr stmt);
  ObjectPtr evalIfStatement(EnvironmentPtr ctx, ast::IfStatementPtr stmt);
  ObjectPtr evalForStatement(EnvironmentPtr ctx, ast::ForStatementPtr stmt);
  ObjectPtr evalWhileStatement(EnvironmentPtr ctx, ast::WhileStatementPtr stmt);
  ObjectPtr evalPrintStatement(EnvironmentPtr ctx, ast::PrintStatementPtr stmt);
  ObjectPtr evalReturnStatement(EnvironmentPtr ctx,
                                ast::ReturnStatementPtr stmt);
  ObjectPtr evalBreakStatement(EnvironmentPtr ctx, ast::BreakStatementPtr stmt);
  ObjectPtr evalContinueStatement(EnvironmentPtr ctx,
                                  ast::ContinueStatementPtr stmt);
  ObjectPtr evalBlockStatement(EnvironmentPtr ctx, ast::BlockPtr stmt);

  ObjectPtr evalExpression(EnvironmentPtr ctx, ast::ExpressionPtr expr);
  ObjectPtr evalBinaryExpression(EnvironmentPtr ctx, ast::BinaryExprPtr expr);
  ObjectPtr evalUnaryExpression(EnvironmentPtr ctx, ast::UnaryExprPtr expr);
  ObjectPtr evalAssignExpression(EnvironmentPtr ctx, ast::AssignmentPtr expr);
  ObjectPtr evalCallExpression(EnvironmentPtr ctx, ast::CallExprPtr expr);
  ObjectPtr evalFunctionCall(EnvironmentPtr ctx, FunctionPtr callee,
                             ast::CallExprPtr expr);
  ObjectPtr evalClassCall(EnvironmentPtr ctx, ClassObjectPtr callee,
                          ast::CallExprPtr expr);
  ObjectPtr evalMemberExpr(EnvironmentPtr ctx, ast::MemberExprPtr expr);
  IntegerObjectPtr evalIntegerLiteral(EnvironmentPtr ctx,
                                      ast::IntegerLiteralPtr expr);
  BooleanObjectPtr evalBooleanLiteral(EnvironmentPtr ctx,
                                      ast::BooleanLiteralPtr expr);
  NullObjectPtr evalNilLiteral(EnvironmentPtr ctx, ast::NilLiteralPtr expr);
  StringObjectPtr evalStringLiteral(EnvironmentPtr ctx,
                                    ast::StringLiteralPtr expr);
  ArrayObjectPtr evalArrayLiteral(EnvironmentPtr ctx,
                                  ast::ArrayLiteralPtr expr);
  ObjectPtr evalArraySubscriptExpression(EnvironmentPtr ctx,
                                         ast::ArraySubscriptExprPtr expr);
  ObjectPtr evalBinaryOperator(EnvironmentPtr ctx, ObjectPtr lhsValue,
                               TokenType operator_, ObjectPtr rhsValue);
  ObjectPtr evalLogicOperator(EnvironmentPtr ctx, ObjectPtr lhsValue,
                              TokenType operator_, ObjectPtr rhsValue);
  ObjectPtr evalComparisonOperator(EnvironmentPtr ctx, ObjectPtr lhsValue,
                                   TokenType operator_, ObjectPtr rhsValue);
  ObjectPtr evalMinusOperator(EnvironmentPtr ctx, ObjectPtr rhsValue);
  ObjectPtr evalBangOperator(EnvironmentPtr ctx, ObjectPtr rhsValue);
};

}  // namespace cpplox

#endif  // __cpplox_evaluator_h