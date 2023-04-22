#include "evaluator.h"

#include <gtest/gtest.h>

#include "ast.h"
#include "astbuilder.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "scanner.h"

using Parser::PythonParser;

using namespace cpplox;
using namespace std;

class EvaluatorTest : public ::testing::Test {
 protected:
  void expectIntValue(string_view testCase, ObjectPtr actualValue,
                      int64_t expectedValue) {
    ASSERT_EQ(actualValue->Type, ObjectType::OBJ_INTEGER) << testCase;
    auto actualIntValue = dynamic_pointer_cast<IntegerObject>(actualValue);
    ASSERT_NE(actualIntValue, nullptr) << testCase;
    EXPECT_EQ(actualIntValue->Value, expectedValue) << testCase;
  }

  void expectBoolValue(string_view testCase, ObjectPtr actualValue,
                       bool expectedValue) {
    ASSERT_EQ(actualValue->Type, ObjectType::OBJ_BOOLEAN) << testCase;
    auto actualBoolValue = dynamic_pointer_cast<BooleanObject>(actualValue);
    ASSERT_NE(actualBoolValue, nullptr) << testCase;
    EXPECT_EQ(actualBoolValue->Value, expectedValue) << testCase;
  }
};

TEST_F(EvaluatorTest, TestLiterals) {
  struct TestCase {
    string source;
    string expectedValue;
  };
  vector<TestCase> testCases = {
      TestCase{"true;", "true"},   TestCase{"false;", "false"},
      TestCase{"nil;", "nil"},     TestCase{"1;", "1"},
      TestCase{"10000;", "10000"}, TestCase{"\"test\";", "test"},
      TestCase{"\"\";", ""},       TestCase{";", "nil"}};

  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    ASSERT_NE(value, nullptr);
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
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
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
      TestCase{"1+1;", 2, nullopt},
      TestCase{"1-1;", 0, nullopt},
      TestCase{"2*2;", 4, nullopt},
      TestCase{"10/2;", 5, nullopt},
      TestCase{"1+2*3-4/5;", 7, nullopt},
      TestCase{"true and true;", nullopt, true},
      TestCase{"true and false;", nullopt, false},
      TestCase{"false and false;", nullopt, false},
      TestCase{"true and true and true;", nullopt, true},
      TestCase{"true or true;", nullopt, true},
      TestCase{"true or false;", nullopt, true},
      TestCase{"false or false;", nullopt, false},
      TestCase{"false or false or true;", nullopt, true},
      TestCase{"1 < 2;", nullopt, true},
      TestCase{"1 <= 1;", nullopt, true},
      TestCase{"2 > 1;", nullopt, true},
      TestCase{"1 >= 1;", nullopt, true},
      TestCase{"1 == 1;", nullopt, true},
      TestCase{"1 != 1;", nullopt, false},
      TestCase{"2 != 1;", nullopt, true},
      TestCase{"1 < 2 and 3 >= 3;", nullopt, true},
      TestCase{"1 > 2 or 2 >= 2;", nullopt, true},
  };
  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    if (testCase.expectedIntValue.has_value()) {
      ASSERT_EQ(value->Type, ObjectType::OBJ_INTEGER)
          << "TestCase: " << testCase.source;
      auto intValue = std::static_pointer_cast<IntegerObject>(value);
      EXPECT_EQ(intValue->Value, *testCase.expectedIntValue)
          << "TestCase: " << testCase.source;
    } else if (testCase.expectedBoolValue.has_value()) {
      ASSERT_EQ(value->Type, ObjectType::OBJ_BOOLEAN)
          << "TestCase: " << testCase.source;
      auto boolValue = std::static_pointer_cast<BooleanObject>(value);
      EXPECT_EQ(boolValue->Value, *testCase.expectedBoolValue)
          << "TestCase: " << testCase.source;
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
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
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

TEST_F(EvaluatorTest, TestAssignmentExpressions) {
  struct TestCase {
    string source;
    unordered_map<string, variant<int, bool>> expectedValues;
  };
  vector<TestCase> testCases = {
      TestCase{"var a = 1; var b = 1; a = 1; b = a + 1;", {{"a", 1}, {"b", 2}}},
      TestCase{"var a = false; var b = true; var c = nil; c = a or b;",
               {{"a", false}, {"b", true}, {"c", true}}},
      TestCase{"var a = 2; var b = a * 2; a = a * b;", {{"a", 8}, {"b", 4}}}};

  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    const auto value = evaluator.eval(program);
    if (testCase.expectedValues.size() > 0) {
      for (const auto& pair : testCase.expectedValues) {
        auto actualValue = evaluator.getGlobalValue(pair.first);
        auto& expectedValue = pair.second;
        if (std::holds_alternative<int>(expectedValue)) {
          expectIntValue(testCase.source, actualValue,
                         std::get<int>(expectedValue));
        } else if (std::holds_alternative<bool>(expectedValue)) {
          expectBoolValue(testCase.source, actualValue,
                          std::get<bool>(expectedValue));
        }
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
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    auto value = evaluator.eval(program);
    if (testCase.expectedValue.has_value()) {
      expectIntValue(testCase.source, value, *testCase.expectedValue);
    }
  }
}

TEST_F(EvaluatorTest, TestIfStmts) {
  struct TestCase {
    string source;
    std::optional<int> expectedValue;
  };
  vector<TestCase> testCases = {
      TestCase{"if(true){ 1; } else { 2; }", 1},
      TestCase{"if(false){ 1; } else { 2; }", 2},
      TestCase{"var i = 0; var n = 10; if(i <= n) { 1; } else { 2; }", 1},
      TestCase{"var i = 0; var n = 10; if(i > n) { 1; } else { 2; }", 2},
      TestCase{"var n = 10; if (n > 0) { n; } else { 0; }", 10},
      TestCase{"var n = 10; if (n == 10) { n; } else { 0; }", 10},
      TestCase{"var n = 10; if (n == 0) { n; } else { 0; }", 0},
  };

  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    auto value = evaluator.eval(program);
    if (testCase.expectedValue.has_value()) {
      expectIntValue(testCase.source, value, *testCase.expectedValue);
    }
  }
}

TEST_F(EvaluatorTest, TestForStmts) {
  struct TestCase {
    string source;
    unordered_map<string, variant<bool, int>> expectedValues;
  };
  vector<TestCase> testCases = {
      TestCase{"for(;false;){}", {}},
      TestCase{"var test = false; for(; test;) { false; }", {{"test", false}}},
      TestCase{"var globalVar = 0; for(var i = 0; i < 10; i = i + 1) { "
               "globalVar = i; }",
               {{"globalVar", 9}}},
      TestCase{"var n = 10; var res = 0; for(var i = 0; i < n; i = i + 1) { "
               "res = i; }",
               {{"res", 9}}}};

  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    evaluator.eval(program);
    for (const auto& pair : testCase.expectedValues) {
      auto value = evaluator.getGlobalValue(pair.first);
      if (std::holds_alternative<bool>(pair.second)) {
        expectBoolValue(testCase.source, value, std::get<bool>(pair.second));
      } else if (std::holds_alternative<int>(pair.second)) {
        expectIntValue(testCase.source, value, std::get<int>(pair.second));
      }
    }
  }
}

