#include "evaluator.h"

#include <cstdarg>
#include <filesystem>

namespace {

static bool isReturnObject(ObjectPtr value) {
  return value->Type == ObjectType::OBJ_RETURN_VALUE;
}

static bool isBreakObject(ObjectPtr value) {
  return value->Type == ObjectType::OBJ_BREAK;
}

static bool isContinueObject(ObjectPtr value) {
  return value->Type == ObjectType::OBJ_CONTINUE;
}

static bool isCallable(ObjectPtr value) {
  return value->Type == ObjectType::OBJ_CLASS ||
         value->Type == ObjectType::OBJ_FUNCTION;
}

}  // namespace

static IntegerObjectPtr tryCastAsInteger(ObjectPtr obj);

RuntimeError RuntimeError::make(const char* file_name, int line,
                                const std::string& msg) {
  std::ostringstream ss;
  ss << "[" << std::filesystem::path(file_name).filename() << ":" << line
     << "] " << msg;
  return RuntimeError(ss.str());
}

Evaluator::Evaluator() { globalCtx = Environment::make(); }

ObjectPtr Evaluator::eval(ProgramPtr program) {
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  for (const auto& stmt : program->statements) {
    if (Settings::getInstance()->isDebugMode()) {
      LOG(INFO) << "Env: " << globalCtx->toString();
      LOG(INFO) << "Executing: " << stmt->toString();
    }
    lastValue = evalStatement(globalCtx, stmt);
    if (isReturnObject(lastValue)) {
      return lastValue;
    } else if (isBreakObject(lastValue) || isContinueObject(lastValue)) {
      std::ostringstream ss;
      ss << "Invalid statement: " << lastValue->toString();
      throw RuntimeError::make(__FILE__, __LINE__, ss.str());
    }
  }
  return lastValue;
}

ObjectPtr Evaluator::evalStatement(EnvironmentPtr ctx, StatementPtr stmt) {
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
      return evalFuncDeclarationStatement(ctx, funcDeclStmt,
                                          FunctionType::TYPE_FUNCTION);
    }
    case NodeType::CLASS_DECLARATION: {
      auto classDeclStmt = std::static_pointer_cast<ClassDeclaration>(stmt);
      return evalClassDeclarationStatement(ctx, classDeclStmt);
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
    case NodeType::BREAK_STATEMENT: {
      auto breakStmt = std::static_pointer_cast<BreakStatement>(stmt);
      return evalBreakStatement(ctx, breakStmt);
    }
    case NodeType::CONTINUE_STATEMENT: {
      auto continueStmt = std::static_pointer_cast<ContinueStatement>(stmt);
      return evalContinueStatement(ctx, continueStmt);
    }
    case NodeType::EMPTY_STATEMENT:
    default:
      return NULL_OBJECT_PTR;
  }
}

ObjectPtr Evaluator::evalVarDeclarationStatement(EnvironmentPtr ctx,
                                                 VarDeclarationPtr stmt) {
  ObjectPtr value = NULL_OBJECT_PTR;
  if (stmt->initializer) {
    value = evalExpression(ctx, stmt->initializer);
  }
  ctx->set(stmt->identifier, value);
  return value;
}

ObjectPtr Evaluator::evalFuncDeclarationStatement(EnvironmentPtr ctx,
                                                  FunctionDeclarationPtr stmt,
                                                  FunctionType functionType) {
  const auto& functionName = stmt->identifier;
  auto function = Function::make(ctx, functionType, stmt, functionName,
                                 stmt->params.size());
  ctx->set(functionName, function);
  return function;
}

ObjectPtr Evaluator::evalClassDeclarationStatement(EnvironmentPtr ctx,
                                                   ClassDeclarationPtr stmt) {
  const auto& className = stmt->identifier;
  auto classDeclaration = ClassObject::make(stmt);
  // classes live in the global ctx
  globalCtx->set(className, classDeclaration);
  return classDeclaration;
}

ObjectPtr Evaluator::evalBlockStatement(EnvironmentPtr ctx, BlockPtr stmt) {
  auto localCtx = Environment::make(ctx);
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  for (const auto& stmt : stmt->statements) {
    lastValue = evalStatement(localCtx, stmt);
    if (isReturnObject(lastValue) || isBreakObject(lastValue) ||
        isContinueObject(lastValue)) {
      return lastValue;
    }
  }
  return lastValue;
}

