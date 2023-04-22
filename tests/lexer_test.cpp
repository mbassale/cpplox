#include "lexer.h"

#include <gtest/gtest.h>

#include "common.h"
#include "python_parser.hpp"

using namespace std;
using Parser::PythonParser;

struct LexerTestData {
  const char *name;
  const char *source;
  vector<PythonParser::token_kind_type> tokenTypes;

  LexerTestData(const char *name, const char *source,
                const vector<PythonParser::token_kind_type> &tokenTypes)
      : name(name), source(source), tokenTypes(tokenTypes) {}
};

class LexerTest : public ::testing::Test {
 protected:
  void assertTokenTypes(
      PythonLexer &lexer,
      const vector<PythonParser::token_kind_type> &expectedTokens);
};

void LexerTest::assertTokenTypes(
    PythonLexer &lexer,
    const vector<PythonParser::token_kind_type> &expectedTokenTypes) {
  size_t tokenCount = 0;
  std::for_each(expectedTokenTypes.begin(), expectedTokenTypes.end(),
                [&](const PythonParser::token_kind_type &expected) {
                  auto actual = lexer.yylex();
                  ASSERT_EQ(actual, expected);
                  tokenCount++;
                });
  ASSERT_EQ(tokenCount, expectedTokenTypes.size());
}

TEST_F(LexerTest, PunctuationTokenAssertions) {
  vector<LexerTestData> testCases = {
      LexerTestData("Punctuation", "(){}[],.-+;/*!!====>>=<<=",
                    vector<PythonParser::token_kind_type>{
                        // One character tokens.
                        PythonParser::token::LPAREN,
                        PythonParser::token::RPAREN,
                        PythonParser::token::LBRACE,
                        PythonParser::token::RBRACE,
                        PythonParser::token::LBRACKET,
                        PythonParser::token::RBRACKET,
                        PythonParser::token::COMMA,
                        PythonParser::token::DOT,
                        PythonParser::token::MINUS,
                        PythonParser::token::PLUS,
                        PythonParser::token::SEMICOLON,
                        PythonParser::token::SLASH,
                        PythonParser::token::STAR,
                        // One or two character tokens.
                        PythonParser::token::BANG,
                        PythonParser::token::BANG_EQUAL,
                        PythonParser::token::EQUAL_EQUAL,
                        PythonParser::token::EQUAL,
                        PythonParser::token::GREATER,
                        PythonParser::token::GREATER_EQUAL,
                        PythonParser::token::LESS,
                        PythonParser::token::LESS_EQUAL,
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
                    vector<PythonParser::token_kind_type>{
                        PythonParser::token::IF,
                        PythonParser::token::WHILE,
                        PythonParser::token::DEF,
                    }),
  };
  for (const auto testData : testCases) {
    std::stringstream ss(testData.source);
    PythonLexer lexer(&ss);
    assertTokenTypes(lexer, testData.tokenTypes);
  }
}
