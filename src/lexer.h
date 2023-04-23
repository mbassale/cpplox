
#pragma once

#include <FlexLexer.h>

#include <iostream>
#include <stack>
#include <string>

#include "ast.h"
#include "astbuilder.h"
#include "parser.h"

class JSLexer : public yyFlexLexer {
 public:
  JSLexer(std::istream* in) : yyFlexLexer(in) {}
};

int yylex(Parser::JSParser::value_type* value, ASTBuilder& builder,
          JSLexer& lexer);