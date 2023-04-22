
#pragma once

#include <FlexLexer.h>

#include <iostream>
#include <stack>
#include <string>
#include "ast.h"
#include "astbuilder.h"
#include "parser.h"

class PythonLexer : public yyFlexLexer {
 public:
  PythonLexer(std::istream *in) : yyFlexLexer(in) {}
};

int yylex(Parser::PythonParser::value_type* value, ASTBuilder& builder,
          PythonLexer& lexer);