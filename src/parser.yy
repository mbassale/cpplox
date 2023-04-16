%{
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "ast.h"
#include "scanner.h"
#include "lexer.h"

using namespace std;
using namespace cpplox::ast;

%}

%language "c++"
%require "3.8"
%define api.namespace {PythonParser}
%define parse.error verbose
%define api.parser.class {PythonParser}
%define api.value.type variant

%code requires {
    class ASTBuilder {
    public:
        virtual ~ASTBuilder() = default;
        
        virtual cpplox::ast::ProgramPtr emitProgram(const std::vector<cpplox::ast::StatementPtr> &statements) = 0;
        virtual cpplox::ast::ExpressionStatementPtr emitExpressionStatement(cpplox::ast::ExpressionPtr expr) = 0;
        virtual cpplox::ast::IntegerLiteralPtr emitIntegerLiteral(const Token &value) = 0;
        virtual cpplox::ast::StringLiteralPtr emitStringLiteral(const Token &value) = 0;
        virtual cpplox::ast::VariableExprPtr emitIdentifier(const Token &value) = 0;
        virtual cpplox::ast::BinaryExprPtr emitBinaryOp(char op, cpplox::ast::ExpressionPtr lhs, cpplox::ast::ExpressionPtr rhs) = 0;
        virtual cpplox::ast::IfStatementPtr emitIfStatement(cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr body) = 0;
        virtual cpplox::ast::WhileStatementPtr emitWhileStatement(cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr body) = 0;
        virtual cpplox::ast::FunctionDeclarationPtr emitDefStatement(const Token &name, cpplox::ast::BlockPtr body) = 0;
        virtual cpplox::ast::BlockPtr emitBlock(const std::vector<cpplox::ast::StatementPtr> &statements) = 0;
    };
}

%param {ASTBuilder& builder} {Scanner& scanner}

%token<Token> INDENT "INDENT"
%token<Token> DEDENT "DEDENT"
%token<Token> INTEGER "INTEGER"
%token<Token> IDENTIFIER "IDENTIFIER"
%token<Token> STRING_LITERAL "STRING_LITERAL"

%token NEWLINE "NEWLINE"
%token IF "if"
%token WHILE "while"
%token DEF "def"

%type<cpplox::ast::ProgramPtr> program
%type<cpplox::ast::ExpressionPtr> expr
%type<cpplox::ast::BlockPtr> suite
%type<cpplox::ast::StatementPtr> statement
%type<cpplox::ast::ExpressionStatementPtr> simple_statement
%type<std::vector<cpplox::ast::StatementPtr>> statements


%start program

%%

program
    : statements { $$ = builder.emitProgram($1); }
    ;

statements
    : /* empty */ { $$ = std::vector<cpplox::ast::StatementPtr>(); }
    | statements statement
      { $1.push_back($2); $$ = $1; }
    ;

statement
    : simple_statement NEWLINE { $$ = $1; }
    | compound_statement
    ;

simple_statement
    : expr { $$ = builder.emitExpressionStatement($1);}
    ;

compound_statement
    : if_statement
    | while_statement
    | def_statement
    ;

if_statement
    : IF expr ':' suite
      %prec INDENT
      { builder.emitIfStatement($2, $4); }
    ;

while_statement
    : WHILE expr ':' suite
      %prec INDENT
      { builder.emitWhileStatement($2, $4); }
    ;

def_statement
    : DEF IDENTIFIER '(' ')' ':' suite
      %prec INDENT
      { builder.emitDefStatement($2, $6); }
    ;

expr
    : INTEGER { $$ = builder.emitIntegerLiteral($1); }
    | IDENTIFIER { $$ = builder.emitIdentifier($1); }
    | STRING_LITERAL { $$ = builder.emitStringLiteral($1); }
    | '(' expr ')' { $$ = $2; }
    | expr '+' expr { $$ = builder.emitBinaryOp('+', $1, $3); }
    | expr '-' expr { $$ = builder.emitBinaryOp('-', $1, $3); }
    | expr '*' expr { $$ = builder.emitBinaryOp('*', $1, $3); }
    | expr '/' expr { $$ = builder.emitBinaryOp('/', $1, $3); }
    ;

suite
    : NEWLINE INDENT statements DEDENT
      { $$ = builder.emitBlock($3); }
    ;

%%

namespace PythonParser
{
  // Report an error to the user.
  auto PythonParser::error (const std::string& msg) -> void
  {
    std::cerr << msg << '\n';
  }
}
