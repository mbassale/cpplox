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

  for (const auto testCase : testCases) {
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    auto program = parser.parse();
    EXPECT_FALSE(parser.hasErrors());
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    EXPECT_EQ(testCase.expectedValue, value->toString());
  }
}
