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

class JSLexer;
#include "parser.h" // Include Bison-generated header file for token definitions

using Parser::JSParser;
JSParser::value_type yylval;
std::string currentString;
%}

%x STRING

%%

<*>\/\/[^\n]*\n { /* Ignore single line comments */ }

  /* String Literal */
<STRING>\" { BEGIN(INITIAL); 
  yylval.emplace<Token>(Token(TokenType::TOKEN_STRING, currentString));
  currentString = "";
  return JSParser::token::STRING_LITERAL; 
}
<STRING>[^\"]* { currentString = yytext; }
<*>\" { 
  BEGIN(STRING);
  currentString = ""; 
}

  /* Keywords */
<*>while    { return JSParser::token::WHILE; }
<*>for      { return JSParser::token::FOR; }
<*>and      { return JSParser::token::AND; }
<*>or       { return JSParser::token::OR; }
<*>if       { return JSParser::token::IF; }
<*>else     { return JSParser::token::ELSE; }
<*>def      { return JSParser::token::DEF; }
<*>var      { return JSParser::token::VAR; }
<*>true     { return JSParser::token::TRUE; }  
<*>false    { return JSParser::token::FALSE; }
<*>null     { return JSParser::token::NIL; }
<*>print    { return JSParser::token::PRINT; }
<*>return   { return JSParser::token::RETURN; }
<*>break    { return JSParser::token::BREAK; }

  /* Integer */
<*>[0-9]+ { 
  yylval.emplace<Token>(Token(TokenType::TOKEN_NUMBER, yytext));
  return JSParser::token::INTEGER; 
}

  /* Identifier */
<*>[a-zA-Z_][a-zA-Z0-9_]* { 
  yylval.emplace<Token>(Token(TokenType::TOKEN_IDENTIFIER, yytext));
  return JSParser::token::IDENTIFIER; 
}

  /* Operators and Single Character Tokens */
<*>\+ { return JSParser::token::PLUS; }
<*>\- { return JSParser::token::MINUS; }
<*>\* { return JSParser::token::STAR; }
<*>\/ { return JSParser::token::SLASH; }
<*>\( { return JSParser::token::LPAREN; }
<*>\) { return JSParser::token::RPAREN; }
<*>\{ { return JSParser::token::LBRACE; }
<*>\} { return JSParser::token::RBRACE; }
<*>\[ { return JSParser::token::LBRACKET; }
<*>\] { return JSParser::token::RBRACKET; }
<*>\= { return JSParser::token::EQUAL; }
<*>\, { return JSParser::token::COMMA; }
<*>\. { return JSParser::token::DOT; }
<*>\! { return JSParser::token::BANG; }
<*>\!\= { return JSParser::token::BANG_EQUAL; }
<*>\=\= { return JSParser::token::EQUAL_EQUAL; }
<*>\> { return JSParser::token::GREATER; }
<*>\>\= { return JSParser::token::GREATER_EQUAL; }
<*>\< { return JSParser::token::LESS; }
<*>\<\= { return JSParser::token::LESS_EQUAL; }
<*>\; { return JSParser::token::SEMICOLON; }
<*>\: { return JSParser::token::COLON; }

<*>[ \t\r\n]+ { /* Ignore whitespace */ }
<*>. { /* Ignore any other characters */ }

%%