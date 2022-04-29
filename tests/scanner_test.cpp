#include "scanner.h"

#include <gtest/gtest.h>

#include "common.h"

template <size_t N>
struct ScanTestData {
  const char *name;
  const char *source;
  std::array<TokenType, N> tokenTypes;

  ScanTestData(const char *name, const char *source,
               const std::array<TokenType, N> &tokenTypes)
      : name(name), source(source), tokenTypes(tokenTypes) {}
};

class ScannerTest : public ::testing::Test {
 protected:
  template <size_t N>
  void assertTokenTypes(Scanner &scanner,
                        const std::array<TokenType, N> &expectedTokens);
};

template <size_t N>
void ScannerTest::assertTokenTypes(
    Scanner &scanner, const std::array<TokenType, N> &expectedTokenTypes) {
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
  std::array<ScanTestData<19>, 1> testCases = {
      ScanTestData<19>("Punctuation", "(){},.-+;/*!!====>>=<<=",
                       std::array<TokenType, 19>{
                           // One character tokens.
                           TokenType::TOKEN_LEFT_PAREN,
                           TokenType::TOKEN_RIGHT_PAREN,
                           TokenType::TOKEN_LEFT_BRACE,
                           TokenType::TOKEN_RIGHT_BRACE,
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
    assertTokenTypes<19>(scanner, testData.tokenTypes);
  }
}

TEST_F(ScannerTest, KeywordTokenAssertions) {
  std::array<ScanTestData<16>, 1> testCases = {
      ScanTestData<16>("Punctuation",
                       "and class  else  false \tfor\t fun\tif\nnil\t\nor\n\t "
                       "print\n \t return \t super \n this  \t\t  true \n\t\n "
                       "var \t\n while \n\n\n \t\n\t \t \n  ",
                       std::array<TokenType, 16>{
                           // One character tokens.
                           TOKEN_AND,
                           TOKEN_CLASS,
                           TOKEN_ELSE,
                           TOKEN_FALSE,
                           TOKEN_FOR,
                           TOKEN_FUN,
                           TOKEN_IF,
                           TOKEN_NIL,
                           TOKEN_OR,
                           TOKEN_PRINT,
                           TOKEN_RETURN,
                           TOKEN_SUPER,
                           TOKEN_THIS,
                           TOKEN_TRUE,
                           TOKEN_VAR,
                           TOKEN_WHILE,
                       }),
  };
  for (const auto testData : testCases) {
    Scanner scanner(testData.source);
    assertTokenTypes<16>(scanner, testData.tokenTypes);
  }
}