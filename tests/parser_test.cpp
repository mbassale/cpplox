#include "parser.h"

#include <gtest/gtest.h>

#include "ast.h"
#include "astbuilder.h"
#include "common.h"
#include "lexer.h"

using Parser::JSParser;

struct ParserTestData {
  const char *name;
  const char *source;
  ProgramPtr program;

  ParserTestData(const char *name, const char *source, ProgramPtr program)
      : name(name), source(source), program(program) {}
};

class ParserTest : public ::testing::Test {
 protected:
  void assertTestCases(const std::vector<ParserTestData> &testCases) {
    for (const auto &testCase : testCases) {
      std::ostringstream os;
      os << testCase.source << std::endl;
      std::istringstream is(os.str());
      JSLexer lexer(&is);
      ASTBuilderImpl builder;
      JSParser parser(builder, lexer);
      parser.parse();
      auto actualProgram = builder.getProgram();
      ASSERT_NE(actualProgram, nullptr) << testCase.name << ": "
                                        << "actualProgram is null";
      ASSERT_TRUE(actualProgram->isEqual(*testCase.program))
          << testCase.name << ": " << actualProgram->toString()
          << " != " << testCase.program->toString();
    }
  }
};

TEST_F(ParserTest, LiteralExprAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData("NullLiteral", "null;",
                     Program::make(std::vector<StatementPtr>{
                         ExpressionStatement::make(NilLiteral::make())})),
      ParserTestData(
          "TrueLiteral", "true;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BooleanLiteral::makeTrue())})),
      ParserTestData(
          "FalseLiteral", "false;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BooleanLiteral::makeFalse())})),
      ParserTestData(
          "IntegerLiteral", "123;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(IntegerLiteral::make(123))})),
      ParserTestData(
          "StringLiteral", "\"test\";",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(StringLiteral::make("test"))})),
  };
  assertTestCases(testCases);
}

TEST_F(ParserTest, StatementAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "EmptyStatement", ";",
          Program::make(std::vector<StatementPtr>{Statement::make()})),
      ParserTestData(
          "ExpressionStatement", "true;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BooleanLiteral::makeTrue())})),
      ParserTestData(
          "VarDeclaration", "var test=true;",
          Program::make(std::vector<StatementPtr>{
              VarDeclaration::make("test", BooleanLiteral::makeTrue())})),
      ParserTestData(
          "IfStatement", "if(true){true;}",
          Program::make(std::vector<StatementPtr>{IfStatement::make(
              BooleanLiteral::makeTrue(),
              Block::make(std::vector<StatementPtr>{
                  ExpressionStatement::make(BooleanLiteral::makeTrue())}))})),
      ParserTestData(
          "IfElseStatement", "if(true){true;}else{false;}",
          Program::make(std::vector<StatementPtr>{IfStatement::make(
              BooleanLiteral::makeTrue(),
              Block::make(std::vector<StatementPtr>{
                  ExpressionStatement::make(BooleanLiteral::makeTrue())}),
              Block::make(std::vector<StatementPtr>{
                  ExpressionStatement::make(BooleanLiteral::makeFalse())}))})),
      ParserTestData(
          "WhileStatement", "while(true){true;}",
          Program::make(std::vector<StatementPtr>{WhileStatement::make(
              BooleanLiteral::makeTrue(),
              Block::make(std::vector<StatementPtr>{
                  ExpressionStatement::make(BooleanLiteral::makeTrue())}))})),
      ParserTestData("PrintStatement", "print true;",
                     Program::make(std::vector<StatementPtr>{
                         PrintStatement::make(BooleanLiteral::makeTrue())})),
      ParserTestData("ReturnStatement", "return true;",
                     Program::make(std::vector<StatementPtr>{
                         ReturnStatement::make(BooleanLiteral::makeTrue())})),
      ParserTestData(
          "ReturnStatement", "return;",
          Program::make(std::vector<StatementPtr>{ReturnStatement::make()}))};

  assertTestCases(testCases);
}

