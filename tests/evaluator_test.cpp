#include "evaluator.h"

#include <gtest/gtest.h>

#include "common.h"
#include "parser.h"
#include "scanner.h"

using namespace cpplox;
using namespace std;

class EvaluatorTest : public ::testing::Test {};

TEST_F(EvaluatorTest, TestLiterals) {
  struct TestCase {
    string source;
    string expectedValue;
  };
  vector<TestCase> testCases = {
      TestCase{"true;", "true"},   TestCase{"false;", "false"},
      TestCase{"nil;", "nil"},     TestCase{"1;", "1"},
      TestCase{"10000;", "10000"}, TestCase{"\"test\";", "test"},
      TestCase{"\"\";", ""}};

  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    EXPECT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    EXPECT_EQ(testCase.expectedValue, value->toString());
  }
}

TEST_F(EvaluatorTest, TestUnaryExpression) {
  struct TestCase {
    string source;
    std::optional<int> expectedIntValue;
    std::optional<bool> expectedBoolValue;
  };
  vector<TestCase> testCases = {TestCase{"-1;", -1, nullopt},
                                TestCase{"!true;", nullopt, false}};
  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    ASSERT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    if (testCase.expectedIntValue.has_value()) {
      ASSERT_EQ(value->Type, ObjectType::OBJ_INTEGER);
      auto intValue = std::static_pointer_cast<IntegerObject>(value);
      EXPECT_EQ(intValue->Value, *testCase.expectedIntValue);
    } else if (testCase.expectedBoolValue.has_value()) {
      ASSERT_EQ(value->Type, ObjectType::OBJ_BOOLEAN);
      auto boolValue = std::static_pointer_cast<BooleanObject>(value);
      EXPECT_EQ(boolValue->Value, *testCase.expectedBoolValue);
    }
  }
}

TEST_F(EvaluatorTest, TestBinaryExpression) {
  struct TestCase {
    string source;
    std::optional<int> expectedIntValue;
    std::optional<bool> expectedBoolValue;
  };
  vector<TestCase> testCases = {
      TestCase{"1+1;", 2, nullopt}, TestCase{"1-1;", 0, nullopt},
      TestCase{"2*2;", 4, nullopt}, TestCase{"10/2;", 5, nullopt},
      TestCase{"1+2*3-4/5;", 7, nullopt}};
  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    ASSERT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    if (testCase.expectedIntValue.has_value()) {
      ASSERT_EQ(value->Type, ObjectType::OBJ_INTEGER);
      auto intValue = std::static_pointer_cast<IntegerObject>(value);
      EXPECT_EQ(intValue->Value, *testCase.expectedIntValue);
    } else if (testCase.expectedBoolValue.has_value()) {
      ASSERT_EQ(value->Type, ObjectType::OBJ_BOOLEAN);
      auto boolValue = std::static_pointer_cast<BooleanObject>(value);
      EXPECT_EQ(boolValue->Value, *testCase.expectedBoolValue);
    }
  }
}

TEST_F(EvaluatorTest, TestVarDeclarationStmts) {
  struct TestCase {
    string source;
    std::unordered_map<std::string, int> expectedValues;
  };
  vector<TestCase> testCases = {
      TestCase{"var test;", {}}, TestCase{"var a=1;", {{"a", 1}}},
      TestCase{"var a=-1;", {{"a", -1}}},
      TestCase{"var a=1; var b=2; var c=3;", {{"a", 1}, {"b", 2}, {"c", 3}}},
      TestCase{"var a=1; var b=a*2; var c=b*b;",
               {{"a", 1}, {"b", 2}, {"c", 4}}}};

  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    ASSERT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    if (testCase.expectedValues.size() > 0) {
      auto it = testCase.expectedValues.begin();
      while (it != testCase.expectedValues.end()) {
        auto actualValue = evaluator.getGlobalValue(it->first);
        ASSERT_EQ(actualValue->Type, ObjectType::OBJ_INTEGER);
        auto actualIntValue =
            std::static_pointer_cast<IntegerObject>(actualValue);
        EXPECT_EQ(actualIntValue->Value, it->second);
        it++;
      }
    } else {
      EXPECT_EQ(value->Type, ObjectType::OBJ_NULL);
    }
  }
}

TEST_F(EvaluatorTest, TestBlockStmts) {
  struct TestCase {
    string source;
    std::optional<int> expectedValue;
  };
  vector<TestCase> testCases = {TestCase{"{}", nullopt},
                                TestCase{"{ 1; 2; 3; }", 3},
                                TestCase{"{ true; 1; }", 1}};

  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    ASSERT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    auto value = evaluator.eval(program);
    if (testCase.expectedValue.has_value()) {
      ASSERT_EQ(value->Type, ObjectType::OBJ_INTEGER);
      auto intValue = std::static_pointer_cast<IntegerObject>(value);
      EXPECT_EQ(intValue->Value, *testCase.expectedValue);
    }
  }
}

TEST_F(EvaluatorTest, TestIfStmts) {
  struct TestCase {
    string source;
    std::optional<int> expectedValue;
  };
  vector<TestCase> testCases = {TestCase{"if(true){ 1; } else { 2; }", 1},
                                TestCase{"if(false){ 1; } else { 2; }", 2}};

  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    ASSERT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    auto value = evaluator.eval(program);
    if (testCase.expectedValue.has_value()) {
      ASSERT_EQ(value->Type, ObjectType::OBJ_INTEGER);
      auto intValue = std::static_pointer_cast<IntegerObject>(value);
      EXPECT_EQ(intValue->Value, *testCase.expectedValue);
    }
  }
}
/*
TEST_F(EvaluatorTest, TestForStmts) {
  struct TestCase {
    string source;
    std::optional<int> expectedValue;
  };
  vector<TestCase> testCases = {
      TestCase{"for(var i = 0; i < 10; i = i + 1){ 1; }", 1}};

  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    ASSERT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    auto value = evaluator.eval(program);
    if (testCase.expectedValue.has_value()) {
      ASSERT_EQ(value->Type, ObjectType::OBJ_INTEGER);
      auto intValue = std::static_pointer_cast<IntegerObject>(value);
      EXPECT_EQ(intValue->Value, *testCase.expectedValue);
    }
  }
}
*/