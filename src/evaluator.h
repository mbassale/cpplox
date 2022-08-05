#ifndef __cpplox_evaluator_h
#define __cpplox_evaluator_h

#include "ast.h"
#include "common.h"
#include "environment.h"
#include "object.h"

namespace cpplox {

class Evaluator {
 private:
  std::shared_ptr<Environment> globalEnv;

 public:
  Evaluator();

  ObjectPtr eval(ast::ProgramPtr program);

 private:
  ObjectPtr evalStatement(ast::StatementPtr stmt);
  ObjectPtr evalVarDeclarationStatement(ast::VarDeclarationPtr stmt);
  ObjectPtr evalExpression(ast::ExpressionPtr expr);
  IntegerObjectPtr evalIntegerLiteral(ast::IntegerLiteralPtr expr);
  BooleanObjectPtr evalBooleanLiteral(ast::BooleanLiteralPtr expr);
  NullObjectPtr evalNilLiteral(ast::NilLiteralPtr expr);
  StringObjectPtr evalStringLiteral(ast::StringLiteralPtr expr);
  ObjectPtr evalUnaryExpression(ast::UnaryExprPtr expr);
  ObjectPtr evalMinusOperator(ObjectPtr rhsValue);
  ObjectPtr evalBangOperator(ObjectPtr rhsValue);
};

}  // namespace cpplox

#endif  // __cpplox_evaluator_h