#include "scanner.h"

#include <gtest/gtest.h>

#include "common.h"

using namespace std;

struct ScanTestData {
  const char *name;
  const char *source;
  vector<TokenType> tokenTypes;

  ScanTestData(const char *name, const char *source,
               const vector<TokenType> &tokenTypes)
      : name(name), source(source), tokenTypes(tokenTypes) {}
};

class ScannerTest : public ::testing::Test {
 protected:
  void assertTokenTypes(Scanner &scanner,
                        const vector<TokenType> &expectedTokens);
};

void ScannerTest::assertTokenTypes(
    Scanner &scanner, const vector<TokenType> &expectedTokenTypes) {
  size_t tokenCount = 0;
  std::for_each(expectedTokenTypes.begin(), expectedTokenTypes.end(),
                [&](const TokenType &expected) {
                  auto actual = scanner.next();
                  ASSERT_EQ(actual.type, expected);
                  tokenCount++;
                });
  ASSERT_EQ(tokenCount, expectedTokenTypes.size());
  // Scanner should return EOF after the source is completely scanned.
  ASSERT_EQ(scanner.next().type, TokenType::TOKEN_EOF);
  ASSERT_EQ(scanner.next().type, TokenType::TOKEN_EOF);
  ASSERT_EQ(scanner.next().type, TokenType::TOKEN_EOF);
}

TEST_F(ScannerTest, PunctuationTokenAssertions) {
  vector<ScanTestData> testCases = {
      ScanTestData("Punctuation", "(){}[],.-+;/*!!====>>=<<=",
                   vector<TokenType>{
                       // One character tokens.
                       TokenType::TOKEN_LEFT_PAREN,
                       TokenType::TOKEN_RIGHT_PAREN,
                       TokenType::TOKEN_LEFT_BRACE,
                       TokenType::TOKEN_RIGHT_BRACE,
                       TokenType::TOKEN_LEFT_BRACKET,
                       TokenType::TOKEN_RIGHT_BRACKET,
                       TokenType::TOKEN_COMMA,
                       TokenType::TOKEN_DOT,
                       TokenType::TOKEN_MINUS,
                       TokenType::TOKEN_PLUS,
                       TokenType::TOKEN_SEMICOLON,
                       TokenType::TOKEN_SLASH,
                       TokenType::TOKEN_STAR,
                       // One or two character tokens.
                       TokenType::TOKEN_BANG,
                       TokenType::TOKEN_BANG_EQUAL,
                       TokenType::TOKEN_EQUAL_EQUAL,
                       TokenType::TOKEN_EQUAL,
                       TokenType::TOKEN_GREATER,
                       TokenType::TOKEN_GREATER_EQUAL,
                       TokenType::TOKEN_LESS,
                       TokenType::TOKEN_LESS_EQUAL,
                   }),
  };
  for (const auto testData : testCases) {
    Scanner scanner(testData.source);
    assertTokenTypes(scanner, testData.tokenTypes);
  }
}

TEST_F(ScannerTest, KeywordTokenAssertions) {
  vector<ScanTestData> testCases = {
      ScanTestData("Punctuation",
                   "and class  else  false \tfor\t fun\tif\nnil\t\nor\n\t "
                   "print\n \t return \t super \n this  \t\t  true \n\t\n "
                   "var \t\n while \n\t break \n\n\n \t\n\t \t \n  ",
                   vector<TokenType>{
                       // One character tokens.
                       TokenType::TOKEN_AND,
                       TokenType::TOKEN_CLASS,
                       TokenType::TOKEN_ELSE,
                       TokenType::TOKEN_FALSE,
                       TokenType::TOKEN_FOR,
                       TokenType::TOKEN_FUN,
                       TokenType::TOKEN_IF,
                       TokenType::TOKEN_NIL,
                       TokenType::TOKEN_OR,
                       TokenType::TOKEN_PRINT,
                       TokenType::TOKEN_RETURN,
                       TokenType::TOKEN_SUPER,
                       TokenType::TOKEN_THIS,
                       TokenType::TOKEN_TRUE,
                       TokenType::TOKEN_VAR,
                       TokenType::TOKEN_WHILE,
                       TokenType::TOKEN_BREAK,
                   }),
  };
  for (const auto testData : testCases) {
    Scanner scanner(testData.source);
    assertTokenTypes(scanner, testData.tokenTypes);
  }
}