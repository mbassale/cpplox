#include "common.h"
#include "lexer.h"
#include "astbuilder.h"
#include "scanner.h"
#include "python_parser.hpp"

extern PythonParser::PythonParser::value_type yylval;

int yylex(PythonParser::PythonParser::value_type* value, ASTBuilder& builder, PythonLexer &lexer) {
  int yyToken = lexer.yylex();
  const auto& token = yylval.as<Token>();
  LOG(INFO) << "YYTOKEN: " << yyToken <<  " TOKEN: " << (int)token.type << " " << token.lexeme();
  value->emplace<Token>(yylval.as<Token>());
  return yyToken;
}