#include "astbuilder.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "scanner.h"

using Parser::JSParser;

extern JSParser::value_type yylval;

int yylex(JSParser::value_type* value, ASTBuilder& builder, JSLexer& lexer) {
  int yyToken = lexer.yylex();
  const auto& token = yylval.as<Token>();
  LOG(INFO) << "YYTOKEN: " << yyToken << " TOKEN: " << (int)token.type << " "
            << token.lexeme();
  value->emplace<Token>(yylval.as<Token>());
  return yyToken;
}