TEST_F(ParserTest, ParseFunctionDeclaration) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "FunctionDeclarationNoArgs", "def test() { return true; }",
          Program::make(std::vector<StatementPtr>{FunctionDeclaration::make(
              "test", {},
              Block::make(std::vector<StatementPtr>{
                  ReturnStatement::make(BooleanLiteral::makeTrue())}))})),
      ParserTestData(
          "FunctionDeclarationOneArg", "def test(arg1) { return true; }",
          Program::make(std::vector<StatementPtr>{FunctionDeclaration::make(
              "test", {"arg1"},
              Block::make(std::vector<StatementPtr>{
                  ReturnStatement::make(BooleanLiteral::makeTrue())}))})),
      ParserTestData(
          "FunctionDeclarationMultipleArgs",
          "def test(arg1, arg2) { return true; }",
          Program::make(std::vector<StatementPtr>{FunctionDeclaration::make(
              "test", {"arg1", "arg2"},
              Block::make(std::vector<StatementPtr>{
                  ReturnStatement::make(BooleanLiteral::makeTrue())}))})),
      ParserTestData(
          "FunctionDeclarationLocalVars",
          "def test(arg1) { var a = arg1; return a; }",
          Program::make(std::vector<StatementPtr>{FunctionDeclaration::make(
              "test", {"arg1"},
              Block::make(std::vector<StatementPtr>{
                  VarDeclaration::make("a", VariableExpr::make("arg1")),
                  ReturnStatement::make(VariableExpr::make("a"))}))})),
  };
  assertTestCases(testCases);
}

TEST_F(ParserTest, AssignmentExprAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "AssignmentExpression", "a=true;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              Assignment::make("a", BooleanLiteral::makeTrue()))})),
      ParserTestData(
          "AssignmentSimpleExpression", "a = a + 1;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(Assignment::make(
                  "a", BinaryExpr::make(VariableExpr::make("a"),
                                        Token::make(TokenType::TOKEN_PLUS),
                                        IntegerLiteral::make(1))))}))};

  assertTestCases(testCases);
}

TEST_F(ParserTest, UnaryExprAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "Negation", "!true;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              UnaryExpr::make(Token::make(TokenType::TOKEN_BANG),
                              BooleanLiteral::makeTrue()))})),
      ParserTestData(
          "Negative", "-1;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              UnaryExpr::make(Token::make(TokenType::TOKEN_MINUS),
                              IntegerLiteral::make(1)))})),
  };

  assertTestCases(testCases);
}

TEST_F(ParserTest, CallExprAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "MinimalCall", "test();",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(CallExpr::make(
                  VariableExpr::make("test"), std::vector<ExpressionPtr>{}))})),
      ParserTestData(
          "CallWithSimpleArgument", "test(arg1);",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(CallExpr::make(
                  VariableExpr::make("test"),
                  std::vector<ExpressionPtr>{VariableExpr::make("arg1")}))})),
      ParserTestData(
          "CallWithExpressionArgument", "test(2*arg1);",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              CallExpr::make(VariableExpr::make("test"),
                             std::vector<ExpressionPtr>{BinaryExpr::make(
                                 IntegerLiteral::make(2),
                                 Token::make(TokenType::TOKEN_STAR),
                                 VariableExpr::make("arg1"))}))})),
      ParserTestData(
          "CallWithTwoExpressionArguments", "test(arg1,2*arg2);",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(CallExpr::make(
                  VariableExpr::make("test"),
                  std::vector<ExpressionPtr>{
                      VariableExpr::make("arg1"),
                      BinaryExpr::make(IntegerLiteral::make(2),
                                       Token::make(TokenType::TOKEN_STAR),
                                       VariableExpr::make("arg2"))}))})),
      ParserTestData(
          "CallWithInnerCallArguments", "test(arg1,test2(arg2));",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(CallExpr::make(
                  VariableExpr::make("test"),
                  std::vector<ExpressionPtr>{
                      VariableExpr::make("arg1"),
                      CallExpr::make(VariableExpr::make("test2"),
                                     std::vector<ExpressionPtr>{
                                         VariableExpr::make("arg2")})}))}))};

  assertTestCases(testCases);
}

