#include "lexer.h"

#include <gtest/gtest.h>

#include "common.h"
#include "python_parser.hpp"

using namespace std;

struct LexerTestData {
  const char *name;
  const char *source;
  vector<PythonParser::PythonParser::token_kind_type> tokenTypes;

  LexerTestData(
      const char *name, const char *source,
      const vector<PythonParser::PythonParser::token_kind_type> &tokenTypes)
      : name(name), source(source), tokenTypes(tokenTypes) {}
};

class LexerTest : public ::testing::Test {
 protected:
  void assertTokenTypes(
      PythonLexer &lexer,
      const vector<PythonParser::PythonParser::token_kind_type>
          &expectedTokens);
};

void LexerTest::assertTokenTypes(
    PythonLexer &lexer,
    const vector<PythonParser::PythonParser::token_kind_type>
        &expectedTokenTypes) {
  size_t tokenCount = 0;
  std::for_each(
      expectedTokenTypes.begin(), expectedTokenTypes.end(),
      [&](const PythonParser::PythonParser::token_kind_type &expected) {
        auto actual = lexer.yylex();
        ASSERT_EQ(actual, expected);
        tokenCount++;
      });
  ASSERT_EQ(tokenCount, expectedTokenTypes.size());
}

TEST_F(LexerTest, PunctuationTokenAssertions) {
  vector<LexerTestData> testCases = {
      LexerTestData("Punctuation", "(){}[],.-+;/*!!====>>=<<=",
                    vector<PythonParser::PythonParser::token_kind_type>{
                        // One character tokens.
                        PythonParser::PythonParser::token::LPAREN,
                        PythonParser::PythonParser::token::RPAREN,
                        PythonParser::PythonParser::token::LBRACE,
                        PythonParser::PythonParser::token::RBRACE,
                        PythonParser::PythonParser::token::LBRACKET,
                        PythonParser::PythonParser::token::RBRACKET,
                        PythonParser::PythonParser::token::COMMA,
                        PythonParser::PythonParser::token::DOT,
                        PythonParser::PythonParser::token::MINUS,
                        PythonParser::PythonParser::token::PLUS,
                        PythonParser::PythonParser::token::SEMICOLON,
                        PythonParser::PythonParser::token::SLASH,
                        PythonParser::PythonParser::token::STAR,
                        // One or two character tokens.
                        PythonParser::PythonParser::token::BANG,
                        PythonParser::PythonParser::token::BANG_EQUAL,
                        PythonParser::PythonParser::token::EQUAL_EQUAL,
                        PythonParser::PythonParser::token::EQUAL,
                        PythonParser::PythonParser::token::GREATER,
                        PythonParser::PythonParser::token::GREATER_EQUAL,
                        PythonParser::PythonParser::token::LESS,
                        PythonParser::PythonParser::token::LESS_EQUAL,
                    }),
  };
  for (const auto testData : testCases) {
    std::stringstream ss(testData.source);
    PythonLexer lexer(&ss);
    assertTokenTypes(lexer, testData.tokenTypes);
  }
}

TEST_F(LexerTest, KeywordTokenAssertions) {
  vector<LexerTestData> testCases = {
      LexerTestData("Keywords", " if   while  def ",
                    vector<PythonParser::PythonParser::token_kind_type>{
                        PythonParser::PythonParser::token::IF,
                        PythonParser::PythonParser::token::WHILE,
                        PythonParser::PythonParser::token::DEF,
                    }),
  };
  for (const auto testData : testCases) {
    std::stringstream ss(testData.source);
    PythonLexer lexer(&ss);
    assertTokenTypes(lexer, testData.tokenTypes);
  }
}
