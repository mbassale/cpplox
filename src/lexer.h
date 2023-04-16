#pragma once

#include "common.h"
#include "scanner.h"
#include "ast.h"
#include "python_parser.hpp"

class ASTBuilder;
int yylex(PythonParser::PythonParser::value_type* yylval, ASTBuilder& builder, Scanner& scanner);
