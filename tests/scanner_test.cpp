#include <gtest/gtest.h>
#include "common.h"
#include "scanner.h"

template <size_t N>
struct ScanTestData
{
    const char *name;
    const char *source;
    std::array<Token, N> tokens;

    ScanTestData(const char *name, const char *source, const std::array<Token, N> &tokens) : name(name), source(source), tokens(tokens) {}
};

TEST(ScannerTest, PunctuationTokenAssertions)
{
    std::array<ScanTestData<19>, 1> testCases = {
        ScanTestData<19>(
            "Punctuation",
            "(){},.-+;/*!!====>>=<<=",
            std::array<Token, 19>{
                // One character tokens.
                Token(TokenType::TOKEN_LEFT_PAREN),
                Token(TokenType::TOKEN_RIGHT_PAREN),
                Token(TokenType::TOKEN_LEFT_BRACE),
                Token(TokenType::TOKEN_RIGHT_BRACE),
                Token(TokenType::TOKEN_COMMA),
                Token(TokenType::TOKEN_DOT),
                Token(TokenType::TOKEN_MINUS),
                Token(TokenType::TOKEN_PLUS),
                Token(TokenType::TOKEN_SEMICOLON),
                Token(TokenType::TOKEN_SLASH),
                Token(TokenType::TOKEN_STAR),
                // One or two character tokens.
                Token(TokenType::TOKEN_BANG),
                Token(TokenType::TOKEN_BANG_EQUAL),
                Token(TokenType::TOKEN_EQUAL_EQUAL),
                Token(TokenType::TOKEN_EQUAL),
                Token(TokenType::TOKEN_GREATER),
                Token(TokenType::TOKEN_GREATER_EQUAL),
                Token(TokenType::TOKEN_LESS),
                Token(TokenType::TOKEN_LESS_EQUAL),
            }),
    };
    for (const auto testData : testCases)
    {
        std::vector<Token> actualTokens{};
        size_t tokenCount = 0;
        Scanner scanner(testData.source);
        std::for_each(
            testData.tokens.begin(),
            testData.tokens.end(),
            [&](const Token &expected)
            {
                auto actual = scanner.next();
                ASSERT_EQ(actual.type, expected.type);
                tokenCount++;
            });
        ASSERT_EQ(tokenCount, testData.tokens.size());
        // Scanner should return EOF after the source is completely scanned.
        ASSERT_EQ(scanner.next().type, TokenType::TOKEN_EOF);
        ASSERT_EQ(scanner.next().type, TokenType::TOKEN_EOF);
        ASSERT_EQ(scanner.next().type, TokenType::TOKEN_EOF);
    }
}
