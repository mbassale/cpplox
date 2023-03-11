#include "evaluator.h"

#include <cstdarg>
#include <filesystem>

namespace {
using namespace cpplox;

bool isReturnValue(ObjectPtr value) {
  return value->Type == ObjectType::OBJ_RETURN_VALUE;
}
}  // namespace

namespace cpplox {
using namespace ast;

static IntegerObjectPtr tryCastAsInteger(ObjectPtr obj);

RuntimeError RuntimeError::make(const char* file_name, int line,
                                const std::string& msg) {
  std::ostringstream ss;
  ss << "[" << std::filesystem::path(file_name).filename() << ":" << line
     << "] " << msg;
  return RuntimeError(ss.str());
}

Evaluator::Evaluator() { globalCtx = EvalContext::make(); }

ObjectPtr Evaluator::eval(ProgramPtr program) {
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  for (const auto& stmt : program->statements) {
    lastValue = evalStatement(globalCtx, stmt);
    if (isReturnValue(lastValue)) {
      return lastValue;
    }
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
    case NodeType::FUNCTION_DECLARATION: {
      auto funcDeclStmt = std::static_pointer_cast<FunctionDeclaration>(stmt);
      return evalFuncDeclarationStatement(ctx, funcDeclStmt);
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
    case NodeType::WHILE_STATEMENT: {
      auto whileStmt = std::static_pointer_cast<WhileStatement>(stmt);
      return evalWhileStatement(ctx, whileStmt);
    }
    case NodeType::PRINT_STATEMENT: {
      auto printStmt = std::static_pointer_cast<PrintStatement>(stmt);
      return evalPrintStatement(ctx, printStmt);
    }
    case NodeType::RETURN_STATEMENT: {
      auto returnStmt = std::static_pointer_cast<ReturnStatement>(stmt);
      return evalReturnStatement(ctx, returnStmt);
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

ObjectPtr Evaluator::evalFuncDeclarationStatement(EvalContextPtr ctx,
                                                  FunctionDeclarationPtr stmt) {
  const auto& functionName = stmt->identifier.lexeme();
  auto function = Function::make(FunctionType::TYPE_FUNCTION, stmt,
                                 functionName, stmt->params.size());
  ctx->env->set(functionName, function);
  return function;
}

ObjectPtr Evaluator::evalBlockStatement(EvalContextPtr ctx,
                                        ast::BlockPtr stmt) {
  auto localCtx = EvalContext::make(ctx);
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  for (const auto& stmt : stmt->statements) {
    lastValue = evalStatement(localCtx, stmt);
    if (isReturnValue(lastValue)) {
      return lastValue;
    }
  }
  return lastValue;
}

ObjectPtr Evaluator::evalIfStatement(EvalContextPtr ctx, IfStatementPtr stmt) {
  auto conditionValue = evalExpression(ctx, stmt->condition);
  if (conditionValue->isTruthy()) {
    return evalStatement(ctx, stmt->thenBranch);
  } else if (stmt->elseBranch != nullptr) {
    return evalStatement(ctx, stmt->elseBranch);
  } else {
    return FALSE_OBJECT_PTR;
  }
}

ObjectPtr Evaluator::evalForStatement(EvalContextPtr ctx,
                                      ForStatementPtr stmt) {
  auto localCtx = EvalContext::make(ctx);
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  lastValue = evalStatement(localCtx, stmt->initializer);
  while (true) {
    auto conditionValue = evalExpression(localCtx, stmt->condition);
    lastValue = conditionValue;
    if (conditionValue->isFalsey()) {
      break;
    }
    lastValue = evalStatement(localCtx, stmt->body);
    if (isReturnValue(lastValue)) {
      return lastValue;
    }
    lastValue = evalExpression(localCtx, stmt->increment);
  }
  return lastValue;
}

ObjectPtr Evaluator::evalWhileStatement(EvalContextPtr ctx,
                                        WhileStatementPtr stmt) {
  ObjectPtr lastValue = evalExpression(ctx, stmt->condition);
  while (lastValue->isTruthy()) {
    lastValue = evalStatement(ctx, stmt->body);
    if (isReturnValue(lastValue)) {
      return lastValue;
    }
    lastValue = evalExpression(ctx, stmt->condition);
  }
  return lastValue;
}

ObjectPtr Evaluator::evalPrintStatement(EvalContextPtr ctx,
                                        PrintStatementPtr stmt) {
  ObjectPtr lastValue = evalExpression(ctx, stmt->expression);
  std::cout << lastValue->toString() << std::endl;
  return lastValue;
}

ObjectPtr Evaluator::evalReturnStatement(EvalContextPtr ctx,
                                         ReturnStatementPtr stmt) {
  ObjectPtr lastValue = evalExpression(ctx, stmt->expression);
  return ReturnObject::make(lastValue);
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
    case NodeType::BINARY_EXPRESSION: {
      auto binaryExpr = std::static_pointer_cast<BinaryExpr>(expr);
      return evalBinaryExpression(ctx, binaryExpr);
    }
    case NodeType::VARIABLE_EXPRESSION: {
      auto varExpr = std::static_pointer_cast<VariableExpr>(expr);
      return ctx->env->get(varExpr->identifier);
    }
    case NodeType::ASSIGNMENT_EXPRESSION: {
      auto assignExpr = std::static_pointer_cast<Assignment>(expr);
      return evalAssignExpression(ctx, assignExpr);
    }
    case NodeType::CALL_EXPRESSION: {
      auto callExpr = std::static_pointer_cast<CallExpr>(expr);
      return evalCallExpression(ctx, callExpr);
    }
    default:
      break;
  }
  return NULL_OBJECT_PTR;
}

ObjectPtr Evaluator::evalAssignExpression(EvalContextPtr ctx,
                                          ast::AssignmentPtr expr) {
  const auto& identifier = expr->identifier;
  auto value = evalExpression(ctx, expr->value);
  ctx->env->set(identifier, value);
  return ctx->env->get(identifier);
}

ObjectPtr Evaluator::evalCallExpression(EvalContextPtr ctx,
                                        ast::CallExprPtr expr) {
  auto value = evalExpression(ctx, expr->left);
  if (isReturnValue(value)) {
    auto returnValue = std::dynamic_pointer_cast<ReturnObject>(value);
    value = returnValue->Value;
  }
  if (value->Type != ObjectType::OBJ_FUNCTION) {
    std::ostringstream ss;
    ss << "Invalid callable: " << value->toString();
    throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
  auto funcValue = std::dynamic_pointer_cast<Function>(value);
  assert(funcValue != nullptr);
  auto funcDeclStmt = funcValue->getDeclaration();
  auto funcCtx = EvalContext::make(ctx);
  // bind arguments
  for (size_t i = 0; i < funcDeclStmt->params.size(); i++) {
    const auto paramName = funcDeclStmt->params[i].lexeme();
    auto argExpr = expr->arguments[i];
    auto argValue = evalExpression(ctx, argExpr);
    funcCtx->env->set(paramName, argValue);
  }
  // execute function body
  auto lastValue = evalStatement(funcCtx, funcDeclStmt->body);
  if (isReturnValue(lastValue)) {
    auto returnValue = std::dynamic_pointer_cast<ReturnObject>(lastValue);
    assert(returnValue != nullptr);
    return returnValue->Value;
  }
  return lastValue;
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

ObjectPtr Evaluator::evalBinaryExpression(EvalContextPtr ctx,
                                          ast::BinaryExprPtr expr) {
  auto leftValue = evalExpression(ctx, expr->left);
  auto rightValue = evalExpression(ctx, expr->right);
  switch (expr->operator_.type) {
    case TokenType::TOKEN_PLUS:
    case TokenType::TOKEN_MINUS:
    case TokenType::TOKEN_STAR:
    case TokenType::TOKEN_SLASH:
      return evalBinaryOperator(ctx, leftValue, expr->operator_.type,
                                rightValue);
    case TokenType::TOKEN_AND:
    case TokenType::TOKEN_OR:
      return evalLogicOperator(ctx, leftValue, expr->operator_.type,
                               rightValue);
    case TokenType::TOKEN_EQUAL_EQUAL:
    case TokenType::TOKEN_BANG_EQUAL:
    case TokenType::TOKEN_LESS:
    case TokenType::TOKEN_LESS_EQUAL:
    case TokenType::TOKEN_GREATER:
    case TokenType::TOKEN_GREATER_EQUAL:
      return evalComparisonOperator(ctx, leftValue, expr->operator_.type,
                                    rightValue);
    default:
      // TODO: throw RuntimeError
      return NULL_OBJECT_PTR;
  }
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
      std::ostringstream ss;
      ss << "Invalid unary operator type: " << expr->operator_.lexeme();
      throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
}

ObjectPtr Evaluator::evalLogicOperator(EvalContextPtr ctx, ObjectPtr lhsValue,
                                       TokenType operator_,
                                       ObjectPtr rhsValue) {
  auto lhsBoolValue = lhsValue->isTruthy();
  auto rhsBoolValue = rhsValue->isTruthy();
  bool result = false;
  switch (operator_) {
    case TokenType::TOKEN_AND:
      result = lhsBoolValue && rhsBoolValue;
      break;
    case TokenType::TOKEN_OR:
      result = lhsBoolValue || rhsBoolValue;
      break;
    default:
      std::ostringstream ss;
      ss << "Invalid binary operands for logic operator: "
         << lhsValue->toString() << (int)operator_ << rhsValue->toString();
      throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
  return BooleanObject::make(result);
}

ObjectPtr Evaluator::evalComparisonOperator(EvalContextPtr ctx,
                                            ObjectPtr lhsValue,
                                            TokenType operator_,
                                            ObjectPtr rhsValue) {
  bool result = false;
  switch (operator_) {
    case TokenType::TOKEN_EQUAL_EQUAL:
      result = lhsValue->isEqual(*rhsValue);
      break;
    case TokenType::TOKEN_BANG_EQUAL:
      result = !lhsValue->isEqual(*rhsValue);
      break;
    case TokenType::TOKEN_LESS: {
      auto lhsIntValue = tryCastAsInteger(lhsValue);
      auto rhsIntValue = tryCastAsInteger(rhsValue);
      result = lhsIntValue->Value < rhsIntValue->Value;
      break;
    }
    case TokenType::TOKEN_LESS_EQUAL: {
      auto lhsIntValue = tryCastAsInteger(lhsValue);
      auto rhsIntValue = tryCastAsInteger(rhsValue);
      result = lhsIntValue->Value <= rhsIntValue->Value;
    }
    case TokenType::TOKEN_GREATER: {
      auto lhsIntValue = tryCastAsInteger(lhsValue);
      auto rhsIntValue = tryCastAsInteger(rhsValue);
      result = lhsIntValue->Value > rhsIntValue->Value;
    }
    case TokenType::TOKEN_GREATER_EQUAL: {
      auto lhsIntValue = tryCastAsInteger(lhsValue);
      auto rhsIntValue = tryCastAsInteger(rhsValue);
      result = lhsIntValue->Value >= rhsIntValue->Value;
      break;
    }
    default:
      std::ostringstream ss;
      ss << "Invalid operands types for comparison operator: "
         << lhsValue->toString() << " " << (int)operator_
         << rhsValue->toString();
      throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
  return BooleanObject::make(result);
}

ObjectPtr Evaluator::evalBinaryOperator(EvalContextPtr ctx, ObjectPtr lhsValue,
                                        TokenType operator_,
                                        ObjectPtr rhsValue) {
  if (lhsValue->isNumeric() && rhsValue->isNumeric()) {
    auto lhsIntValue = tryCastAsInteger(lhsValue);
    auto rhsIntValue = tryCastAsInteger(rhsValue);
    int64_t result;
    switch (operator_) {
      case TokenType::TOKEN_STAR:
        result = lhsIntValue->Value * rhsIntValue->Value;
        break;
      case TokenType::TOKEN_SLASH:
        result = lhsIntValue->Value / rhsIntValue->Value;
        break;
      case TokenType::TOKEN_PLUS:
        result = lhsIntValue->Value + rhsIntValue->Value;
        break;
      case TokenType::TOKEN_MINUS:
        result = lhsIntValue->Value - rhsIntValue->Value;
        break;
      default:
        std::ostringstream ss;
        ss << "Invalid binary operator type: " << (int)operator_;
        throw RuntimeError::make(__FILE__, __LINE__, ss.str());
    }
    return IntegerObject::make(result);
  }
  std::ostringstream ss;
  ss << "Invalid binary operands: " << lhsValue->toString() << " and "
     << rhsValue->toString();
  throw RuntimeError::make(__FILE__, __LINE__, ss.str());
}

ObjectPtr Evaluator::evalMinusOperator(EvalContextPtr ctx, ObjectPtr rhsValue) {
  auto intObj = tryCastAsInteger(rhsValue);
  return IntegerObject::make(-intObj->Value);
}

ObjectPtr Evaluator::evalBangOperator(EvalContextPtr ctx, ObjectPtr rhsValue) {
  return BooleanObject::make(!rhsValue->isTruthy());
}

IntegerObjectPtr tryCastAsInteger(ObjectPtr obj) {
  if (obj->Type == ObjectType::OBJ_INTEGER) {
    return std::static_pointer_cast<IntegerObject>(obj);
  }
  std::ostringstream ss;
  ss << "Cannot convert object to integer: " << obj->toString();
  throw RuntimeError::make(__FILE__, __LINE__, ss.str());
}

}  // namespace cpplox