TEST_F(ParserTest, BinaryExprAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "And", "true and false;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BinaryExpr::make(
                  BooleanLiteral::makeTrue(), Token::make(TokenType::TOKEN_AND),
                  BooleanLiteral::makeFalse()))})),
      ParserTestData("MultipleAnd", "true and false and true;",
                     Program::make(std::vector<StatementPtr>{
                         ExpressionStatement::make(BinaryExpr::make(
                             BinaryExpr::make(BooleanLiteral::makeTrue(),
                                              Token::make(TokenType::TOKEN_AND),
                                              BooleanLiteral::makeFalse()),
                             Token::make(TokenType::TOKEN_AND),
                             BooleanLiteral::makeTrue()))})),
      ParserTestData(
          "Or", "true or false;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BinaryExpr::make(
                  BooleanLiteral::makeTrue(), Token::make(TokenType::TOKEN_OR),
                  BooleanLiteral::makeFalse()))})),
      ParserTestData("MultipleOr", "true or false or true;",
                     Program::make(std::vector<StatementPtr>{
                         ExpressionStatement::make(BinaryExpr::make(
                             BinaryExpr::make(BooleanLiteral::makeTrue(),
                                              Token::make(TokenType::TOKEN_OR),
                                              BooleanLiteral::makeFalse()),
                             Token::make(TokenType::TOKEN_OR),
                             BooleanLiteral::makeTrue()))})),
      ParserTestData(
          "MultipleAndOr", "true or false and true or true;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BinaryExpr::make(
                  BinaryExpr::make(
                      BinaryExpr::make(BooleanLiteral::makeTrue(),
                                       Token::make(TokenType::TOKEN_OR),
                                       BooleanLiteral::makeFalse()),
                      Token::make(TokenType::TOKEN_AND),
                      BooleanLiteral::makeTrue()),
                  Token::make(TokenType::TOKEN_OR),
                  BooleanLiteral::makeTrue()))})),
      ParserTestData(
          "Equal", "true==false;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              BinaryExpr::make(BooleanLiteral::makeTrue(),
                               Token::make(TokenType::TOKEN_EQUAL_EQUAL),
                               BooleanLiteral::makeFalse()))})),
      ParserTestData(
          "EqualRecursive", "true==false==true;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BinaryExpr::make(
                  BinaryExpr::make(BooleanLiteral::makeTrue(),
                                   Token::make(TokenType::TOKEN_EQUAL_EQUAL),
                                   BooleanLiteral::makeFalse()),
                  Token::make(TokenType::TOKEN_EQUAL_EQUAL),
                  BooleanLiteral::makeTrue()))})),
      ParserTestData(
          "NotEqual", "true!=false;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              BinaryExpr::make(BooleanLiteral::makeTrue(),
                               Token::make(TokenType::TOKEN_BANG_EQUAL),
                               BooleanLiteral::makeFalse()))})),
      ParserTestData(
          "Less", "true<false;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              BinaryExpr::make(BooleanLiteral::makeTrue(),
                               Token::make(TokenType::TOKEN_LESS),
                               BooleanLiteral::makeFalse()))})),
      ParserTestData(
          "LessEqual", "true<=false;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              BinaryExpr::make(BooleanLiteral::makeTrue(),
                               Token::make(TokenType::TOKEN_LESS_EQUAL),
                               BooleanLiteral::makeFalse()))})),
      ParserTestData(
          "LessEqualRecursive", "true<=false<=true;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BinaryExpr::make(
                  BinaryExpr::make(BooleanLiteral::makeTrue(),
                                   Token::make(TokenType::TOKEN_LESS_EQUAL),
                                   BooleanLiteral::makeFalse()),
                  Token::make(TokenType::TOKEN_LESS_EQUAL),
                  BooleanLiteral::makeTrue()))})),
      ParserTestData("Greater", "true>false;",
                     Program::make(std::vector<StatementPtr>{
                         ExpressionStatement::make(BinaryExpr::make(
                             BooleanLiteral::makeTrue(),
                             Token::make(TokenType::TOKEN_GREATER),
                             BooleanLiteral::makeFalse()))})),
      ParserTestData(
          "GreaterEqual", "true>=false;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              BinaryExpr::make(BooleanLiteral::makeTrue(),
                               Token::make(TokenType::TOKEN_GREATER_EQUAL),
                               BooleanLiteral::makeFalse()))})),
      ParserTestData(
          "Plus", "1+2;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BinaryExpr::make(
                  IntegerLiteral::make(1), Token::make(TokenType::TOKEN_PLUS),
                  IntegerLiteral::make(2)))})),
      ParserTestData(
          "Minus", "1-2;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BinaryExpr::make(
                  IntegerLiteral::make(1), Token::make(TokenType::TOKEN_MINUS),
                  IntegerLiteral::make(2)))})),
      ParserTestData(
          "Multiply", "1*2;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BinaryExpr::make(
                  IntegerLiteral::make(1), Token::make(TokenType::TOKEN_STAR),
                  IntegerLiteral::make(2)))})),
      ParserTestData(
          "Division", "1/2;",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(BinaryExpr::make(
                  IntegerLiteral::make(1), Token::make(TokenType::TOKEN_SLASH),
                  IntegerLiteral::make(2)))})),
  };

  assertTestCases(testCases);
}

