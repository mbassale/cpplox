#include "astbuilder.h"
#include "common.h"
#include "lexer.h"
#include "python_parser.hpp"
#include "scanner.h"

using Parser::PythonParser;

extern PythonParser::value_type yylval;

int yylex(PythonParser::value_type* value, ASTBuilder& builder,
          PythonLexer& lexer) {
  int yyToken = lexer.yylex();
  const auto& token = yylval.as<Token>();
  LOG(INFO) << "YYTOKEN: " << yyToken << " TOKEN: " << (int)token.type << " "
            << token.lexeme();
  value->emplace<Token>(yylval.as<Token>());
  return yyToken;
}