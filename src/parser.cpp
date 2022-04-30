#include "parser.h"

namespace cpplox {

ast::ProgramPtr Parser::parse() {
  bool keepRunning = true;

  ast::ProgramPtr programNode = std::make_shared<ast::Program>();

  previous = Token();
  current = scanner.next();
  while (!isAtEnd()) {
    try {
      const auto stmt = statement();
      programNode->statements.push_back(stmt);
    } catch (ParserException& ex) {
      std::cerr << "Error: " << ex.what() << std::endl;
      errors.push_back(ex);
    }
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
  } else if (match(TOKEN_SEMICOLON)) {
    return std::make_shared<ast::Statement>();
  } else {
    return std::static_pointer_cast<ast::Statement>(expressionStatement());
  }
}

ast::BlockPtr Parser::block() {
  auto blockStmt = std::make_shared<ast::Block>();
  while (!match(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
    auto stmt = statement();
    blockStmt->statements.push_back(stmt);
  }
  return blockStmt;
}

ast::ForStatementPtr Parser::forStatement() {
  consume(TOKEN_LEFT_PAREN, "Missing left paren");
  ast::ExpressionPtr initializer{nullptr};
  if (!match(TOKEN_SEMICOLON)) {
    initializer = expression();
    consume(TOKEN_SEMICOLON, "Missing semicolon after initializer");
  }
  ast::ExpressionPtr condition{nullptr};
  if (!match(TOKEN_SEMICOLON)) {
    condition = expression();
    consume(TOKEN_SEMICOLON, "Missing semicolon after initializer");
  }
  ast::ExpressionPtr increment{nullptr};
  if (!match(TOKEN_RIGHT_PAREN)) {
    increment = expression();
    consume(TOKEN_RIGHT_PAREN, "Missing right paren");
  }
  ast::StatementPtr body = statement();
  auto forStmt = std::make_shared<ast::ForStatement>();
  forStmt->initializer = initializer;
  forStmt->condition = condition;
  forStmt->increment = increment;
  forStmt->body = body;
  return std::static_pointer_cast<ast::ForStatement>(forStmt);
}

/**
 * expressionStatement: expression ";" ;
 *
 * @return ast::ExpressionStatement
 */
ast::ExpressionStatementPtr Parser::expressionStatement() {
  const auto expr = expression();
  consume(TOKEN_SEMICOLON, "Missing semicolon");
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
      consume(TOKEN_RIGHT_PAREN, "Unbalanced parenthesis");
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
}

void Parser::consume(TokenType tokenType, const std::string& error_message) {
  if (current.type == tokenType) {
    advance();
    return;
  }
  throw ParserException(error_message, current);
}

}  // namespace cpplox