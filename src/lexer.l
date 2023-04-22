%option c++
%option yylineno
%option noyywrap

%{
#include <iostream>
#include <string>
#include <stack>
#include "common.h"
#include "scanner.h"
#include "ast.h"

class PythonLexer;
#include "python_parser.hpp" // Include Bison-generated header file for token definitions

PythonParser::PythonParser::value_type yylval;
std::string currentString;
std::stack<int> indentStack;
%}

%x INDENT_MODE
%x STRING

%%

/* String Literal */
<STRING>\" { BEGIN(INITIAL); 
  yylval.emplace<Token>(Token(TokenType::TOKEN_STRING, currentString));
  return PythonParser::PythonParser::token::STRING_LITERAL; 
}
<STRING>[^\"]* { currentString = yytext; }
<*>\" { BEGIN(STRING); }

/* Keywords */
<*>while    { return PythonParser::PythonParser::token::WHILE; }
<*>if       { return PythonParser::PythonParser::token::IF; }
<*>def      { return PythonParser::PythonParser::token::DEF; }

/* Integer */
<*>[0-9]+ { 
  yylval.emplace<Token>(Token(TokenType::TOKEN_NUMBER, yytext));
  return PythonParser::PythonParser::token::INTEGER; 
}

/* Identifier */
<*>[a-zA-Z_][a-zA-Z0-9_]* { 
  yylval.emplace<Token>(Token(TokenType::TOKEN_IDENTIFIER, yytext));
  return PythonParser::PythonParser::token::IDENTIFIER; 
}

/* Operators */
<*>\+ { return PythonParser::PythonParser::token::PLUS; }
<*>\- { return PythonParser::PythonParser::token::MINUS; }
<*>\* { return PythonParser::PythonParser::token::STAR; }
<*>\/ { return PythonParser::PythonParser::token::SLASH; }
<*>\: { return PythonParser::PythonParser::token::COLON; }

/* Identation and Blocks */
<*>[ \t]*\n { 
  BEGIN(INDENT_MODE); 
  return PythonParser::PythonParser::token::NEWLINE; 
}
<INDENT_MODE>[ \t]+ {
  indentStack.push(yyleng);
  return PythonParser::PythonParser::token::INDENT;
}
<INDENT_MODE>[^ \t\n] {
  while (!indentStack.empty() && indentStack.top() >= yyleng) {
    indentStack.pop();
    return PythonParser::PythonParser::token::DEDENT;
  }
  BEGIN(INITIAL);
  yyless(0); // Put back the non-whitespace character
}

<*>[ \t]+ { /* Ignore whitespace */ }
<*>. { /* Ignore any other characters */ }

%%