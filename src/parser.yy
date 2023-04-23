%{
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "common.h"
#include "ast.h"
#include "scanner.h"
#include "lexer.h"
#include "astbuilder.h"

using namespace std;
using namespace cpplox::ast;

%}

%language "c++"
%require "3.8"
%define api.namespace {Parser}
%define parse.error verbose
%define api.parser.class {JSParser}
%define api.value.type variant

%code requires {
    #include "ast.h"
    #include "common.h"
    
    class JSLexer;
    class ASTBuilder {
    public:
        virtual ~ASTBuilder() = default;
        
        virtual cpplox::ast::ProgramPtr emitProgram(const std::vector<cpplox::ast::StatementPtr> &statements) = 0;
        virtual cpplox::ast::VarDeclarationPtr emitVarDeclaration(cpplox::ast::VariableExprPtr identifier, cpplox::ast::ExpressionPtr initializer = nullptr) = 0;
        virtual cpplox::ast::ExpressionStatementPtr emitExpressionStatement(cpplox::ast::ExpressionPtr expr) = 0;
        virtual cpplox::ast::IntegerLiteralPtr emitIntegerLiteral(const Token &value) = 0;
        virtual cpplox::ast::StringLiteralPtr emitStringLiteral(const Token &value) = 0;
        virtual cpplox::ast::BooleanLiteralPtr emitBooleanLiteral(bool value) = 0;
        virtual cpplox::ast::NilLiteralPtr emitNilLiteral() = 0;
        virtual cpplox::ast::VariableExprPtr emitVarExpression(const Token &value) = 0;
        virtual cpplox::ast::BinaryExprPtr emitBinaryOp(TokenType op, cpplox::ast::ExpressionPtr lhs, cpplox::ast::ExpressionPtr rhs) = 0;
        virtual cpplox::ast::StatementPtr emitEmptyStatement() = 0;
        virtual cpplox::ast::IfStatementPtr emitIfStatement(cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr thenBody, cpplox::ast::BlockPtr elseBody = nullptr) = 0;
        virtual cpplox::ast::WhileStatementPtr emitWhileStatement(cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr body) = 0;
        virtual cpplox::ast::FunctionDeclarationPtr emitDefStatement(cpplox::ast::VariableExprPtr name, const std::vector<cpplox::ast::VariableExprPtr>& arguments, cpplox::ast::BlockPtr body) = 0;
        virtual cpplox::ast::BlockPtr emitBlock(const std::vector<cpplox::ast::StatementPtr> &statements) = 0;
    };
}

%param {ASTBuilder& builder} {JSLexer& lexer}

%token<Token> INTEGER "INTEGER"
%token<Token> IDENTIFIER "IDENTIFIER"
%token<Token> STRING_LITERAL "STRING_LITERAL"

%token IF "if"
%token ELSE "else"
%token WHILE "while"
%token DEF "def"
%token VAR "var"
%token NIL "null"
%token TRUE "true"
%token FALSE "false"
%token PLUS "+"
%token MINUS "-"
%token STAR "*"
%token SLASH "/"
%token LPAREN "("
%token RPAREN ")"
%token LBRACE "{"
%token RBRACE "}"
%token LBRACKET "["
%token RBRACKET "]"
%token COMMA ","
%token DOT "."
%token BANG "!"
%token BANG_EQUAL "!="
%token EQUAL "="
%token EQUAL_EQUAL "=="
%token GREATER ">"
%token GREATER_EQUAL ">="
%token LESS "<"
%token LESS_EQUAL "<="
%token SEMICOLON ";"
%token COLON ":"

%type<cpplox::ast::ProgramPtr> program
%type<cpplox::ast::VariableExprPtr> varExpr
%type<cpplox::ast::ExpressionPtr> expr
%type<cpplox::ast::BlockPtr> suite
%type<cpplox::ast::StatementPtr> statement
%type<cpplox::ast::ExpressionStatementPtr> simple_statement
%type<std::vector<cpplox::ast::StatementPtr>> statements
%type<cpplox::ast::StatementPtr> compound_statement
%type<cpplox::ast::VarDeclarationPtr> var_declaration
%type<cpplox::ast::IfStatementPtr> if_statement
%type<cpplox::ast::WhileStatementPtr> while_statement
%type<cpplox::ast::FunctionDeclarationPtr> def_statement
%type<std::vector<cpplox::ast::VariableExprPtr>> arguments

%left PLUS MINUS
%left STAR SLASH
%left BANG_EQUAL EQUAL_EQUAL GREATER GREATER_EQUAL LESS LESS_EQUAL

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
    : compound_statement
    | simple_statement { $$ = $1; }
    | SEMICOLON { $$ = builder.emitEmptyStatement(); }
    ;

simple_statement
    : expr SEMICOLON { $$ = builder.emitExpressionStatement($1); }
    ;

compound_statement
    : var_declaration { $$ = $1; }
    | if_statement { $$ = $1; }
    | while_statement { $$ = $1; }
    | def_statement { $$ = $1; }
    ;

if_statement
    : IF LPAREN expr RPAREN suite ELSE suite
      { $$ = builder.emitIfStatement($3, $5, $7); }
    | IF LPAREN expr RPAREN suite
      { $$ = builder.emitIfStatement($3, $5); }

while_statement
    : WHILE LPAREN expr RPAREN suite
      { $$ = builder.emitWhileStatement($3, $5); }
    ;

def_statement
    : DEF varExpr LPAREN arguments RPAREN suite
      { $$ = builder.emitDefStatement($2, $4, $6); }
    ;

var_declaration
    : VAR varExpr EQUAL expr SEMICOLON { $$ = builder.emitVarDeclaration($2, $4); }
    | VAR varExpr SEMICOLON { $$ = builder.emitVarDeclaration($2); }

arguments
    : /* empty */ { $$ = std::vector<cpplox::ast::VariableExprPtr>(); }
    | arguments COMMA varExpr
      { $1.push_back($3); $$ = $1; }

varExpr
    : IDENTIFIER { $$ = builder.emitVarExpression($1); }
    ;

expr
    : LPAREN expr RPAREN { $$ = $2; }
    | expr PLUS expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_PLUS, $1, $3); }
    | expr MINUS expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_MINUS, $1, $3); }
    | expr STAR expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_STAR, $1, $3); }
    | expr SLASH expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_SLASH, $1, $3); }
    | expr EQUAL_EQUAL expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_EQUAL_EQUAL, $1, $3); }
    | expr BANG_EQUAL expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_BANG_EQUAL, $1, $3); }
    | expr GREATER expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_GREATER, $1, $3); }
    | expr GREATER_EQUAL expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_GREATER_EQUAL, $1, $3); }
    | expr LESS expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_LESS, $1, $3); }
    | expr LESS_EQUAL expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_LESS_EQUAL, $1, $3); }
    | INTEGER { $$ = builder.emitIntegerLiteral($1); }
    | STRING_LITERAL { $$ = builder.emitStringLiteral($1); }
    | NIL { $$ = builder.emitNilLiteral(); }
    | TRUE { $$ = builder.emitBooleanLiteral(true); }
    | FALSE { $$ = builder.emitBooleanLiteral(false); }
    | varExpr { $$ = $1; }
    ;

suite
    : LBRACE statements RBRACE { $$ = builder.emitBlock($2); }
    | statement { $$ = builder.emitBlock({ $1 }); }
    ;

%%

namespace Parser
{
  // Report an error to the user.
  auto JSParser::error (const std::string& msg) -> void
  {
    std::cerr << msg << '\n';
  }
}
