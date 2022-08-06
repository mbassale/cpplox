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
    case NodeType::VAR_DECLARATION: {
      auto varDeclStmt = std::static_pointer_cast<VarDeclaration>(stmt);
      return evalVarDeclarationStatement(varDeclStmt);
    }
    case NodeType::BLOCK_STATEMENT: {
      auto blockStmt = std::static_pointer_cast<Block>(stmt);
      return evalBlockStatement(blockStmt);
    }
    case NodeType::EMPTY_STATEMENT: {
      break;
    }
  }
  return NULL_OBJECT_PTR;
}

ObjectPtr Evaluator::evalVarDeclarationStatement(VarDeclarationPtr stmt) {
  ObjectPtr value = NULL_OBJECT_PTR;
  if (stmt->initializer) {
    value = evalExpression(stmt->initializer);
  }
  // TODO: we shouldn't access the lexeme here.
  const auto identifier = stmt->identifier.lexeme();
  globalEnv->set(identifier, value);
  return value;
}

ObjectPtr Evaluator::evalBlockStatement(ast::BlockPtr stmt) {
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  for (const auto& stmt : stmt->statements) {
    lastValue = evalStatement(stmt);
  }
  return lastValue;
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
    case NodeType::UNARY_EXPRESSION: {
      auto unaryExpr = std::static_pointer_cast<UnaryExpr>(expr);
      return evalUnaryExpression(unaryExpr);
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

ObjectPtr Evaluator::evalUnaryExpression(ast::UnaryExprPtr expr) {
  auto rhsValue = evalExpression(expr->right);

  switch (expr->operator_.type) {
    case TokenType::TOKEN_MINUS: {
      return evalMinusOperator(rhsValue);
    }
    case TokenType::TOKEN_BANG: {
      return evalBangOperator(rhsValue);
    }
    default:
      // TODO: throw RuntimeError
      return NULL_OBJECT_PTR;
  }
}

ObjectPtr Evaluator::evalMinusOperator(ObjectPtr rhsValue) {
  if (rhsValue->Type != ObjectType::OBJ_INTEGER) {
    // TODO: throw RuntimeError
  }
  auto intObj = std::static_pointer_cast<IntegerObject>(rhsValue);
  return IntegerObject::make(-intObj->Value);
}

ObjectPtr Evaluator::evalBangOperator(ObjectPtr rhsValue) {
  return BooleanObject::make(!rhsValue->isTruthy());
}

}  // namespace cpplox