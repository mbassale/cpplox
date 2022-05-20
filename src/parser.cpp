#include "parser.h"

namespace cpplox {

ast::ProgramPtr Parser::parse() {
  bool keepRunning = true;

  ast::ProgramPtr programNode = std::make_shared<ast::Program>();

  previous = Token();
  current = scanner.next();
  while (!isAtEnd()) {
    try {
      const auto stmt = declaration();
      programNode->statements.push_back(stmt);
    } catch (ParserException& ex) {
      std::cerr << "Error: " << ex.what() << std::endl;
      errors.push_back(ex);
      // TODO: move to next statement to keep gathering errors
      break;
    }
  }

  return programNode;
}

/**
 * declaration:
 *  classDecl
 *   | funDecl
 *   | varDecl
 *   | statement ;
 *
 * @return ast::StatementPtr
 */
ast::StatementPtr Parser::declaration() {
  if (match(TOKEN_VAR)) {
    return varDeclaration();
  } else {
    return statement();
  }
}

/**
 * varDeclaration: "var" IDENTIFIER ( "=" expression )? ";" ;
 *
 * @return ast::VarDeclarationPtr
 */
ast::VarDeclarationPtr Parser::varDeclaration() {
  auto identifier = current;
  consume(TOKEN_IDENTIFIER, "Invalid identifier");
  ast::ExpressionPtr initializer = nullptr;
  if (match(TOKEN_EQUAL)) {
    initializer = expression();
  }
  consume(TOKEN_SEMICOLON, "Missing semicolon");
  return ast::VarDeclaration::make(identifier, initializer);
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
 * @return ast::StatementPtr
 */
ast::StatementPtr Parser::statement() {
  if (match(TOKEN_LEFT_BRACE)) {
    return block();
  } else if (match(TOKEN_FOR)) {
    return forStatement();
  } else if (match(TOKEN_IF)) {
    return ifStatement();
  } else if (match(TOKEN_WHILE)) {
    return whileStatement();
  } else if (match(TOKEN_PRINT)) {
    return printStatement();
  } else if (match(TOKEN_RETURN)) {
    return returnStatement();
  } else if (match(TOKEN_SEMICOLON)) {
    return ast::Statement::make();
  } else {
    return expressionStatement();
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
  auto body = statement();
  return ast::ForStatement::make(initializer, condition, increment, body);
}

ast::IfStatementPtr Parser::ifStatement() {
  consume(TOKEN_LEFT_PAREN, "Missing left paren");
  auto condition = expression();
  consume(TOKEN_RIGHT_PAREN, "Missing right paren");
  auto thenBranch = statement();
  ast::StatementPtr elseBranch{nullptr};
  if (match(TOKEN_ELSE)) {
    elseBranch = statement();
  }
  return ast::IfStatement::make(condition, thenBranch, elseBranch);
}

/**
 * whileStatement: "while" "(" expression ")" statement ;
 *
 * @return ast::WhileStatementPtr
 */
ast::WhileStatementPtr Parser::whileStatement() {
  consume(TOKEN_LEFT_PAREN, "Missing left paren");
  auto condition = expression();
  consume(TOKEN_RIGHT_PAREN, "Missing right paren");
  auto body = statement();
  return ast::WhileStatement::make(condition, body);
}

/**
 * printStatement: "print" expression ";" ;
 *
 * @return ast::PrintStatementPtr
 */
ast::PrintStatementPtr Parser::printStatement() {
  auto expr = expression();
  consume(TOKEN_SEMICOLON, "Missing semicolon");
  return ast::PrintStatement::make(expr);
}

/**
 * returnStatement: "return" expression? ";" ;
 *
 * @return ast::ReturnStatementPtr
 */
ast::ReturnStatementPtr Parser::returnStatement() {
  ast::ExpressionPtr expr = nullptr;
  if (!match(TOKEN_SEMICOLON)) {
    expr = expression();
    consume(TOKEN_SEMICOLON, "Missing semicolon");
  }
  return ast::ReturnStatement::make(expr);
}

/**
 * expressionStatement: expression ";" ;
 *
 * @return ast::ExpressionStatementPtr
 */
ast::ExpressionStatementPtr Parser::expressionStatement() {
  const auto expr = expression();
  consume(TOKEN_SEMICOLON, "Missing semicolon");
  auto exprStmt = std::make_shared<ast::ExpressionStatement>();
  exprStmt->expression = expr;
  return exprStmt;
}

ast::ExpressionPtr Parser::expression() { return primary(); }

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