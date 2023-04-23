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
        virtual cpplox::ast::ArrayLiteralPtr emitArrayLiteral(const std::vector<cpplox::ast::ExpressionPtr> &elements) = 0;
        virtual cpplox::ast::ArraySubscriptExprPtr emitArraySubscript(cpplox::ast::ExpressionPtr array, cpplox::ast::ExpressionPtr index) = 0;
        virtual cpplox::ast::VariableExprPtr emitVarExpression(const Token &value) = 0;
        virtual cpplox::ast::AssignmentPtr emitAssignmentExpression(cpplox::ast::VariableExprPtr identifier, cpplox::ast::ExpressionPtr value) = 0;
        virtual cpplox::ast::CallExprPtr emitCallExpression(cpplox::ast::ExpressionPtr callee, const std::vector<cpplox::ast::ExpressionPtr> &arguments) = 0;
        virtual cpplox::ast::UnaryExprPtr emitUnaryOp(TokenType op, cpplox::ast::ExpressionPtr rhs) = 0;
        virtual cpplox::ast::BinaryExprPtr emitBinaryOp(TokenType op, cpplox::ast::ExpressionPtr lhs, cpplox::ast::ExpressionPtr rhs) = 0;
        virtual cpplox::ast::StatementPtr emitEmptyStatement() = 0;
        virtual cpplox::ast::IfStatementPtr emitIfStatement(cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr thenBody, cpplox::ast::BlockPtr elseBody = nullptr) = 0;
        virtual cpplox::ast::WhileStatementPtr emitWhileStatement(cpplox::ast::ExpressionPtr condition, cpplox::ast::BlockPtr body) = 0;
        virtual cpplox::ast::ForStatementPtr emitForStatement(cpplox::ast::StatementPtr initialization, cpplox::ast::ExpressionStatementPtr condition, cpplox::ast::ExpressionStatementPtr increment, cpplox::ast::BlockPtr body) = 0;
        virtual cpplox::ast::FunctionDeclarationPtr emitDefStatement(cpplox::ast::VariableExprPtr name, const std::vector<cpplox::ast::VariableExprPtr>& arguments, cpplox::ast::BlockPtr body) = 0;
        virtual cpplox::ast::PrintStatementPtr emitPrintStatement(cpplox::ast::ExpressionPtr expr) = 0;
        virtual cpplox::ast::ReturnStatementPtr emitReturnStatement(cpplox::ast::ExpressionPtr expr = nullptr) = 0;
        virtual cpplox::ast::BreakStatementPtr emitBreakStatement() = 0;
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
%token FOR "for"
%token AND "and"
%token OR "or"
%token DEF "def"
%token VAR "var"
%token NIL "null"
%token TRUE "true"
%token FALSE "false"
%token PRINT "print"
%token RETURN "return"
%token BREAK "break"
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
%type<cpplox::ast::AssignmentPtr> assignment_expr
%type<cpplox::ast::CallExprPtr> call_expr
%type<std::vector<cpplox::ast::ExpressionPtr>> call_arguments
%type<cpplox::ast::ExpressionPtr> expr
%type<cpplox::ast::BlockPtr> suite
%type<cpplox::ast::StatementPtr> statement
%type<cpplox::ast::StatementPtr> simple_statement
%type<std::vector<cpplox::ast::StatementPtr>> statements
%type<cpplox::ast::StatementPtr> compound_statement
%type<cpplox::ast::VarDeclarationPtr> var_declaration
%type<cpplox::ast::IfStatementPtr> if_statement
%type<cpplox::ast::WhileStatementPtr> while_statement
%type<cpplox::ast::ForStatementPtr> for_statement
%type<cpplox::ast::StatementPtr> for_initialization
%type<cpplox::ast::ExpressionStatementPtr> for_condition
%type<cpplox::ast::ExpressionStatementPtr> for_increment
%type<cpplox::ast::ExpressionStatementPtr> expr_statement
%type<cpplox::ast::FunctionDeclarationPtr> def_statement
%type<std::vector<cpplox::ast::VariableExprPtr>> function_parameters
%type<cpplox::ast::PrintStatementPtr> print_statement
%type<cpplox::ast::ReturnStatementPtr> return_statement
%type<cpplox::ast::BreakStatementPtr> break_statement
%type<cpplox::ast::ArrayLiteralPtr> array_literal
%type<std::vector<cpplox::ast::ExpressionPtr>> array_elements
%type<cpplox::ast::ArraySubscriptExprPtr> array_subscript
%type<cpplox::ast::UnaryExprPtr> unary_expr

