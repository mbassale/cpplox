#ifndef __cpplox_evaluator_h
#define __cpplox_evaluator_h

#include "ast.h"
#include "common.h"
#include "environment.h"
#include "object.h"

namespace cpplox {

struct EvalContext;
using EvalContextPtr = std::shared_ptr<EvalContext>;

struct EvalContext {
  EnvironmentPtr env;

  EvalContext() { env = std::make_shared<Environment>(); }
  EvalContext(EvalContextPtr enclosing) {
    env = std::make_shared<Environment>(enclosing->env);
  }

  inline static EvalContextPtr make() {
    return std::make_shared<EvalContext>();
  }
  inline static EvalContextPtr make(EvalContextPtr enclosing) {
    return std::make_shared<EvalContext>(enclosing);
  }
};

class Evaluator {
 private:
  EvalContextPtr globalCtx;

 public:
  Evaluator();

  ObjectPtr eval(ast::ProgramPtr program);
  ObjectPtr getGlobalValue(const std::string& identifier) const {
    return globalCtx->env->get(identifier);
  }

 private:
  ObjectPtr evalStatement(EvalContextPtr ctx, ast::StatementPtr stmt);
  ObjectPtr evalVarDeclarationStatement(EvalContextPtr ctx,
                                        ast::VarDeclarationPtr stmt);
  ObjectPtr evalIfStatement(EvalContextPtr ctx, ast::IfStatementPtr stmt);
  ObjectPtr evalForStatement(EvalContextPtr ctx, ast::ForStatementPtr stmt);
  ObjectPtr evalBlockStatement(EvalContextPtr ctx, ast::BlockPtr stmt);

  ObjectPtr evalExpression(EvalContextPtr ctx, ast::ExpressionPtr expr);
  ObjectPtr evalBinaryExpression(EvalContextPtr ctx, ast::BinaryExprPtr expr);
  ObjectPtr evalUnaryExpression(EvalContextPtr ctx, ast::UnaryExprPtr expr);
  IntegerObjectPtr evalIntegerLiteral(EvalContextPtr ctx,
                                      ast::IntegerLiteralPtr expr);
  BooleanObjectPtr evalBooleanLiteral(EvalContextPtr ctx,
                                      ast::BooleanLiteralPtr expr);
  NullObjectPtr evalNilLiteral(EvalContextPtr ctx, ast::NilLiteralPtr expr);
  StringObjectPtr evalStringLiteral(EvalContextPtr ctx,
                                    ast::StringLiteralPtr expr);
  ObjectPtr evalBinaryOperator(EvalContextPtr ctx, ObjectPtr lhsValue,
                               TokenType operator_, ObjectPtr rhsValue);
  ObjectPtr evalMinusOperator(EvalContextPtr ctx, ObjectPtr rhsValue);
  ObjectPtr evalBangOperator(EvalContextPtr ctx, ObjectPtr rhsValue);
};

}  // namespace cpplox

#endif  // __cpplox_evaluator_h