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
  array<TestCase, 7> testCases = {
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
  array<TestCase, 2> testCases = {TestCase{"-1;", -1, nullopt},
                                  TestCase{"!true;", nullopt, false}};
  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    EXPECT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    if (testCase.expectedIntValue.has_value()) {
      EXPECT_EQ(value->Type, ObjectType::OBJ_INTEGER);
      auto intValue = std::static_pointer_cast<IntegerObject>(value);
      EXPECT_EQ(intValue->Value, *testCase.expectedIntValue);
    } else if (testCase.expectedBoolValue.has_value()) {
      EXPECT_EQ(value->Type, ObjectType::OBJ_BOOLEAN);
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
  array<TestCase, 4> testCases = {
      TestCase{"var test;", {}}, TestCase{"var a=1;", {{"a", 1}}},
      TestCase{"var a=-1;", {{"a", -1}}},
      TestCase{"var a=1; var b=2; var c=3;", {{"a", 1}, {"b", 2}, {"c", 3}}}};

  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    EXPECT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    if (testCase.expectedValues.size() > 0) {
      auto it = testCase.expectedValues.begin();
      while (it != testCase.expectedValues.end()) {
        auto actualValue = evaluator.getGlobalValue(it->first);
        EXPECT_EQ(actualValue->Type, ObjectType::OBJ_INTEGER);
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
  array<TestCase, 3> testCases = {TestCase{"{}", nullopt},
                                  TestCase{"{ 1; 2; 3; }", 3},
                                  TestCase{"{ true; 1; }", 1}};

  for (const auto& testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    EXPECT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    if (testCase.expectedValue.has_value()) {
      EXPECT_EQ(value->Type, ObjectType::OBJ_INTEGER);
      const auto intValue = std::static_pointer_cast<IntegerObject>(value);
      EXPECT_EQ(intValue->Value, *testCase.expectedValue);
    }
  }
}