TEST_F(EvaluatorTest, TestWhileStmts) {
  struct TestCase {
    string source;
    unordered_map<string, variant<bool, int>> expectedValues;
  };
  vector<TestCase> testCases = {
      TestCase{"while(false);", {}}, TestCase{"while(false){}", {}},
      TestCase{"var test = false; while(test){}", {{"test", false}}},
      TestCase{"var i = 0; while (i < 10){ i = i + 1; }", {{"i", 10}}}};

  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    evaluator.eval(program);
    for (const auto& pair : testCase.expectedValues) {
      auto value = evaluator.getGlobalValue(pair.first);
      if (std::holds_alternative<bool>(pair.second)) {
        expectBoolValue(testCase.source, value, std::get<bool>(pair.second));
      } else if (std::holds_alternative<int>(pair.second)) {
        expectIntValue(testCase.source, value, std::get<int>(pair.second));
      }
    }
  }
}

TEST_F(EvaluatorTest, TestPrintStmts) {
  struct TestCase {
    string source;
    variant<bool, int> expectedValue;
  };
  vector<TestCase> testCases = {
      TestCase{"print true;", true}, TestCase{"print false;", false},
      TestCase{"print 1+2*3;", 7}, TestCase{"var a = 2; print a*2+1;", 5}};

  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    auto value = evaluator.eval(program);
    if (std::holds_alternative<bool>(testCase.expectedValue)) {
      expectBoolValue(testCase.source, value,
                      std::get<bool>(testCase.expectedValue));
    } else if (std::holds_alternative<int>(testCase.expectedValue)) {
      expectIntValue(testCase.source, value,
                     std::get<int>(testCase.expectedValue));
    }
  }
}

TEST_F(EvaluatorTest, TestFunctionDeclarationStmts) {
  struct TestCase {
    string source;
    string expectedFunctionName;
    int expectedFunctionArity;
  };
  vector<TestCase> testCases = {
      TestCase{"fun test(arg1, arg2) { print arg1 + arg2; }", "test", 2},
      TestCase{"fun test() {}", "test", 0}};

  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    evaluator.eval(program);
    auto value = evaluator.getGlobalValue(testCase.expectedFunctionName);
    ASSERT_NE(value, nullptr);
    auto funcValue = dynamic_pointer_cast<Function>(value);
    ASSERT_NE(funcValue, nullptr);
    EXPECT_EQ(funcValue->getArity(), testCase.expectedFunctionArity);
  }
}

