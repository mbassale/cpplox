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
      ParserTestData("ExpressionStatement", "true;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         AS_STATEMENT(ast::ExpressionStatement::make(
                             ast::Literal::make(Token(TOKEN_TRUE, "true"))))})),
      ParserTestData(
          "ForStatement", "for(;;){true;}",
          ast::Program::make(std::vector<ast::StatementPtr>{
              AS_STATEMENT(ast::ForStatement::make(
                  nullptr, nullptr, nullptr,
                  AS_STATEMENT(ast::ExpressionStatement::make(
                      ast::Literal::make(Token(TOKEN_TRUE, "true"))))))}))};

  for (const auto &testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    const auto actualProgram = parser.parse();
    ASSERT_TRUE(actualProgram->isEqual(*testCase.program));
  }
}