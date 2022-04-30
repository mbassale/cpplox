#include "parser.h"

namespace cpplox {

ast::ProgramPtr Parser::parse() {
  bool keepRunning = true;

  ast::ProgramPtr programNode = std::make_shared<ast::Program>();

  previous = Token();
  current = scanner.next();
  std::cout << "Token: " << current.lexeme() << std::endl;
  while (!isAtEnd()) {
    const auto stmt = statement();
    programNode->statements.push_back(stmt);
  }

  return programNode;
}

/**
 * statement:
 *  exprStmt
 *  | forStmt
 *  | ifStmt
 *  | printStmt
 *  | returnStmt
 *  | whileStmt
 *  | block ;
 *
 * @return ast::Statement
 */
ast::StatementPtr Parser::statement() {
  if (match(TOKEN_LEFT_BRACE)) {
    return std::static_pointer_cast<ast::Statement>(block());
  } else if (match(TOKEN_FOR)) {
    return std::static_pointer_cast<ast::ForStatement>(forStatement());
  } else {
    return std::static_pointer_cast<ast::Statement>(expressionStatement());
  }
}

ast::BlockPtr Parser::block() {
  auto blockStmt = std::make_shared<ast::Block>();
  while (!match(TOKEN_RIGHT_BRACE) && isAtEnd()) {
    auto stmt = statement();
    blockStmt->statements.push_back(stmt);
  }
  return blockStmt;
}

ast::ForStatementPtr Parser::forStatement() {
  ast::ExpressionPtr initializer{nullptr};
  if (!match(TOKEN_SEMICOLON)) {
    initializer = expression();
  }
  ast::ExpressionPtr condition{nullptr};
  if (!match(TOKEN_SEMICOLON)) {
    condition = expression();
  }
  ast::ExpressionPtr increment{nullptr};
  if (!match(TOKEN_SEMICOLON)) {
    increment = expression();
  }
  auto forStmt = std::make_shared<ast::ForStatement>();
  forStmt->initializer = initializer;
  forStmt->condition = condition;
  forStmt->increment = increment;
  return std::static_pointer_cast<ast::ForStatement>(forStmt);
}

/**
 * expressionStatement: expression ";" ;
 *
 * @return ast::ExpressionStatement
 */
ast::ExpressionStatementPtr Parser::expressionStatement() {
  const auto expr = expression();
  consume(TOKEN_SEMICOLON, "Missing semicolon.");
  auto exprStmt = std::make_shared<ast::ExpressionStatement>();
  exprStmt->expression = expr;
  return exprStmt;
}

ast::ExpressionPtr Parser::expression() {
  return std::static_pointer_cast<ast::Expression>(primary());
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
    case TOKEN_TRUE:
    case TOKEN_FALSE:
    case TOKEN_NIL:
    case TOKEN_NUMBER:
    case TOKEN_STRING:
    case TOKEN_IDENTIFIER:
      advance();
      return std::static_pointer_cast<ast::Expression>(
          ast::Literal::make(previous));
    case TOKEN_LEFT_PAREN: {
      const auto expr = expression();
      consume(TOKEN_RIGHT_PAREN, "Unbalanced parenthesis.");
      return expr;
    }
    default:
      throw std::runtime_error("Unknown primary.");
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
  std::cout << "Token: " << current.lexeme() << std::endl;
}

void Parser::consume(TokenType tokenType, const std::string& error_message) {
  if (match(tokenType)) {
    advance();
    return;
  }
  throw std::runtime_error(error_message);
}

}  // namespace cpplox