TEST_F(EvaluatorTest, TestFunctionCallExpression) {
  struct TestCase {
    string source;
    unordered_map<string, variant<int, bool>> expectedValues;
  };
  vector<TestCase> testCases = {
      TestCase{"fun test(arg1) { arg1 + 1; } var a = test(1);", {{"a", 2}}},
      TestCase{"fun test1(arg1, arg2) { arg1 + arg2; } fun test2(arg1) { 2 * "
               "arg1; } var a = test2(test1(1, 1));",
               {{"a", 4}}},
      TestCase{"fun test1(arg1, arg2) { arg1+arg2; } var a = test1(2*2, 2+2);",
               {{"a", 8}}},
      TestCase{"fun test1(arg1, arg2) { arg1 and arg2; } var a = "
               "test1(test1(true, true),test1(false,true));",
               {{"a", false}}},
      TestCase{"fun test(a){ return a + 1; }"
               "var a = test(1); var b = test(2);",
               {{"a", 2}, {"b", 3}}},
      TestCase{"fun max(a, b) { if (a >= b) { return a; } else { return b; }} "
               "var a = max(1,2); var b = max(2,3);",
               {{"a", 2}, {"b", 3}}},
      TestCase{"fun test(a,b) { while (a < b) { a = "
               "a + 1; if (a > 5) { return 90; }}} var a = test(1,10);",
               {{"a", 90}}}};

  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    evaluator.eval(program);
    for (const auto& pair : testCase.expectedValues) {
      auto value = evaluator.getGlobalValue(pair.first);
      if (std::holds_alternative<int>(pair.second)) {
        expectIntValue(testCase.source, value, std::get<int>(pair.second));
      } else if (std::holds_alternative<bool>(pair.second)) {
        expectBoolValue(testCase.source, value, std::get<bool>(pair.second));
      }
    }
  }
}

TEST_F(EvaluatorTest, TestBreakStatement) {
  struct TestCase {
    string source;
    unordered_map<string, variant<int, bool>> expectedValues;
  };
  vector<TestCase> testCases = {
      TestCase{
          "var a; for (a = 1; a < 10; a = a + 1) { if (a == 5) { break; }}",
          {{"a", 5}}},
      TestCase{
          "var a = 1; while (a < 10) { if (a == 5) { break; } a = a + 1; }",
          {{"a", 5}}}};

  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    evaluator.eval(program);
    for (const auto& pair : testCase.expectedValues) {
      auto value = evaluator.getGlobalValue(pair.first);
      if (std::holds_alternative<int>(pair.second)) {
        expectIntValue(testCase.source, value, std::get<int>(pair.second));
      } else if (std::holds_alternative<bool>(pair.second)) {
        expectBoolValue(testCase.source, value, std::get<bool>(pair.second));
      }
    }
  }
}

TEST_F(EvaluatorTest, TestArrayExpressions) {
  struct TestCase {
    string source;
    unordered_map<string, variant<vector<int>, int>> expectedValues;
  };
  vector<TestCase> testCases = {
      TestCase{"var a = [1,2,3];", {{"a", vector<int>{1, 2, 3}}}},
      TestCase{"var a = [1,2,3]; var b = [1];",
               {{"a", vector<int>{1, 2, 3}}, {"b", vector<int>{1}}}},
      TestCase{"var a = [1,2,3]; var b = a[0]; var c = a[1]; var d = a[2];",
               {{"a", vector<int>{1, 2, 3}}, {"b", 1}, {"c", 2}, {"d", 3}}}};
  for (const auto& testCase : testCases) {
    std::istringstream ss(testCase.source);
    PythonLexer lexer(&ss);
    ASTBuilderImpl builder;
    PythonParser parser(builder, lexer);
    parser.parse();
    auto program = builder.getProgram();
    ASSERT_NE(program, nullptr);
    Evaluator evaluator;
    evaluator.eval(program);
    for (const auto& pair : testCase.expectedValues) {
      auto value = evaluator.getGlobalValue(pair.first);
      ASSERT_NE(value, nullptr);
      if (std::holds_alternative<int>(pair.second)) {
        expectIntValue(testCase.source, value, std::get<int>(pair.second));
      } else if (std::holds_alternative<vector<int>>(pair.second)) {
        auto arrayValue = dynamic_pointer_cast<ArrayObject>(value);
        auto expectedArray = std::get<vector<int>>(pair.second);
        ASSERT_NE(arrayValue, nullptr);
        EXPECT_EQ(arrayValue->Values.size(), expectedArray.size());
        for (size_t i = 0; i < expectedArray.size(); ++i) {
          expectIntValue(testCase.source, arrayValue->Values[i],
                         expectedArray[i]);
        }
      }
    }
  }
}