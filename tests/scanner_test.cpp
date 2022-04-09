#include <gtest/gtest.h>
#include "common.h"
#include "scanner.h"

template <size_t N>
struct ScanTestData
{
    const char *name;
    const char *source;
    std::array<TokenType, N> tokenTypes;

    ScanTestData(const char *name, const char *source, const std::array<TokenType, N> &tokenTypes) : name(name), source(source), tokenTypes(tokenTypes) {}
};

TEST(ScannerTest, PunctuationTokenAssertions)
{
    std::array<ScanTestData<19>, 1> testCases = {
        ScanTestData<19>(
            "Punctuation",
            "(){},.-+;/*!!====>>=<<=",
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
    for (const auto testData : testCases)
    {
        size_t tokenCount = 0;
        Scanner scanner(testData.source);
        std::for_each(
            testData.tokenTypes.begin(),
            testData.tokenTypes.end(),
            [&](const TokenType &expected)
            {
                auto actual = scanner.next();
                ASSERT_EQ(actual.type, expected);
                tokenCount++;
            });
        ASSERT_EQ(tokenCount, testData.tokenTypes.size());
        // Scanner should return EOF after the source is completely scanned.
        ASSERT_EQ(scanner.next().type, TokenType::TOKEN_EOF);
        ASSERT_EQ(scanner.next().type, TokenType::TOKEN_EOF);
        ASSERT_EQ(scanner.next().type, TokenType::TOKEN_EOF);
    }
}