TEST_F(ParserTest, ForStatementAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "EmptyForStatement", "for(;;);",
          Program::make(std::vector<StatementPtr>{ForStatement::make(
              Statement::make(), BooleanLiteral::makeTrue(),
              EmptyExpression::make(), Block::make({Statement::make()}))})),
      ParserTestData(
          "MinimalForStatementWithBlock", "for(;;){true;}",
          Program::make(std::vector<StatementPtr>{ForStatement::make(
              Statement::make(), BooleanLiteral::makeTrue(),
              EmptyExpression::make(),
              Block::make(std::vector<StatementPtr>{
                  ExpressionStatement::make(BooleanLiteral::makeTrue())}))})),
      ParserTestData(
          "ForStatementFalseCondition", "for(;false;){false;}",
          Program::make(std::vector<StatementPtr>{ForStatement::make(
              Statement::make(), BooleanLiteral::makeFalse(),
              EmptyExpression::make(),
              Block::make(std::vector<StatementPtr>{
                  ExpressionStatement::make(BooleanLiteral::makeFalse())}))})),
      ParserTestData(
          "ForStatementSimple", "for(var i = 0; i < 10; i = i + 1) { i; }",
          Program::make(std::vector<StatementPtr>{ForStatement::make(
              VarDeclaration::make("i", IntegerLiteral::make(0)),
              BinaryExpr::make(VariableExpr::make("i"),
                               Token::make(TokenType::TOKEN_LESS),
                               IntegerLiteral::make(10)),
              Assignment::make(
                  "i", BinaryExpr::make(VariableExpr::make("i"),
                                        Token::make(TokenType::TOKEN_PLUS),
                                        IntegerLiteral::make(1))),
              Block::make(std::vector<StatementPtr>{
                  ExpressionStatement::make(VariableExpr::make("i"))}))}))};

  assertTestCases(testCases);
}

TEST_F(ParserTest, BreakStatementAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData("DirectBreakInFor", "for(;;) { break; }",
                     Program::make(std::vector<StatementPtr>{ForStatement::make(
                         Statement::make(), BooleanLiteral::makeTrue(),
                         EmptyExpression::make(),
                         Block::make(std::vector<StatementPtr>{
                             BreakStatement::make()}))})),
      ParserTestData(
          "DirectBreakInWhile", "while(true) { break; }",
          Program::make(std::vector<StatementPtr>{WhileStatement::make(
              BooleanLiteral::makeTrue(), Block::make(std::vector<StatementPtr>{
                                              BreakStatement::make()}))}))};

  assertTestCases(testCases);
}

