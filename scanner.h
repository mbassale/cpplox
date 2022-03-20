#ifndef __cpplox_scanner_h
#define __cpplox_scanner_h

#include "common.h"

enum TokenType
{
    // Single-character tokens.
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_SLASH,
    TOKEN_STAR,
    // One or two character tokens.
    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    // Literals.
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    // Keywords.
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

    TOKEN_ERROR,
    TOKEN_EOF
};

struct Token
{
public:
    TokenType type;
    std::string::const_iterator start;
    size_t length;
    size_t line;

    Token(TokenType type, std::string::const_iterator start, size_t length, size_t line)
        : type(type), start(start), length(length), line(line) {}
};

class Scanner
{
    const std::string &source;
    std::string::const_iterator start;
    std::string::const_iterator current;
    size_t line;

public:
    explicit Scanner(const std::string &source) : source(source)
    {
        start = source.begin();
        current = source.begin();
        line = 1;
    }

    Token next()
    {
        return Token(TokenType::TOKEN_EOF, source.cend(), 1, line);
    }
};

#endif // __cpplox_scanner_h