#include "parser.h"

namespace cpplox {

ast::ProgramPtr Parser::parse() {
  bool keepRunning = true;

  ast::ProgramPtr programNode = std::make_shared<ast::Program>();

  previous = Token();
  current = scanner.next();
  VLOG(1) << "TOKEN: " << current.str();
  while (!isAtEnd()) {
    try {
      const auto stmt = declaration();
      programNode->statements.push_back(stmt);
    } catch (ParserException& ex) {
      LOG(ERROR) << ex.what();
      errors.push_back(ex);
      // TODO: move to next statement to keep gathering errors
      break;
    }
  }

  return programNode;
}

/**
 * @brief declaration: classDecl | functionDeclaration | varDeclaration |
 * statement ;
 *
 * @return ast::StatementPtr
 */
ast::StatementPtr Parser::declaration() {
  if (match(TokenType::TOKEN_VAR)) {
    return varDeclaration();
  } else if (match(TokenType::TOKEN_FUN)) {
    return functionDeclaration();
  } else {
    return statement();
  }
}

/**
 * @brief varDeclaration: "var" IDENTIFIER ( "=" expression )? ";" ;
 *
 * @return ast::VarDeclarationPtr
 */
ast::VarDeclarationPtr Parser::varDeclaration() {
  auto identifier = current;
  consume(TokenType::TOKEN_IDENTIFIER, "Invalid identifier");
  ast::ExpressionPtr initializer = nullptr;
  if (match(TokenType::TOKEN_EQUAL)) {
    initializer = expression();
  }
  consume(TokenType::TOKEN_SEMICOLON, "Missing semicolon");
  return ast::VarDeclaration::make(identifier, initializer);
}

/**
 * @brief functionDeclaration: "fun" function ;
 * function: IDENTIFIER "(" parameters? ")" block ;
 *
 * @return ast::FunctionDeclarationPtr
 */
ast::FunctionDeclarationPtr Parser::functionDeclaration() {
  auto identifier = current;
  consume(TokenType::TOKEN_IDENTIFIER, "Invalid identifier");
  consume(TokenType::TOKEN_LEFT_PAREN, "Missing left paren");
  std::vector<Token> params;
  while (match(TokenType::TOKEN_IDENTIFIER)) {
    params.push_back(previous);
    match(TokenType::TOKEN_COMMA);
  }
  consume(TokenType::TOKEN_RIGHT_PAREN, "Missing right paren");
  consume(TokenType::TOKEN_LEFT_BRACE, "Missing right brace");
  auto body = block();
  return ast::FunctionDeclaration::make(identifier, params, body);
}

/**
 * @brief statement: exprStmt | forStmt | ifStmt | printStmt | returnStmt |
 * whileStmt | block ;
 *
 * @return ast::StatementPtr
 */
ast::StatementPtr Parser::statement() {
  if (match(TokenType::TOKEN_LEFT_BRACE)) {
    return block();
  } else if (match(TokenType::TOKEN_FOR)) {
    return forStatement();
  } else if (match(TokenType::TOKEN_IF)) {
    return ifStatement();
  } else if (match(TokenType::TOKEN_WHILE)) {
    return whileStatement();
  } else if (match(TokenType::TOKEN_PRINT)) {
    return printStatement();
  } else if (match(TokenType::TOKEN_RETURN)) {
    return returnStatement();
  } else if (match(TokenType::TOKEN_SEMICOLON)) {
    return ast::Statement::make();
  } else {
    return expressionStatement();
  }
}

ast::BlockPtr Parser::block() {
  auto blockStmt = std::make_shared<ast::Block>();
  while (!match(TokenType::TOKEN_RIGHT_BRACE) && !isAtEnd()) {
    auto stmt = statement();
    blockStmt->statements.push_back(stmt);
  }
  return blockStmt;
}

ast::ForStatementPtr Parser::forStatement() {
  consume(TokenType::TOKEN_LEFT_PAREN, "Missing left paren");
  ast::StatementPtr initializer{nullptr};
  if (match(TokenType::TOKEN_SEMICOLON)) {
    // no initializer.
  } else if (match(TokenType::TOKEN_VAR)) {
    initializer = varDeclaration();
  } else {
    initializer = expressionStatement();
  }
  ast::ExpressionPtr condition{nullptr};
  if (!match(TokenType::TOKEN_SEMICOLON)) {
    condition = expression();
    consume(TokenType::TOKEN_SEMICOLON, "Missing semicolon after initializer");
  }
  ast::ExpressionPtr increment{nullptr};
  if (!match(TokenType::TOKEN_RIGHT_PAREN)) {
    increment = expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Missing right paren");
  }
  auto body = statement();
  return ast::ForStatement::make(initializer, condition, increment, body);
}

ast::IfStatementPtr Parser::ifStatement() {
  consume(TokenType::TOKEN_LEFT_PAREN, "Missing left paren");
  auto condition = expression();
  consume(TokenType::TOKEN_RIGHT_PAREN, "Missing right paren");
  auto thenBranch = statement();
  ast::StatementPtr elseBranch{nullptr};
  if (match(TokenType::TOKEN_ELSE)) {
    elseBranch = statement();
  }
  return ast::IfStatement::make(condition, thenBranch, elseBranch);
}

/**
 * @brief whileStatement: "while" "(" expression ")" statement ;
 *
 * @return ast::WhileStatementPtr
 */
ast::WhileStatementPtr Parser::whileStatement() {
  consume(TokenType::TOKEN_LEFT_PAREN, "Missing left paren");
  auto condition = expression();
  consume(TokenType::TOKEN_RIGHT_PAREN, "Missing right paren");
  auto body = statement();
  return ast::WhileStatement::make(condition, body);
}

/**
 * @brief printStatement: "print" expression ";" ;
 *
 * @return ast::PrintStatementPtr
 */
ast::PrintStatementPtr Parser::printStatement() {
  auto expr = expression();
  consume(TokenType::TOKEN_SEMICOLON, "Missing semicolon");
  return ast::PrintStatement::make(expr);
}

/**
 * @brief returnStatement: "return" expression? ";" ;
 *
 * @return ast::ReturnStatementPtr
 */
ast::ReturnStatementPtr Parser::returnStatement() {
  ast::ExpressionPtr expr = nullptr;
  if (!match(TokenType::TOKEN_SEMICOLON)) {
    expr = expression();
    consume(TokenType::TOKEN_SEMICOLON, "Missing semicolon");
  }
  return ast::ReturnStatement::make(expr);
}

/**
 * @brief expressionStatement: expression ";" ;
 *
 * @return ast::ExpressionStatementPtr
 */
ast::ExpressionStatementPtr Parser::expressionStatement() {
  const auto expr = expression();
  consume(TokenType::TOKEN_SEMICOLON, "Missing semicolon");
  auto exprStmt = std::make_shared<ast::ExpressionStatement>();
  exprStmt->expression = expr;
  return exprStmt;
}

/**
 * @brief expression: assignment
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::expression() { return assignment(); }

/**
 * @brief assignment: ( call "." )? IDENTIFIER "=" assignment | logicOr ;
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::assignment() {
  auto expr = logicOr();
  if (match(TokenType::TOKEN_EQUAL)) {
    auto value = assignment();
    if (expr->Type == ast::NodeType::VARIABLE_EXPRESSION) {
      const auto varExpr = std::dynamic_pointer_cast<ast::VariableExpr>(expr);
      return ast::Assignment::make(varExpr->identifier, value);
    }
    throw ParserException("Invalid assignment target", current);
  }
  return expr;
}

/**
 * @brief logicOr: logicAnd ( "or" logicAnd )* ;
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::logicOr() {
  auto expr = logicAnd();

  if (match(TokenType::TOKEN_OR)) {
    auto operator_ = previous;
    auto rhsExpr = logicAnd();
    return ast::BinaryExpr::make(expr, operator_, rhsExpr);
  }

  return expr;
}

/**
 * @brief  logicAnd: equality ( "and" equality )* ;
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::logicAnd() {
  auto expr = equality();

  if (match(TokenType::TOKEN_AND)) {
    auto operator_ = previous;
    auto rhsExpr = equality();
    return ast::BinaryExpr::make(expr, operator_, rhsExpr);
  }

  return expr;
}

/**
 * @brief equality: comparison ( ( "!=" | "==" ) comparison )* ;
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::equality() {
  auto expr = comparison();

  while (match(TokenType::TOKEN_EQUAL_EQUAL) ||
         match(TokenType::TOKEN_BANG_EQUAL)) {
    const auto operator_ = previous;
    const auto right = comparison();
    expr = ast::BinaryExpr::make(expr, operator_, right);
  }

  return expr;
}

/**
 * @brief comparison: term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::comparison() {
  auto expr = term();

  while (match(TokenType::TOKEN_GREATER) ||
         match(TokenType::TOKEN_GREATER_EQUAL) ||
         match(TokenType::TOKEN_LESS) || match(TokenType::TOKEN_LESS_EQUAL)) {
    const auto operator_ = previous;
    const auto right = term();
    expr = ast::BinaryExpr::make(expr, operator_, right);
  }

  return expr;
}

/**
 * @brief term: factor ( ( "-" | "+" ) factor )* ;
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::term() {
  auto expr = factor();

  while (match(TokenType::TOKEN_PLUS) || match(TokenType::TOKEN_MINUS)) {
    const auto operator_ = previous;
    const auto right = factor();
    expr = ast::BinaryExpr::make(expr, operator_, right);
  }

  return expr;
}

/**
 * @brief factor: unary ( ( "/" | "*" ) unary )* ;
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::factor() {
  auto expr = unary();

  while (match(TokenType::TOKEN_STAR) || match(TokenType::TOKEN_SLASH)) {
    const auto operator_ = previous;
    const auto right = unary();
    expr = ast::BinaryExpr::make(expr, operator_, right);
  }

  return expr;
}

/**
 * @brief unary: ( "!" | "-" ) unary | primary ;
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::unary() {
  if (match(TokenType::TOKEN_BANG) || match(TokenType::TOKEN_MINUS)) {
    const auto operator_ = previous;
    const auto right = unary();
    return ast::UnaryExpr::make(operator_, right);
  }

  return primary();
}

/**
 * primary:
 *  "true"
 *  | "false"
 *  | "nil"
 *  | "this"
 *  | NUMBER
 *  | STRING
 *  | IDENTIFIER
 *  | "(" expression ")"
 *  | "super" "." IDENTIFIER ;
 *
 * @return ast::ExpressionPtr
 */
ast::ExpressionPtr Parser::primary() {
  switch (current.type) {
    case TokenType::TOKEN_TRUE:
      advance();
      return ast::BooleanLiteral::makeTrue();
    case TokenType::TOKEN_FALSE:
      advance();
      return ast::BooleanLiteral::makeFalse();
    case TokenType::TOKEN_NIL:
      advance();
      return ast::NilLiteral::make();
    case TokenType::TOKEN_NUMBER:
      advance();
      return ast::IntegerLiteral::make(std::stod(previous.lexeme()));
    case TokenType::TOKEN_STRING: {
      advance();
      const auto lexeme = previous.lexeme();
      assert(lexeme.size() >= 2);
      const auto strLiteral = lexeme.substr(1, lexeme.size() - 2);
      return ast::StringLiteral::make(strLiteral);
    }
    case TokenType::TOKEN_IDENTIFIER:
      advance();
      return ast::VariableExpr::make(previous.lexeme());
    case TokenType::TOKEN_LEFT_PAREN: {
      const auto expr = expression();
      consume(TokenType::TOKEN_RIGHT_PAREN, "Unbalanced parenthesis");
      return expr;
    }
    default:
      throw ParserException("Unknown primary", current);
  }
}

bool Parser::match(TokenType tokenType) {
  if (current.type == tokenType) {
    advance();
    return true;
  }
  return false;
}

void Parser::advance() {
  previous = current;
  current = scanner.next();
  VLOG(1) << "TOKEN: " << current.str();
}

void Parser::consume(TokenType tokenType, const std::string& error_message) {
  if (current.type == tokenType) {
    advance();
    return;
  }
  throw ParserException(error_message, current);
}

}  // namespace cpplox