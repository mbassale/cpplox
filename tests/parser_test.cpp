#include "parser.h"

#include <gtest/gtest.h>

#include "common.h"
#include "debug.h"

using namespace cpplox;

struct ParserTestData {
  const char *name;
  const char *source;
  ast::ProgramPtr program;

  ParserTestData(const char *name, const char *source, ast::ProgramPtr program)
      : name(name), source(source), program(program) {}
};

class ParserTest : public ::testing::Test {
 protected:
  void compareAsts(ast::NodePtr left, ast::NodePtr right);
};

void ParserTest::compareAsts(ast::NodePtr left, ast::NodePtr right) {}

TEST_F(ParserTest, ParserAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData("MinimalStatement", ";",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::Statement::make()})),
      ParserTestData(
          "ExpressionStatement", "true;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ExpressionStatement::make(ast::Literal::makeTrue())})),
      ParserTestData(
          "VarDeclaration", "var test=true;",
          ast::Program::make(
              std::vector<ast::StatementPtr>{ast::VarDeclaration::make(
                  Token(TOKEN_IDENTIFIER, "test"), ast::Literal::makeTrue())})),
      ParserTestData("IfStatement", "if(true){true;}",
                     ast::Program::make(
                         std::vector<ast::StatementPtr>{ast::IfStatement::make(
                             ast::Literal::makeTrue(),
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::Literal::makeTrue())}))})),
      ParserTestData("IfElseStatement", "if(true){true;}else{false;}",
                     ast::Program::make(
                         std::vector<ast::StatementPtr>{ast::IfStatement::make(
                             ast::Literal::makeTrue(),
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::Literal::makeTrue())}),
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::Literal::makeFalse())}))})),
      ParserTestData("WhileStatement", "while(true){true;}",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::WhileStatement::make(
                             ast::Literal::makeTrue(),
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::Literal::makeTrue())}))})),
      ParserTestData("PrintStatement", "print true;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::PrintStatement::make(ast::Literal::makeTrue())})),
      ParserTestData(
          "ReturnStatement", "return true;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ReturnStatement::make(ast::Literal::makeTrue())})),
      ParserTestData("ReturnStatement", "return;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ReturnStatement::make()}))};

  for (const auto &testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    const auto actualProgram = parser.parse();
    ASSERT_TRUE(actualProgram->isEqual(*testCase.program)) << testCase.name;
  }
}

TEST_F(ParserTest, AssignmentExprAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "AssignmentExpression", "a=true;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ExpressionStatement::make(ast::Assignment::make(
                  ast::VariableExpr::make("a"), ast::Literal::makeTrue()))})),
  };

  for (const auto &testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    const auto actualProgram = parser.parse();
    ASSERT_TRUE(actualProgram->isEqual(*testCase.program)) << testCase.name;
  }
}

TEST_F(ParserTest, BinaryExprAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "Equal", "true==false;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ExpressionStatement::make(ast::BinaryExpr::make(
                  ast::Literal::makeTrue(), Token(TOKEN_EQUAL_EQUAL, "=="),
                  ast::Literal::makeFalse()))})),
      ParserTestData(
          "EqualRecursive", "true==false==true;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ExpressionStatement::make(ast::BinaryExpr::make(
                  ast::BinaryExpr::make(ast::Literal::makeTrue(),
                                        Token(TOKEN_EQUAL_EQUAL, "=="),
                                        ast::Literal::makeFalse()),
                  Token(TOKEN_EQUAL_EQUAL, "=="), ast::Literal::makeTrue()))})),
      ParserTestData(
          "NotEqual", "true!=false;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ExpressionStatement::make(ast::BinaryExpr::make(
                  ast::Literal::makeTrue(), Token(TOKEN_BANG_EQUAL, "!="),
                  ast::Literal::makeFalse()))})),
      ParserTestData("Less", "true<false;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::Literal::makeTrue(), Token(TOKEN_LESS, "<"),
                             ast::Literal::makeFalse()))})),
      ParserTestData(
          "LessEqual", "true<=false;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ExpressionStatement::make(ast::BinaryExpr::make(
                  ast::Literal::makeTrue(), Token(TOKEN_LESS_EQUAL, "<="),
                  ast::Literal::makeFalse()))})),
      ParserTestData(
          "LessEqualRecursive", "true<=false<=true;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ExpressionStatement::make(ast::BinaryExpr::make(
                  ast::BinaryExpr::make(ast::Literal::makeTrue(),
                                        Token(TOKEN_LESS_EQUAL, "<="),
                                        ast::Literal::makeFalse()),
                  Token(TOKEN_LESS_EQUAL, "<="), ast::Literal::makeTrue()))})),
      ParserTestData(
          "Greater",
          "true>false;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ExpressionStatement::make(ast::BinaryExpr::make(
                  ast::Literal::makeTrue(), Token(TOKEN_GREATER, ">"),
                  ast::Literal::makeFalse()))})),
      ParserTestData(
          "GreaterEqual", "true>=false;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ExpressionStatement::make(ast::BinaryExpr::make(
                  ast::Literal::makeTrue(), Token(TOKEN_GREATER_EQUAL, ">="),
                  ast::Literal::makeFalse()))})),
  };

  for (const auto &testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    const auto actualProgram = parser.parse();
    ASSERT_TRUE(actualProgram->isEqual(*testCase.program)) << testCase.name;
  }
}

TEST_F(ParserTest, ForStatementAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData("MinimalForStatement", "for(;;);",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ForStatement::make(nullptr, nullptr, nullptr,
                                                 ast::Statement::make())})),
      ParserTestData("MinimalForStatementWithBlock", "for(;;){true;}",
                     ast::Program::make(
                         std::vector<ast::StatementPtr>{ast::ForStatement::make(
                             nullptr, nullptr, nullptr,
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::Literal::makeTrue())}))})),
  };

  for (const auto &testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    const auto actualProgram = parser.parse();
    ASSERT_TRUE(actualProgram->isEqual(*testCase.program)) << testCase.name;
  }
}