%left PLUS MINUS
%left STAR SLASH
%left BANG_EQUAL EQUAL_EQUAL GREATER GREATER_EQUAL LESS LESS_EQUAL
%left AND OR

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
    ;

simple_statement
    : expr_statement { $$ = $1; }
    | SEMICOLON { $$ = builder.emitEmptyStatement(); }
    ;

expr_statement
    : expr SEMICOLON { $$ = builder.emitExpressionStatement($1); }
    ;

compound_statement
    : var_declaration { $$ = $1; }
    | if_statement { $$ = $1; }
    | while_statement { $$ = $1; }
    | for_statement { $$ = $1; }
    | def_statement { $$ = $1; }
    | print_statement { $$ = $1; }
    | return_statement { $$ = $1; }
    | break_statement { $$ = $1; }
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

for_statement
    : FOR LPAREN for_initialization for_condition for_increment RPAREN suite
      { $$ = builder.emitForStatement($3, $4, $5, $7); }

for_initialization
    : var_declaration { $$ = $1; }
    | simple_statement { $$ = $1; }

for_condition
    : expr SEMICOLON { $$ = builder.emitExpressionStatement($1); }
    | SEMICOLON { $$ = builder.emitExpressionStatement(builder.emitBooleanLiteral(true)); }

for_increment
    : expr { $$ = builder.emitExpressionStatement($1); }
    | /* empty */ { $$ = builder.emitExpressionStatement(builder.emitNilLiteral()); }

def_statement
    : DEF varExpr LPAREN function_parameters RPAREN suite
      { $$ = builder.emitDefStatement($2, $4, $6); }
    ;

function_parameters
    : /* empty */ { $$ = std::vector<cpplox::ast::VariableExprPtr>(); }
    | function_parameters varExpr COMMA
      { $1.push_back($2); $$ = $1; }
    | function_parameters varExpr
      { $1.push_back($2); $$ = $1; }

print_statement
    : PRINT LPAREN expr RPAREN SEMICOLON
      { $$ = builder.emitPrintStatement($3); }
    | PRINT expr SEMICOLON
      { $$ = builder.emitPrintStatement($2); }

return_statement
    : RETURN expr SEMICOLON
      { $$ = builder.emitReturnStatement($2); }
    | RETURN SEMICOLON
      { $$ = builder.emitReturnStatement(); }
  
break_statement
    : BREAK SEMICOLON
      { $$ = builder.emitBreakStatement(); }

var_declaration
    : VAR varExpr EQUAL expr SEMICOLON { $$ = builder.emitVarDeclaration($2, $4); }
    | VAR varExpr SEMICOLON { $$ = builder.emitVarDeclaration($2); }

varExpr
    : IDENTIFIER { $$ = builder.emitVarExpression($1); }
    ;

assignment_expr 
    : varExpr EQUAL expr { $$ = builder.emitAssignmentExpression($1, $3); }

call_expr
    : expr LPAREN call_arguments RPAREN { $$ = builder.emitCallExpression($1, $3); }

call_arguments
    : /* empty */ { $$ = std::vector<cpplox::ast::ExpressionPtr>(); }
    | call_arguments expr COMMA
      { $1.push_back($2); $$ = $1; }
    | call_arguments expr
      { $1.push_back($2); $$ = $1; }

array_literal
    : LBRACKET array_elements RBRACKET { $$ = builder.emitArrayLiteral($2); }

array_elements
    : /* empty */ { $$ = std::vector<cpplox::ast::ExpressionPtr>(); }
    | array_elements expr COMMA
      { $1.push_back($2); $$ = $1; }
    | array_elements expr
      { $1.push_back($2); $$ = $1; }

array_subscript
    : expr LBRACKET expr RBRACKET { $$ = builder.emitArraySubscript($1, $3); }

unary_expr
    : BANG expr { $$ = builder.emitUnaryOp(TokenType::TOKEN_BANG, $2); }
    | MINUS expr { $$ = builder.emitUnaryOp(TokenType::TOKEN_MINUS, $2); }

expr
    : unary_expr { $$ = $1; }
    | assignment_expr { $$ = $1; }
    | call_expr { $$ = $1; }
    | array_subscript { $$ = $1; }
    | array_literal { $$ = $1; }
    | LPAREN expr RPAREN { $$ = $2; }
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
    | expr AND expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_AND, $1, $3); }
    | expr OR expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_OR, $1, $3); }
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
