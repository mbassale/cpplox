#ifndef __cpplox_scanner_h
#define __cpplox_scanner_h

#include "common.h"

enum TokenType
{
    TOKEN_EMPTY,

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

class Scanner;

struct Token
{
public:
    TokenType type;
    std::string::const_iterator start;
    size_t length;
    size_t line;
    std::string error;

    explicit Token() : type(TOKEN_EMPTY) {}
    explicit Token(TokenType type) : type(type), start(), length(), line() {}
    explicit Token(TokenType type, std::string::const_iterator start, size_t length, size_t line)
        : type(type), start(start), length(length), line(line) {}
    const std::string lexeme() const { return std::string(start, start + length); }
    const std::string str() const
    {
        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(4) << line << " "
           << std::setfill('0') << std::setw(2) << type
           << " " << lexeme();
        return ss.str();
    }

    static Token make(const Scanner &scanner, TokenType type);
    static Token makeError(const Scanner &scanner, const std::string &error);
};

class Scanner
{
    const std::string source;
    std::string::const_iterator start;
    std::string::const_iterator current;
    size_t line;

public:
    explicit Scanner(const std::string &source) : source(source)
    {
        start = this->source.begin();
        current = this->source.begin();
        line = 1;
    }

    Token next();

private:
    friend class Token;
    inline bool isAtEnd() { return current == source.cend(); }
    inline char advance()
    {
        current++;
        return *(current - 1);
    }
    inline bool match(char expected)
    {
        if (isAtEnd())
            return false;
        if (*current != expected)
            return false;
        current++;
        return true;
    }
    inline char peek() { return *current; }
    inline char peekNext()
    {
        if (isAtEnd())
            return '\0';
        return *(current + 1);
    }
    void skipWhitespaceAndComments();
    Token scanString();
    Token scanNumber();
    Token scanIdentifier();
    TokenType identifierType();
    TokenType checkKeyword(size_t offset, const std::string &rest, TokenType type);
};

typedef std::shared_ptr<Scanner> ScannerPtr;

#endif // __cpplox_scanner_h