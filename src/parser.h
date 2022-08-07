#ifndef __cpplox_h_parser_h
#define __cpplox_h_parser_h

#include "ast.h"
#include "common.h"
#include "scanner.h"

namespace cpplox {

class ParserException : public std::runtime_error {
 private:
  Token location;

 public:
  ParserException(const std::string& error_message, const Token& location)
      : std::runtime_error(error_message), location(location) {}

  const Token getLocation() const { return location; }
};

class Parser {
 private:
  Token previous;
  Token current;
  Scanner& scanner;
  std::vector<ParserException> errors;

 public:
  Parser(Scanner& scanner)
      : scanner(scanner), previous(), current(), errors() {}

  ast::ProgramPtr parse();

  inline bool hasErrors() const { return !errors.empty(); }
  inline const std::vector<ParserException>& getErrors() const {
    return errors;
  }

 private:
  ast::StatementPtr declaration();
  ast::VarDeclarationPtr varDeclaration();
  ast::FunctionDeclarationPtr functionDeclaration();
  ast::BlockPtr block();
  ast::StatementPtr statement();
  ast::ForStatementPtr forStatement();
  ast::IfStatementPtr ifStatement();
  ast::WhileStatementPtr whileStatement();
  ast::PrintStatementPtr printStatement();
  ast::ReturnStatementPtr returnStatement();
  ast::ExpressionStatementPtr expressionStatement();
  ast::ExpressionPtr expression();
  ast::ExpressionPtr assignment();
  ast::ExpressionPtr logicOr();
  ast::ExpressionPtr logicAnd();
  ast::ExpressionPtr equality();
  ast::ExpressionPtr comparison();
  ast::ExpressionPtr term();
  ast::ExpressionPtr factor();
  ast::ExpressionPtr unary();
  ast::ExpressionPtr primary();

  bool isAtEnd() { return current.type == TokenType::TOKEN_EOF; }
  bool match(TokenType tokenType);
  void advance();
  void consume(TokenType tokenType, const std::string& error_message);
  void moveToNextStatement();
};

}  // namespace cpplox

#endif  // __cpplox_h_parser_h