#include "astbuilder.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

using Parser::JSParser;

extern JSParser::value_type yylval;

int yylex(JSParser::value_type* value, ASTBuilder& builder, JSLexer& lexer) {
  int yyToken = lexer.yylex();
  const auto& token = yylval.as<Token>();
  value->emplace<Token>(yylval.as<Token>());
  return yyToken;
}