ObjectPtr Evaluator::evalIfStatement(EnvironmentPtr ctx, IfStatementPtr stmt) {
  auto conditionValue = evalExpression(ctx, stmt->condition);
  if (conditionValue->isTruthy()) {
    return evalStatement(ctx, stmt->thenBranch);
  } else if (stmt->elseBranch != nullptr) {
    return evalStatement(ctx, stmt->elseBranch);
  } else {
    return FALSE_OBJECT_PTR;
  }
}

ObjectPtr Evaluator::evalForStatement(EnvironmentPtr ctx,
                                      ForStatementPtr stmt) {
  auto localCtx = Environment::make(ctx);
  ObjectPtr lastValue = NULL_OBJECT_PTR;
  lastValue = evalStatement(localCtx, stmt->initializer);
  while (true) {
    auto conditionValue = evalExpression(localCtx, stmt->condition);
    lastValue = conditionValue;
    if (conditionValue->isFalsey()) {
      break;
    }
    lastValue = evalStatement(localCtx, stmt->body);
    if (isReturnObject(lastValue)) {
      return lastValue;
    } else if (isBreakObject(lastValue)) {
      return NULL_OBJECT_PTR;
    }
    lastValue = evalExpression(localCtx, stmt->increment);
  }
  return lastValue;
}

ObjectPtr Evaluator::evalWhileStatement(EnvironmentPtr ctx,
                                        WhileStatementPtr stmt) {
  ObjectPtr lastValue = evalExpression(ctx, stmt->condition);
  while (lastValue->isTruthy()) {
    lastValue = evalStatement(ctx, stmt->body);
    if (isReturnObject(lastValue)) {
      return lastValue;
    } else if (isBreakObject(lastValue)) {
      return NULL_OBJECT_PTR;
    }
    lastValue = evalExpression(ctx, stmt->condition);
  }
  return lastValue;
}

ObjectPtr Evaluator::evalPrintStatement(EnvironmentPtr ctx,
                                        PrintStatementPtr stmt) {
  ObjectPtr lastValue = evalExpression(ctx, stmt->expression);
  std::cout << lastValue->toString() << std::endl;
  return lastValue;
}

ObjectPtr Evaluator::evalReturnStatement(EnvironmentPtr ctx,
                                         ReturnStatementPtr stmt) {
  ObjectPtr lastValue = evalExpression(ctx, stmt->expression);
  return ReturnObject::make(lastValue);
}

ObjectPtr Evaluator::evalBreakStatement(EnvironmentPtr ctx,
                                        BreakStatementPtr stmt) {
  return BreakObject::make();
}

ObjectPtr Evaluator::evalContinueStatement(EnvironmentPtr ctx,
                                           ContinueStatementPtr stmt) {
  return ContinueObject::make();
}

ObjectPtr Evaluator::evalExpression(EnvironmentPtr ctx, ExpressionPtr expr) {
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
    case NodeType::ARRAY_LITERAL: {
      auto arrayExpr = std::static_pointer_cast<ArrayLiteral>(expr);
      return evalArrayLiteral(ctx, arrayExpr);
    }
    case NodeType::ARRAY_SUBSCRIPT_EXPRESSION: {
      auto arraySubscriptExpr =
          std::static_pointer_cast<ArraySubscriptExpr>(expr);
      return evalArraySubscriptExpression(ctx, arraySubscriptExpr);
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
      return ctx->get(varExpr->identifier);
    }
    case NodeType::ASSIGNMENT_EXPRESSION: {
      auto assignExpr = std::static_pointer_cast<Assignment>(expr);
      return evalAssignExpression(ctx, assignExpr);
    }
    case NodeType::CALL_EXPRESSION: {
      auto callExpr = std::static_pointer_cast<CallExpr>(expr);
      return evalCallExpression(ctx, callExpr);
    }
    case NodeType::MEMBER_EXPRESSION: {
      auto memberExpr = std::static_pointer_cast<MemberExpr>(expr);
      return evalMemberExpr(ctx, memberExpr);
    }
    default:
      break;
  }
  return NULL_OBJECT_PTR;
}

ObjectPtr Evaluator::evalAssignExpression(EnvironmentPtr ctx,
                                          AssignmentPtr expr) {
  const auto& identifier = expr->identifier;
  auto value = evalExpression(ctx, expr->value);
  ctx->set(identifier, value);
  return ctx->get(identifier);
}

