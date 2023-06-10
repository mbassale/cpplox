%{
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "common.h"
#include "ast.h"
#include "token.h"
#include "lexer.h"
#include "astbuilder.h"

using namespace std;

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
        
        virtual ProgramPtr emitProgram(const std::vector<StatementPtr> &statements) = 0;
        virtual VarDeclarationPtr emitVarDeclaration(VariableExprPtr identifier, ExpressionPtr initializer = nullptr) = 0;
        virtual MemberExprPtr emitMemberExpression(VariableExprPtr object, const Token &member) = 0;
        virtual ClassDeclarationPtr emitClassDeclaration(const Token& name, const std::vector<StatementPtr> &definitions = {}) = 0;
        virtual ExpressionStatementPtr emitExpressionStatement(ExpressionPtr expr) = 0;
        virtual IntegerLiteralPtr emitIntegerLiteral(const Token &value) = 0;
        virtual StringLiteralPtr emitStringLiteral(const Token &value) = 0;
        virtual BooleanLiteralPtr emitBooleanLiteral(bool value) = 0;
        virtual NilLiteralPtr emitNilLiteral() = 0;
        virtual ArrayLiteralPtr emitArrayLiteral(const std::vector<ExpressionPtr> &elements) = 0;
        virtual ArraySubscriptExprPtr emitArraySubscript(ExpressionPtr array, ExpressionPtr index) = 0;
        virtual VariableExprPtr emitVarExpression(const Token &value) = 0;
        virtual AssignmentPtr emitAssignmentExpression(ExpressionPtr lhs, ExpressionPtr rhs) = 0;
        virtual CallExprPtr emitCallExpression(ExpressionPtr callee, const std::vector<ExpressionPtr> &arguments) = 0;
        virtual UnaryExprPtr emitUnaryOp(TokenType op, ExpressionPtr rhs) = 0;
        virtual BinaryExprPtr emitBinaryOp(TokenType op, ExpressionPtr lhs, ExpressionPtr rhs) = 0;
        virtual StatementPtr emitEmptyStatement() = 0;
        virtual IfStatementPtr emitIfStatement(ExpressionPtr condition, BlockPtr thenBody, BlockPtr elseBody = nullptr) = 0;
        virtual WhileStatementPtr emitWhileStatement(ExpressionPtr condition, BlockPtr body) = 0;
        virtual ForStatementPtr emitForStatement(StatementPtr initialization, ExpressionStatementPtr condition, ExpressionStatementPtr increment, BlockPtr body) = 0;
        virtual FunctionDeclarationPtr emitDefStatement(VariableExprPtr name, const std::vector<VariableExprPtr>& arguments, BlockPtr body) = 0;
        virtual PrintStatementPtr emitPrintStatement(ExpressionPtr expr) = 0;
        virtual ReturnStatementPtr emitReturnStatement(ExpressionPtr expr = nullptr) = 0;
        virtual BreakStatementPtr emitBreakStatement() = 0;
        virtual ContinueStatementPtr emitContinueStatement() = 0;
        virtual BlockPtr emitBlock(const std::vector<StatementPtr> &statements) = 0;
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
%token CONTINUE "continue"
%token CLASS "class"
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

%type<ProgramPtr> program
%type<VariableExprPtr> varExpr
%type<MemberExprPtr> member_expr
%type<AssignmentPtr> assignment_expr
%type<CallExprPtr> call_expr
%type<std::vector<ExpressionPtr>> call_arguments
%type<ExpressionPtr> expr
%type<BlockPtr> suite
%type<StatementPtr> statement
%type<StatementPtr> simple_statement
%type<std::vector<StatementPtr>> statements
%type<StatementPtr> compound_statement
%type<VarDeclarationPtr> var_declaration
%type<ClassDeclarationPtr> class_declaration
%type<std::vector<StatementPtr>> class_body
%type<IfStatementPtr> if_statement
%type<WhileStatementPtr> while_statement
%type<ForStatementPtr> for_statement
%type<StatementPtr> for_initialization
%type<ExpressionStatementPtr> for_condition
%type<ExpressionStatementPtr> for_increment
%type<ExpressionStatementPtr> expr_statement
%type<FunctionDeclarationPtr> def_statement
%type<std::vector<VariableExprPtr>> function_parameters
%type<PrintStatementPtr> print_statement
%type<ReturnStatementPtr> return_statement
%type<BreakStatementPtr> break_statement
%type<ContinueStatementPtr> continue_statement
%type<ArrayLiteralPtr> array_literal
%type<std::vector<ExpressionPtr>> array_elements
%type<ArraySubscriptExprPtr> array_subscript
%type<UnaryExprPtr> unary_expr

%left PLUS MINUS
%left STAR SLASH
%left BANG_EQUAL EQUAL_EQUAL GREATER GREATER_EQUAL LESS LESS_EQUAL
%left AND OR
%nonassoc UMINUS

%start program

%%

program
    : statements { $$ = builder.emitProgram($1); }
    ;

statements
    : /* empty */ { $$ = std::vector<StatementPtr>(); }
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
    : class_declaration { $$ = $1; }
    | var_declaration { $$ = $1; }
    | if_statement { $$ = $1; }
    | while_statement { $$ = $1; }
    | for_statement { $$ = $1; }
    | def_statement { $$ = $1; }
    | print_statement { $$ = $1; }
    | return_statement { $$ = $1; }
    | break_statement { $$ = $1; }
    | continue_statement { $$ = $1; }
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
    : /* empty */ { $$ = std::vector<VariableExprPtr>(); }
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

continue_statement
    : CONTINUE SEMICOLON
      { $$ = builder.emitContinueStatement(); }

class_declaration
    : CLASS IDENTIFIER LBRACE class_body RBRACE
      { $$ = builder.emitClassDeclaration($2, $4); }

class_body
    : /* empty */ { $$ = std::vector<StatementPtr>(); }
    | class_body def_statement
      { $1.push_back($2); $$ = $1; }
    | class_body var_declaration
      { $1.push_back($2); $$ = $1; }
    ;

var_declaration
    : VAR varExpr EQUAL expr SEMICOLON { $$ = builder.emitVarDeclaration($2, $4); }
    | VAR varExpr SEMICOLON { $$ = builder.emitVarDeclaration($2); }

varExpr
    : IDENTIFIER { $$ = builder.emitVarExpression($1); }
    ;

member_expr
    : varExpr DOT IDENTIFIER { $$ = builder.emitMemberExpression($1, $3); }

assignment_expr 
    : varExpr EQUAL expr { $$ = builder.emitAssignmentExpression($1, $3); }
    | member_expr EQUAL expr { $$ = builder.emitAssignmentExpression($1, $3); }

call_expr
    : expr LPAREN call_arguments RPAREN { $$ = builder.emitCallExpression($1, $3); }

call_arguments
    : /* empty */ { $$ = std::vector<ExpressionPtr>(); }
    | call_arguments expr COMMA
      { $1.push_back($2); $$ = $1; }
    | call_arguments expr
      { $1.push_back($2); $$ = $1; }

array_literal
    : LBRACKET array_elements RBRACKET { $$ = builder.emitArrayLiteral($2); }

array_elements
    : /* empty */ { $$ = std::vector<ExpressionPtr>(); }
    | array_elements expr COMMA
      { $1.push_back($2); $$ = $1; }
    | array_elements expr
      { $1.push_back($2); $$ = $1; }

array_subscript
    : expr LBRACKET expr RBRACKET { $$ = builder.emitArraySubscript($1, $3); }

unary_expr
    : BANG expr { $$ = builder.emitUnaryOp(TokenType::TOKEN_BANG, $2); }
    | MINUS expr %prec UMINUS { $$ = builder.emitUnaryOp(TokenType::TOKEN_MINUS, $2); }

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
    | INTEGER { $$ = builder.emitIntegerLiteral($1); }
    | STRING_LITERAL { $$ = builder.emitStringLiteral($1); }
    | NIL { $$ = builder.emitNilLiteral(); }
    | TRUE { $$ = builder.emitBooleanLiteral(true); }
    | FALSE { $$ = builder.emitBooleanLiteral(false); }
    | member_expr { $$ = $1; }
    | varExpr { $$ = $1; }
    | expr AND expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_AND, $1, $3); }
    | expr OR expr { $$ = builder.emitBinaryOp(TokenType::TOKEN_OR, $1, $3); }
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
