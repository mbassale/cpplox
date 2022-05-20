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