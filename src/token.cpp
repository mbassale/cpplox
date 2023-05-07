#include "token.h"

bool operator==(const Token &lhs, const Token &rhs) { return lhs.isEqual(rhs); }

Token Token::make(TokenType type) {
  switch (type) {
    case TokenType::TOKEN_LEFT_PAREN:
      return Token(type, "(");
    case TokenType::TOKEN_RIGHT_PAREN:
      return Token(type, ")");
    case TokenType::TOKEN_LEFT_BRACE:
      return Token(type, "{");
    case TokenType::TOKEN_RIGHT_BRACE:
      return Token(type, "}");
    case TokenType::TOKEN_LEFT_BRACKET:
      return Token(type, "[");
    case TokenType::TOKEN_RIGHT_BRACKET:
      return Token(type, "]");
    case TokenType::TOKEN_COMMA:
      return Token(type, ",");
    case TokenType::TOKEN_DOT:
      return Token(type, ".");
    case TokenType::TOKEN_MINUS:
      return Token(type, "-");
    case TokenType::TOKEN_PLUS:
      return Token(type, "+");
    case TokenType::TOKEN_SEMICOLON:
      return Token(type, ";");
    case TokenType::TOKEN_SLASH:
      return Token(type, "/");
    case TokenType::TOKEN_STAR:
      return Token(type, "*");
    case TokenType::TOKEN_BANG:
      return Token(type, "!");
    case TokenType::TOKEN_BANG_EQUAL:
      return Token(type, "!=");
    case TokenType::TOKEN_EQUAL:
      return Token(type, "=");
    case TokenType::TOKEN_EQUAL_EQUAL:
      return Token(type, "==");
    case TokenType::TOKEN_GREATER:
      return Token(type, ">");
    case TokenType::TOKEN_GREATER_EQUAL:
      return Token(type, ">=");
    case TokenType::TOKEN_LESS:
      return Token(type, "<");
    case TokenType::TOKEN_LESS_EQUAL:
      return Token(type, "<=");
    case TokenType::TOKEN_AND:
      return Token(type, "and");
    case TokenType::TOKEN_OR:
      return Token(type, "or");
    default: {
      std::string error =
          std::string("Invalid TokenType: ") + std::to_string((int)type);
      throw std::invalid_argument(error);
    }
  }
}