ObjectPtr Evaluator::evalCallExpression(EnvironmentPtr ctx, CallExprPtr expr) {
  auto value = evalExpression(ctx, expr->left);
  if (isReturnObject(value)) {
    auto returnValue = std::dynamic_pointer_cast<ReturnObject>(value);
    value = returnValue->Value;
  }
  if (!isCallable(value)) {
    std::ostringstream ss;
    ss << "Invalid callable: " << value->toString();
    throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
  if (value->Type == ObjectType::OBJ_FUNCTION) {
    auto funcValue = std::dynamic_pointer_cast<Function>(value);
    assert(funcValue != nullptr);
    return evalFunctionCall(ctx, funcValue, expr);
  } else if (value->Type == ObjectType::OBJ_CLASS) {
    auto classDeclValue = std::dynamic_pointer_cast<ClassObject>(value);
    return evalClassCall(ctx, classDeclValue, expr);
  }
  throw RuntimeError::make(__FILE__, __LINE__, "Invalid callable");
}

ObjectPtr Evaluator::evalFunctionCall(EnvironmentPtr ctx, FunctionPtr callee,
                                      CallExprPtr expr) {
  auto funcDeclStmt = callee->getDeclaration();
  auto funcCtx = callee->getCtx();
  // bind arguments
  for (size_t i = 0; i < funcDeclStmt->params.size(); i++) {
    const auto paramName = funcDeclStmt->params[i];
    auto argExpr = expr->arguments[i];
    auto argValue = evalExpression(ctx, argExpr);
    funcCtx->declare(paramName, argValue);
  }
  // execute function body
  auto lastValue = evalStatement(funcCtx, funcDeclStmt->body);
  if (isReturnObject(lastValue)) {
    auto returnValue = std::dynamic_pointer_cast<ReturnObject>(lastValue);
    assert(returnValue != nullptr);
    return returnValue->Value;
  } else if (isBreakObject(lastValue) || isContinueObject(lastValue)) {
    std::ostringstream ss;
    ss << "Invalid statement: " << funcDeclStmt->toString();
    throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
  return lastValue;
}

ObjectPtr Evaluator::evalClassCall(EnvironmentPtr ctx, ClassObjectPtr callee,
                                   CallExprPtr expr) {
  auto recordCtx = Environment::make(ctx);
  auto recordObj = Record::make(recordCtx, callee->declaration);
  recordCtx->declare("self", recordObj);

  auto classDecl = callee->declaration;
  for (auto& field : classDecl->fields) {
    const auto& fieldName = field->identifier;
    auto fieldValue = evalVarDeclarationStatement(recordCtx, field);
    recordObj->setField(fieldName, fieldValue);
  }

  for (auto& method : classDecl->methods) {
    const auto& methodName = method->identifier;
    const auto arity = method->params.size();
    auto functionObj = evalFuncDeclarationStatement(recordCtx, method,
                                                    FunctionType::TYPE_METHOD);
    assert(functionObj->Type == ObjectType::OBJ_FUNCTION);
    auto methodObj = std::dynamic_pointer_cast<Function>(functionObj);
    assert(methodObj != nullptr);
    recordObj->setMethod(methodName, methodObj);
  }

  // If we have a ctor, invoke it.
  if (callee->declaration->ctor) {
    // we don't store ctors, they are invoked only once per object.
    auto functionObj = evalFuncDeclarationStatement(
        recordCtx, callee->declaration->ctor, FunctionType::TYPE_INITIALIZER);
    assert(functionObj->Type == ObjectType::OBJ_FUNCTION);
    auto ctor = std::dynamic_pointer_cast<Function>(functionObj);
    assert(ctor != nullptr);
    evalFunctionCall(recordCtx, ctor, expr);
  }
  return recordObj;
}

IntegerObjectPtr Evaluator::evalIntegerLiteral(EnvironmentPtr ctx,
                                               IntegerLiteralPtr expr) {
  return std::make_shared<IntegerObject>(expr->Value);
}

ObjectPtr Evaluator::evalMemberExpr(EnvironmentPtr ctx, MemberExprPtr expr) {
  auto varValue = evalExpression(ctx, expr->left);
  if (varValue->Type == ObjectType::OBJ_RECORD) {
    auto recordValue = std::dynamic_pointer_cast<Record>(varValue);
    assert(recordValue != nullptr);
    auto memberIdentifier = expr->member;
    if (recordValue->methods.find(memberIdentifier) !=
        recordValue->methods.end()) {
      return recordValue->methods[memberIdentifier];
    }
    return NULL_OBJECT_PTR;
  } else {
    std::ostringstream ss;
    ss << "Invalid member expression: " << expr->toString();
    throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
}

BooleanObjectPtr Evaluator::evalBooleanLiteral(EnvironmentPtr ctx,
                                               BooleanLiteralPtr expr) {
  if (expr->Value) {
    return TRUE_OBJECT_PTR;
  } else {
    return FALSE_OBJECT_PTR;
  }
}

NullObjectPtr Evaluator::evalNilLiteral(EnvironmentPtr ctx,
                                        NilLiteralPtr expr) {
  return NULL_OBJECT_PTR;
}

StringObjectPtr Evaluator::evalStringLiteral(EnvironmentPtr ctx,
                                             StringLiteralPtr expr) {
  return std::make_shared<StringObject>(expr->Value);
}

ArrayObjectPtr Evaluator::evalArrayLiteral(EnvironmentPtr ctx,
                                           ArrayLiteralPtr expr) {
  std::vector<ObjectPtr> elements;
  for (auto elementExpr : expr->elements) {
    auto elementValue = evalExpression(ctx, elementExpr);
    elements.push_back(elementValue);
  }
  return ArrayObject::make(elements);
}

ObjectPtr Evaluator::evalArraySubscriptExpression(EnvironmentPtr ctx,
                                                  ArraySubscriptExprPtr expr) {
  auto arrayValue = evalExpression(ctx, expr->array);
  auto indexValue = evalExpression(ctx, expr->index);
  if (arrayValue->Type != ObjectType::OBJ_ARRAY) {
    std::ostringstream ss;
    ss << "Invalid array: " << arrayValue->toString();
    throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
  if (indexValue->Type != ObjectType::OBJ_INTEGER) {
    std::ostringstream ss;
    ss << "Invalid index: " << indexValue->toString();
    throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
  auto arrayObject = std::dynamic_pointer_cast<ArrayObject>(arrayValue);
  auto indexObject = std::dynamic_pointer_cast<IntegerObject>(indexValue);
  assert(arrayObject != nullptr);
  assert(indexObject != nullptr);
  if (indexObject->Value < 0 ||
      indexObject->Value >= arrayObject->Values.size()) {
    std::ostringstream ss;
    ss << "Index out of range: " << indexObject->Value;
    throw RuntimeError::make(__FILE__, __LINE__, ss.str());
  }
  return arrayObject->Values[indexObject->Value];
}

ObjectPtr Evaluator::evalBinaryExpression(EnvironmentPtr ctx,
                                          BinaryExprPtr expr) {
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
    case TokenType::TOKEN_GREATER_EQUAL: {
      auto result = evalComparisonOperator(ctx, leftValue, expr->operator_.type,
                                           rightValue);
      return result;
    }
    default:
      // TODO: throw RuntimeError
      return NULL_OBJECT_PTR;
  }
}

ObjectPtr Evaluator::evalUnaryExpression(EnvironmentPtr ctx,
                                         UnaryExprPtr expr) {
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

ObjectPtr Evaluator::evalLogicOperator(EnvironmentPtr ctx, ObjectPtr lhsValue,
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

ObjectPtr Evaluator::evalComparisonOperator(EnvironmentPtr ctx,
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
      break;
    }
    case TokenType::TOKEN_GREATER: {
      auto lhsIntValue = tryCastAsInteger(lhsValue);
      auto rhsIntValue = tryCastAsInteger(rhsValue);
      result = lhsIntValue->Value > rhsIntValue->Value;
      break;
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

ObjectPtr Evaluator::evalBinaryOperator(EnvironmentPtr ctx, ObjectPtr lhsValue,
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

ObjectPtr Evaluator::evalMinusOperator(EnvironmentPtr ctx, ObjectPtr rhsValue) {
  auto intObj = tryCastAsInteger(rhsValue);
  return IntegerObject::make(-intObj->Value);
}

ObjectPtr Evaluator::evalBangOperator(EnvironmentPtr ctx, ObjectPtr rhsValue) {
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
