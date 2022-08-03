#include "evaluator.h"

namespace cpplox {
using namespace ast;

Evaluator::Evaluator() { globalEnv = std::make_shared<Environment>(); }

ObjectPtr Evaluator::eval(ProgramPtr program) {
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  for (const auto& stmt : program->statements) {
    lastValue = evalStatement(stmt);
  }
  return lastValue;
}

ObjectPtr Evaluator::evalStatement(StatementPtr stmt) {
  switch (stmt->Type) {
    case NodeType::EXPRESSION_STATEMENT: {
      auto exprStmt = std::static_pointer_cast<ExpressionStatement>(stmt);
      return evalExpression(exprStmt->expression);
    }
  }
  return NULL_OBJECT_PTR;
}

ObjectPtr Evaluator::evalExpression(ExpressionPtr expr) {
  switch (expr->Type) {
    case NodeType::INTEGER_LITERAL: {
      auto intExpr = std::static_pointer_cast<IntegerLiteral>(expr);
      return evalIntegerLiteral(intExpr);
    }
    case NodeType::BOOLEAN_LITERAL: {
      auto boolExpr = std::static_pointer_cast<BooleanLiteral>(expr);
      return evalBooleanLiteral(boolExpr);
    }
    case NodeType::STRING_LITERAL: {
      auto stringExpr = std::static_pointer_cast<StringLiteral>(expr);
      return evalStringLiteral(stringExpr);
    }
    case NodeType::NIL_LITERAL: {
      auto nilExpr = std::static_pointer_cast<NilLiteral>(expr);
      return evalNilLiteral(nilExpr);
    }
    default:
      break;
  }
  return NULL_OBJECT_PTR;
}

IntegerObjectPtr Evaluator::evalIntegerLiteral(ast::IntegerLiteralPtr expr) {
  return std::make_shared<IntegerObject>(expr->Value);
}

BooleanObjectPtr Evaluator::evalBooleanLiteral(ast::BooleanLiteralPtr expr) {
  if (expr->Value) {
    return TRUE_OBJECT_PTR;
  } else {
    return FALSE_OBJECT_PTR;
  }
}

NullObjectPtr Evaluator::evalNilLiteral(ast::NilLiteralPtr expr) {
  return NULL_OBJECT_PTR;
}

StringObjectPtr Evaluator::evalStringLiteral(ast::StringLiteralPtr expr) {
  return std::make_shared<StringObject>(expr->Value);
}

}  // namespace cpplox