TEST_F(ParserTest, ContinueStatementAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData("DirectContinueInFor", "for(;;) { continue; }",
                     Program::make(std::vector<StatementPtr>{ForStatement::make(
                         Statement::make(), BooleanLiteral::makeTrue(),
                         EmptyExpression::make(),
                         Block::make(std::vector<StatementPtr>{
                             ContinueStatement::make()}))})),
      ParserTestData(
          "DirectContinueInWhile", "while(true) { continue; }",
          Program::make(std::vector<StatementPtr>{WhileStatement::make(
              BooleanLiteral::makeTrue(), Block::make(std::vector<StatementPtr>{
                                              ContinueStatement::make()}))}))};

  assertTestCases(testCases);
}

TEST_F(ParserTest, ArrayLiteralAssertions) {
  std::vector testCases = {
      ParserTestData(
          "EmptyArray", "[];",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              ArrayLiteral::make(std::vector<ExpressionPtr>{}))})),
      ParserTestData(
          "ArrayWithOneElement", "[1];",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(ArrayLiteral::make(
                  std::vector<ExpressionPtr>{IntegerLiteral::make(1)}))})),
      ParserTestData(
          "ArrayWithTwoElements", "[1, 2];",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              ArrayLiteral::make(std::vector<ExpressionPtr>{
                  IntegerLiteral::make(1), IntegerLiteral::make(2)}))})),
      ParserTestData(
          "ArrayWithSubArray", "[1, [2, 3]];",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              ArrayLiteral::make(std::vector<ExpressionPtr>{
                  IntegerLiteral::make(1),
                  ArrayLiteral::make(std::vector<ExpressionPtr>{
                      IntegerLiteral::make(2), IntegerLiteral::make(3)})}))})),
      ParserTestData(
          "ArrayWithString", "[\"hello\", \"world\"];",
          Program::make(std::vector<StatementPtr>{
              ExpressionStatement::make(ArrayLiteral::make(
                  std::vector<ExpressionPtr>{StringLiteral::make("hello"),
                                             StringLiteral::make("world")}))})),
      ParserTestData(
          "ArrayAssignment", "var a = [1, 2];",
          Program::make(std::vector<StatementPtr>{VarDeclaration::make(
              "a", ArrayLiteral::make(std::vector<ExpressionPtr>{
                       IntegerLiteral::make(1), IntegerLiteral::make(2)}))})),
      ParserTestData(
          "ArraySubscript", "var a = [1, 2]; var b = a[1];",
          Program::make(std::vector<StatementPtr>{
              VarDeclaration::make(
                  "a", ArrayLiteral::make(std::vector<ExpressionPtr>{
                           IntegerLiteral::make(1), IntegerLiteral::make(2)})),
              VarDeclaration::make(
                  "b", ArraySubscriptExpr::make(VariableExpr::make("a"),
                                                IntegerLiteral::make(1)))}))};
  assertTestCases(testCases);
}

TEST_F(ParserTest, ClassDeclarationAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData("EmptyClass", "class A {}",
                     Program::make(std::vector<StatementPtr>{
                         ClassDeclaration::make("A", nullptr, {}, {})})),
      ParserTestData(
          "ClassWithOneMethod", "class A { def method1() {} }",
          Program::make(std::vector<StatementPtr>{ClassDeclaration::make(
              "A", nullptr, {},
              {FunctionDeclaration::make("method1", {}, Block::make({}))})})),
      ParserTestData(
          "ClassWithTwoMethods",
          "class A { def method1() {} def method2() {} }",
          Program::make(std::vector<StatementPtr>{ClassDeclaration::make(
              "A", nullptr, {},
              {FunctionDeclaration::make("method1", {}, Block::make({})),
               FunctionDeclaration::make("method2", {}, Block::make({}))})})),
      ParserTestData(
          "ClassWithTwoMethodsWithMultipleArgs",
          "class A { def method1(a, b) {} def method2(c, d) {} }",
          Program::make(std::vector<StatementPtr>{ClassDeclaration::make(
              "A", nullptr, {},
              {FunctionDeclaration::make("method1", {"a", "b"},
                                         Block::make({})),
               FunctionDeclaration::make("method2", {"c", "d"},
                                         Block::make({}))})})),
      ParserTestData("TwoClasses", "class A {} class B {}",
                     Program::make(std::vector<StatementPtr>{
                         ClassDeclaration::make("A", nullptr, {}, {}),
                         ClassDeclaration::make("B", nullptr, {}, {})})),
      ParserTestData(
          "ClassWithConstructor", "class A { def __init__() {} }",
          Program::make(std::vector<StatementPtr>{ClassDeclaration::make(
              "A", FunctionDeclaration::make("__init__", {}, Block::make({})),
              {}, {})})),
  };

  assertTestCases(testCases);
}

