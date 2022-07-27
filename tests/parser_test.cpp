#include "parser.h"

#include <gtest/gtest.h>

#include "common.h"

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
                         ast::ExpressionStatement::make(
                             ast::BooleanLiteral::makeTrue())})),
      ParserTestData("VarDeclaration", "var test=true;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::VarDeclaration::make(
                             Token(TokenType::TOKEN_IDENTIFIER, "test"),
                             ast::BooleanLiteral::makeTrue())})),
      ParserTestData("FunctionDeclaration No Args", "fun test() { true; }",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::FunctionDeclaration::make(
                             Token(TokenType::TOKEN_IDENTIFIER, "test"),
                             std::vector<Token>{},
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::BooleanLiteral::makeTrue())}))})),
      ParserTestData(
          "FunctionDeclaration Multiple Args", "fun test(arg1, arg2) { true; }",
          ast::Program::make(
              std::vector<ast::StatementPtr>{ast::FunctionDeclaration::make(
                  Token(TokenType::TOKEN_IDENTIFIER, "test"),
                  std::vector<Token>{
                      Token(TokenType::TOKEN_IDENTIFIER, "arg1"),
                      Token(TokenType::TOKEN_IDENTIFIER, "arg2")},
                  ast::Block::make(std::vector<ast::StatementPtr>{
                      ast::ExpressionStatement::make(
                          ast::BooleanLiteral::makeTrue())}))})),
      ParserTestData("IfStatement", "if(true){true;}",
                     ast::Program::make(
                         std::vector<ast::StatementPtr>{ast::IfStatement::make(
                             ast::BooleanLiteral::makeTrue(),
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::BooleanLiteral::makeTrue())}))})),
      ParserTestData("IfElseStatement", "if(true){true;}else{false;}",
                     ast::Program::make(
                         std::vector<ast::StatementPtr>{ast::IfStatement::make(
                             ast::BooleanLiteral::makeTrue(),
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::BooleanLiteral::makeTrue())}),
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::BooleanLiteral::makeFalse())}))})),
      ParserTestData("WhileStatement", "while(true){true;}",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::WhileStatement::make(
                             ast::BooleanLiteral::makeTrue(),
                             ast::Block::make(std::vector<ast::StatementPtr>{
                                 ast::ExpressionStatement::make(
                                     ast::BooleanLiteral::makeTrue())}))})),
      ParserTestData(
          "PrintStatement", "print true;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::PrintStatement::make(ast::BooleanLiteral::makeTrue())})),
      ParserTestData(
          "ReturnStatement", "return true;",
          ast::Program::make(std::vector<ast::StatementPtr>{
              ast::ReturnStatement::make(ast::BooleanLiteral::makeTrue())})),
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
      ParserTestData("AssignmentExpression", "a=true;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::Assignment::make(
                             "a", ast::BooleanLiteral::makeTrue()))})),
  };

  for (const auto &testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    const auto actualProgram = parser.parse();
    ASSERT_TRUE(actualProgram->isEqual(*testCase.program)) << testCase.name;
  }
}

TEST_F(ParserTest, UnaryExprAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData("Negation", "!true;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::UnaryExpr::make(
                             Token(TokenType::TOKEN_BANG, "!"),
                             ast::BooleanLiteral::makeTrue()))})),
      ParserTestData("Negative", "-1;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::UnaryExpr::make(
                             Token(TokenType::TOKEN_MINUS, "-"),
                             ast::IntegerLiteral::make(1)))})),
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
      ParserTestData("Equal", "true==false;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::BooleanLiteral::makeTrue(),
                             Token(TokenType::TOKEN_EQUAL_EQUAL, "=="),
                             ast::BooleanLiteral::makeFalse()))})),
      ParserTestData("EqualRecursive", "true==false==true;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::BinaryExpr::make(
                                 ast::BooleanLiteral::makeTrue(),
                                 Token(TokenType::TOKEN_EQUAL_EQUAL, "=="),
                                 ast::BooleanLiteral::makeFalse()),
                             Token(TokenType::TOKEN_EQUAL_EQUAL, "=="),
                             ast::BooleanLiteral::makeTrue()))})),
      ParserTestData("NotEqual", "true!=false;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::BooleanLiteral::makeTrue(),
                             Token(TokenType::TOKEN_BANG_EQUAL, "!="),
                             ast::BooleanLiteral::makeFalse()))})),
      ParserTestData("Less", "true<false;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::BooleanLiteral::makeTrue(),
                             Token(TokenType::TOKEN_LESS, "<"),
                             ast::BooleanLiteral::makeFalse()))})),
      ParserTestData("LessEqual", "true<=false;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::BooleanLiteral::makeTrue(),
                             Token(TokenType::TOKEN_LESS_EQUAL, "<="),
                             ast::BooleanLiteral::makeFalse()))})),
      ParserTestData("LessEqualRecursive", "true<=false<=true;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::BinaryExpr::make(
                                 ast::BooleanLiteral::makeTrue(),
                                 Token(TokenType::TOKEN_LESS_EQUAL, "<="),
                                 ast::BooleanLiteral::makeFalse()),
                             Token(TokenType::TOKEN_LESS_EQUAL, "<="),
                             ast::BooleanLiteral::makeTrue()))})),
      ParserTestData("Greater", "true>false;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::BooleanLiteral::makeTrue(),
                             Token(TokenType::TOKEN_GREATER, ">"),
                             ast::BooleanLiteral::makeFalse()))})),
      ParserTestData("GreaterEqual", "true>=false;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::BooleanLiteral::makeTrue(),
                             Token(TokenType::TOKEN_GREATER_EQUAL, ">="),
                             ast::BooleanLiteral::makeFalse()))})),
      ParserTestData("Plus", "1+2;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::IntegerLiteral::make(1),
                             Token(TokenType::TOKEN_PLUS, "+"),
                             ast::IntegerLiteral::make(2)))})),
      ParserTestData("Minus", "1-2;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::IntegerLiteral::make(1),
                             Token(TokenType::TOKEN_MINUS, "-"),
                             ast::IntegerLiteral::make(2)))})),
      ParserTestData("Multiply", "1*2;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::IntegerLiteral::make(1),
                             Token(TokenType::TOKEN_STAR, "*"),
                             ast::IntegerLiteral::make(2)))})),
      ParserTestData("Division", "1/2;",
                     ast::Program::make(std::vector<ast::StatementPtr>{
                         ast::ExpressionStatement::make(ast::BinaryExpr::make(
                             ast::IntegerLiteral::make(1),
                             Token(TokenType::TOKEN_SLASH, "/"),
                             ast::IntegerLiteral::make(2)))})),
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
                                     ast::BooleanLiteral::makeTrue())}))})),
  };

  for (const auto &testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    const auto actualProgram = parser.parse();
    ASSERT_TRUE(actualProgram->isEqual(*testCase.program)) << testCase.name;
  }
}