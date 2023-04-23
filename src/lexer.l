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
#include "parser.h" // Include Bison-generated header file for token definitions

using Parser::PythonParser;
PythonParser::value_type yylval;
std::string currentString;
std::stack<int> indentStack;
%}

%x INDENT_MODE
%x STRING

%%

  /* String Literal */
<STRING>\" { BEGIN(INITIAL); 
  yylval.emplace<Token>(Token(TokenType::TOKEN_STRING, currentString));
  return PythonParser::token::STRING_LITERAL; 
}
<STRING>[^\"]* { currentString = yytext; }
<*>\" { BEGIN(STRING); }

  /* Keywords */
<*>while    { return PythonParser::token::WHILE; }
<*>if       { return PythonParser::token::IF; }
<*>def      { return PythonParser::token::DEF; }
<*>True     { return PythonParser::token::TRUE; }  
<*>False    { return PythonParser::token::FALSE; }
<*>None     { return PythonParser::token::NONE; }

  /* Integer */
<*>[0-9]+ { 
  yylval.emplace<Token>(Token(TokenType::TOKEN_NUMBER, yytext));
  return PythonParser::token::INTEGER; 
}

  /* Identifier */
<*>[a-zA-Z_][a-zA-Z0-9_]* { 
  yylval.emplace<Token>(Token(TokenType::TOKEN_IDENTIFIER, yytext));
  return PythonParser::token::IDENTIFIER; 
}

  /* Operators */
<*>\+ { return PythonParser::token::PLUS; }
<*>\- { return PythonParser::token::MINUS; }
<*>\* { return PythonParser::token::STAR; }
<*>\/ { return PythonParser::token::SLASH; }
<*>\( { return PythonParser::token::LPAREN; }
<*>\) { return PythonParser::token::RPAREN; }
<*>\{ { return PythonParser::token::LBRACE; }
<*>\} { return PythonParser::token::RBRACE; }
<*>\[ { return PythonParser::token::LBRACKET; }
<*>\] { return PythonParser::token::RBRACKET; }
<*>\= { return PythonParser::token::EQUAL; }
<*>\, { return PythonParser::token::COMMA; }
<*>\. { return PythonParser::token::DOT; }
<*>\! { return PythonParser::token::BANG; }
<*>\!\= { return PythonParser::token::BANG_EQUAL; }
<*>\=\= { return PythonParser::token::EQUAL_EQUAL; }
<*>\> { return PythonParser::token::GREATER; }
<*>\>\= { return PythonParser::token::GREATER_EQUAL; }
<*>\< { return PythonParser::token::LESS; }
<*>\<\= { return PythonParser::token::LESS_EQUAL; }
<*>\; { return PythonParser::token::SEMICOLON; }
<*>\: { return PythonParser::token::COLON; }

  /* Identation and Blocks */
<*>[ \t]*\n { 
  BEGIN(INDENT_MODE); 
  return PythonParser::token::NEWLINE; 
}
<INDENT_MODE>[ \t]+ {
  indentStack.push(yyleng);
  return PythonParser::token::INDENT;
}
<INDENT_MODE>[^ \t\n] {
  while (!indentStack.empty() && indentStack.top() >= yyleng) {
    indentStack.pop();
    return PythonParser::token::DEDENT;
  }
  BEGIN(INITIAL);
  yyless(0); // Put back the non-whitespace character
}

<*>[ \t]+ { /* Ignore whitespace */ }
<*>. { /* Ignore any other characters */ }

%%