TEST_F(ParserTest, MemberExpressionAssertions) {
  std::vector<ParserTestData> testCases = {
      ParserTestData(
          "Simple", "test.member;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              MemberExpr::make(VariableExpr::make("test"), "member"))})),
      ParserTestData(
          "SimpleSelf", "self.member;",
          Program::make(std::vector<StatementPtr>{ExpressionStatement::make(
              MemberExpr::make(VariableExpr::make("self"), "member"))})),
      ParserTestData(
          "VariableAssignmentFromSelf", "var a = self.member;",
          Program::make(std::vector<StatementPtr>{VarDeclaration::make(
              "a", MemberExpr::make(VariableExpr::make("self"), "member"))})),
      ParserTestData(
          "ClassWithMemberAccessAndCall",
          "class A { def method1() {} } var a = A(); a.method1; "
          "a.method1();",
          Program::make(std::vector<StatementPtr>{
              ClassDeclaration::make(
                  "A", nullptr, {},
                  {FunctionDeclaration::make("method1", {}, Block::make({}))}),
              VarDeclaration::make(
                  "a", CallExpr::make(VariableExpr::make("A"),
                                      std::vector<ExpressionPtr>{})),
              ExpressionStatement::make(
                  MemberExpr::make(VariableExpr::make("a"), "method1")),
              ExpressionStatement::make(CallExpr::make(
                  MemberExpr::make(VariableExpr::make("a"), "method1"),
                  std::vector<ExpressionPtr>{}))})),
      ParserTestData(
          "ClassWithMemberArgumentsAndCall",
          "class A { var a = 1; def method1(a, b) {} } var a = A(); "
          "a.method1(1, 2);",
          Program::make(std::vector<StatementPtr>{
              ClassDeclaration::make(
                  "A", nullptr,
                  {VarDeclaration::make("a", IntegerLiteral::make(1))},
                  {FunctionDeclaration::make("method1", {"a", "b"},
                                             Block::make({}))}),
              VarDeclaration::make(
                  "a", CallExpr::make(VariableExpr::make("A"),
                                      std::vector<ExpressionPtr>{})),
              ExpressionStatement::make(CallExpr::make(
                  MemberExpr::make(VariableExpr::make("a"), "method1"),
                  std::vector<ExpressionPtr>{IntegerLiteral::make(1),
                                             IntegerLiteral::make(2)}))})),
      ParserTestData(
          "ClassWithMemberArgumentsAndCallAndReturn",
          "class A { var a = 1; def method1(a, b) { return a + b; } var b = 2; "
          "} var a = "
          "A(); var b "
          "= a.method1(1, 2);",
          Program::make(std::vector<StatementPtr>{
              ClassDeclaration::make(
                  "A", nullptr,
                  {
                      VarDeclaration::make("a", IntegerLiteral::make(1)),
                      VarDeclaration::make("b", IntegerLiteral::make(2)),
                  },
                  {FunctionDeclaration::make(
                      "method1", {"a", "b"},
                      Block::make({ReturnStatement::make(
                          BinaryExpr::make(VariableExpr::make("a"),
                                           Token::make(TokenType::TOKEN_PLUS),
                                           VariableExpr::make("b")))}))}),
              VarDeclaration::make(
                  "a", CallExpr::make(VariableExpr::make("A"),
                                      std::vector<ExpressionPtr>{})),
              VarDeclaration::make(
                  "b",
                  CallExpr::make(
                      MemberExpr::make(VariableExpr::make("a"), "method1"),
                      std::vector<ExpressionPtr>{IntegerLiteral::make(1),
                                                 IntegerLiteral::make(2)}))}))};

  assertTestCases(testCases);
}