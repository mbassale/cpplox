#include "lexer.h"

#include "ast.h"

int yylex(PythonParser::PythonParser::value_type* yylval, ASTBuilder& builder,
          Scanner& scanner) {
  auto token = scanner.next();
  switch (token.type) {
    case TokenType::TOKEN_NUMBER:
      yylval->emplace<Token>(token);
      return PythonParser::PythonParser::token::INTEGER;
    case TokenType::TOKEN_IDENTIFIER:
      yylval->emplace<Token>(token);
      return PythonParser::PythonParser::token::IDENTIFIER;
    case TokenType::TOKEN_STRING:
      yylval->emplace<Token>(token);
      return PythonParser::PythonParser::token::STRING_LITERAL;
    case TokenType::TOKEN_EOF:
      return PythonParser::PythonParser::token::YYEOF;
    default:
      return PythonParser::PythonParser::token::YYEMPTY;
  }
}