#ifndef __cpplox_h_parser_h
#define __cpplox_h_parser_h

#include "ast.h"
#include "common.h"
#include "scanner.h"

namespace cpplox {

class Parser {
 private:
  Token previous;
  Token current;
  Scanner& scanner;

 public:
  Parser(Scanner& scanner) : scanner(scanner), previous(), current() {}

  ast::ProgramPtr parse();

 private:
  ast::BlockPtr block();
  ast::StatementPtr statement();
  ast::ForStatementPtr forStatement();
  ast::ExpressionStatementPtr expressionStatement();
  ast::ExpressionPtr expression();
  ast::ExpressionPtr primary();

  bool isAtEnd() { return current.type == TOKEN_EOF; }
  bool match(TokenType tokenType);
  void advance();
  void consume(TokenType tokenType, const std::string& error_message);
};

}  // namespace cpplox

#endif  // __cpplox_h_parser_h