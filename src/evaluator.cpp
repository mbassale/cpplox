#include "evaluator.h"

namespace cpplox {
using namespace ast;

Evaluator::Evaluator() { globalCtx = EvalContext::make(); }

ObjectPtr Evaluator::eval(ProgramPtr program) {
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  for (const auto& stmt : program->statements) {
    lastValue = evalStatement(globalCtx, stmt);
  }
  return lastValue;
}

ObjectPtr Evaluator::evalStatement(EvalContextPtr ctx, StatementPtr stmt) {
  switch (stmt->Type) {
    case NodeType::EXPRESSION_STATEMENT: {
      auto exprStmt = std::static_pointer_cast<ExpressionStatement>(stmt);
      return evalExpression(ctx, exprStmt->expression);
    }
    case NodeType::VAR_DECLARATION: {
      auto varDeclStmt = std::static_pointer_cast<VarDeclaration>(stmt);
      return evalVarDeclarationStatement(ctx, varDeclStmt);
    }
    case NodeType::BLOCK_STATEMENT: {
      auto blockStmt = std::static_pointer_cast<Block>(stmt);
      return evalBlockStatement(ctx, blockStmt);
    }
    case NodeType::IF_STATEMENT: {
      auto ifStmt = std::static_pointer_cast<IfStatement>(stmt);
      return evalIfStatement(ctx, ifStmt);
    }
    case NodeType::FOR_STATEMENT: {
      auto forStmt = std::static_pointer_cast<ForStatement>(stmt);
      return evalForStatement(ctx, forStmt);
    }
    case NodeType::EMPTY_STATEMENT: {
      break;
    }
  }
  return NULL_OBJECT_PTR;
}

ObjectPtr Evaluator::evalVarDeclarationStatement(EvalContextPtr ctx,
                                                 VarDeclarationPtr stmt) {
  ObjectPtr value = NULL_OBJECT_PTR;
  if (stmt->initializer) {
    value = evalExpression(ctx, stmt->initializer);
  }
  // TODO: we shouldn't access the lexeme here.
  const auto identifier = stmt->identifier.lexeme();
  ctx->env->set(identifier, value);
  return value;
}

ObjectPtr Evaluator::evalBlockStatement(EvalContextPtr ctx,
                                        ast::BlockPtr stmt) {
  auto localCtx = EvalContext::make(ctx);
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  for (const auto& stmt : stmt->statements) {
    lastValue = evalStatement(localCtx, stmt);
  }
  return lastValue;
}

ObjectPtr Evaluator::evalIfStatement(EvalContextPtr ctx, IfStatementPtr stmt) {
  auto conditionValue = evalExpression(ctx, stmt->condition);
  if (conditionValue->isTruthy()) {
    return evalStatement(ctx, stmt->thenBranch);
  } else {
    return evalStatement(ctx, stmt->elseBranch);
  }
}

ObjectPtr Evaluator::evalForStatement(EvalContextPtr ctx,
                                      ast::ForStatementPtr stmt) {
  auto localCtx = EvalContext::make(ctx);
  auto lastValue = evalStatement(localCtx, stmt->initializer);
  auto keepLooping = true;
  while (keepLooping) {
    auto conditionValue = evalExpression(localCtx, stmt->condition);
    lastValue = conditionValue;
    if (conditionValue->isFalsey()) {
      break;
    }
    lastValue = evalStatement(localCtx, stmt->body);
    lastValue = evalExpression(localCtx, stmt->increment);
    LOG(INFO) << lastValue->toString();
  }
  return lastValue;
}

ObjectPtr Evaluator::evalExpression(EvalContextPtr ctx, ExpressionPtr expr) {
  switch (expr->Type) {
    case NodeType::INTEGER_LITERAL: {
      auto intExpr = std::static_pointer_cast<IntegerLiteral>(expr);
      return evalIntegerLiteral(ctx, intExpr);
    }
    case NodeType::BOOLEAN_LITERAL: {
      auto boolExpr = std::static_pointer_cast<BooleanLiteral>(expr);
      return evalBooleanLiteral(ctx, boolExpr);
    }
    case NodeType::STRING_LITERAL: {
      auto stringExpr = std::static_pointer_cast<StringLiteral>(expr);
      return evalStringLiteral(ctx, stringExpr);
    }
    case NodeType::NIL_LITERAL: {
      auto nilExpr = std::static_pointer_cast<NilLiteral>(expr);
      return evalNilLiteral(ctx, nilExpr);
    }
    case NodeType::UNARY_EXPRESSION: {
      auto unaryExpr = std::static_pointer_cast<UnaryExpr>(expr);
      return evalUnaryExpression(ctx, unaryExpr);
    }
    default:
      break;
  }
  return NULL_OBJECT_PTR;
}

IntegerObjectPtr Evaluator::evalIntegerLiteral(EvalContextPtr ctx,
                                               ast::IntegerLiteralPtr expr) {
  return std::make_shared<IntegerObject>(expr->Value);
}

BooleanObjectPtr Evaluator::evalBooleanLiteral(EvalContextPtr ctx,
                                               ast::BooleanLiteralPtr expr) {
  if (expr->Value) {
    return TRUE_OBJECT_PTR;
  } else {
    return FALSE_OBJECT_PTR;
  }
}

NullObjectPtr Evaluator::evalNilLiteral(EvalContextPtr ctx,
                                        ast::NilLiteralPtr expr) {
  return NULL_OBJECT_PTR;
}

StringObjectPtr Evaluator::evalStringLiteral(EvalContextPtr ctx,
                                             ast::StringLiteralPtr expr) {
  return std::make_shared<StringObject>(expr->Value);
}

ObjectPtr Evaluator::evalUnaryExpression(EvalContextPtr ctx,
                                         ast::UnaryExprPtr expr) {
  auto rhsValue = evalExpression(ctx, expr->right);

  switch (expr->operator_.type) {
    case TokenType::TOKEN_MINUS: {
      return evalMinusOperator(ctx, rhsValue);
    }
    case TokenType::TOKEN_BANG: {
      return evalBangOperator(ctx, rhsValue);
    }
    default:
      // TODO: throw RuntimeError
      return NULL_OBJECT_PTR;
  }
}

ObjectPtr Evaluator::evalMinusOperator(EvalContextPtr ctx, ObjectPtr rhsValue) {
  if (rhsValue->Type != ObjectType::OBJ_INTEGER) {
    // TODO: throw RuntimeError
  }
  auto intObj = std::static_pointer_cast<IntegerObject>(rhsValue);
  return IntegerObject::make(-intObj->Value);
}

ObjectPtr Evaluator::evalBangOperator(EvalContextPtr ctx, ObjectPtr rhsValue) {
  return BooleanObject::make(!rhsValue->isTruthy());
}

}  // namespace cpplox