#include "lexer.h"

#include <gtest/gtest.h>

#include "common.h"
#include "parser.h"

using namespace std;
using Parser::JSParser;

struct LexerTestData {
  const char *name;
  const char *source;
  vector<JSParser::token_kind_type> tokenTypes;

  LexerTestData(const char *name, const char *source,
                const vector<JSParser::token_kind_type> &tokenTypes)
      : name(name), source(source), tokenTypes(tokenTypes) {}
};

class LexerTest : public ::testing::Test {
 protected:
  void assertTokenTypes(
      JSLexer &lexer, const vector<JSParser::token_kind_type> &expectedTokens);
};

void LexerTest::assertTokenTypes(
    JSLexer &lexer,
    const vector<JSParser::token_kind_type> &expectedTokenTypes) {
  size_t tokenCount = 0;
  std::for_each(expectedTokenTypes.begin(), expectedTokenTypes.end(),
                [&](const JSParser::token_kind_type &expected) {
                  auto actual = lexer.yylex();
                  ASSERT_EQ(actual, expected);
                  tokenCount++;
                });
  ASSERT_EQ(tokenCount, expectedTokenTypes.size());
}

TEST_F(LexerTest, PunctuationTokenAssertions) {
  vector<LexerTestData> testCases = {
      LexerTestData("Punctuation", "(){}[],.-+;/*!!====>>=<<=",
                    vector<JSParser::token_kind_type>{
                        // One character tokens.
                        JSParser::token::LPAREN,
                        JSParser::token::RPAREN,
                        JSParser::token::LBRACE,
                        JSParser::token::RBRACE,
                        JSParser::token::LBRACKET,
                        JSParser::token::RBRACKET,
                        JSParser::token::COMMA,
                        JSParser::token::DOT,
                        JSParser::token::MINUS,
                        JSParser::token::PLUS,
                        JSParser::token::SEMICOLON,
                        JSParser::token::SLASH,
                        JSParser::token::STAR,
                        // One or two character tokens.
                        JSParser::token::BANG,
                        JSParser::token::BANG_EQUAL,
                        JSParser::token::EQUAL_EQUAL,
                        JSParser::token::EQUAL,
                        JSParser::token::GREATER,
                        JSParser::token::GREATER_EQUAL,
                        JSParser::token::LESS,
                        JSParser::token::LESS_EQUAL,
                    }),
  };
  for (const auto testData : testCases) {
    std::stringstream ss(testData.source);
    JSLexer lexer(&ss);
    assertTokenTypes(lexer, testData.tokenTypes);
  }
}

TEST_F(LexerTest, KeywordTokenAssertions) {
  vector<LexerTestData> testCases = {
      LexerTestData("Keywords", "if while def true false class var null continue break",
                    vector<JSParser::token_kind_type>{
                        JSParser::token::IF,
                        JSParser::token::WHILE,
                        JSParser::token::DEF,
                        JSParser::token::TRUE,
                        JSParser::token::FALSE,
                        JSParser::token::CLASS,
                        JSParser::token::VAR,
                        JSParser::token::NIL,
                        JSParser::token::CONTINUE,
                        JSParser::token::BREAK,
                    }),
  };
  for (const auto testData : testCases) {
    std::stringstream ss(testData.source);
    JSLexer lexer(&ss);
    assertTokenTypes(lexer, testData.tokenTypes);
  }
}
