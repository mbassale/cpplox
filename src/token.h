#pragma once

#include "common.h"

enum class TokenType {
  TOKEN_EMPTY,

  // Single-character tokens.
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_LEFT_BRACKET,
  TOKEN_RIGHT_BRACKET,
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
  TOKEN_BREAK,
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

struct Token {
 public:
  TokenType type;
  std::string::const_iterator start;
  size_t length;
  size_t line;
  std::string error;
  std::string lexeme_;

  explicit Token() : type(TokenType::TOKEN_EMPTY) {}
  explicit Token(TokenType type) : type(type), start(), length(), line() {}
  explicit Token(TokenType type, const std::string &lexeme)
      : type(type), lexeme_(lexeme), line(1) {
    start = lexeme_.cbegin();
    length = lexeme_.size();
  }
  explicit Token(TokenType type, std::string::const_iterator start,
                 size_t length, size_t line)
      : type(type), start(start), length(length), line(line) {
    lexeme_ = std::string(start, start + length);
  }
  const std::string lexeme() const { return lexeme_; }
  const std::string str() const {
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(4) << line << " " << std::setfill('0')
       << std::setw(2) << (int)type << " " << lexeme();
    return ss.str();
  }

  bool isEqual(const Token &token) const {
    return this->type == token.type && this->lexeme() == token.lexeme();
  }

  static Token make(TokenType type);
};

bool operator==(const Token &lhs, const Token &rhs);
