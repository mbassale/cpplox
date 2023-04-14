#include "scanner.h"

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

Token Token::make(const Scanner &scanner, TokenType type) {
  const auto length = scanner.current - scanner.start;
  return Token(type, scanner.start, length, scanner.line);
}

Token Token::makeError(const Scanner &scanner, const std::string &error) {
  Token token(TokenType::TOKEN_ERROR);
  token.error = error;
  token.start = token.error.cbegin();
  token.length = token.error.length();
  token.line = scanner.line;
  return token;
}

Token Scanner::next() {
  skipWhitespaceAndComments();
  start = current;
  if (isAtEnd()) return Token::make(*this, TokenType::TOKEN_EOF);

  const auto c = advance();
  if (isalpha(c) || c == '_') return scanIdentifier();
  if (isdigit(c)) return scanNumber();
  switch (c) {
    case '(':
      return Token::make(*this, TokenType::TOKEN_LEFT_PAREN);
    case ')':
      return Token::make(*this, TokenType::TOKEN_RIGHT_PAREN);
    case '{':
      return Token::make(*this, TokenType::TOKEN_LEFT_BRACE);
    case '}':
      return Token::make(*this, TokenType::TOKEN_RIGHT_BRACE);
    case '[':
      return Token::make(*this, TokenType::TOKEN_LEFT_BRACKET);
    case ']':
      return Token::make(*this, TokenType::TOKEN_RIGHT_BRACKET);
    case ';':
      return Token::make(*this, TokenType::TOKEN_SEMICOLON);
    case ',':
      return Token::make(*this, TokenType::TOKEN_COMMA);
    case '.':
      return Token::make(*this, TokenType::TOKEN_DOT);
    case '-':
      return Token::make(*this, TokenType::TOKEN_MINUS);
    case '+':
      return Token::make(*this, TokenType::TOKEN_PLUS);
    case '/':
      return Token::make(*this, TokenType::TOKEN_SLASH);
    case '*':
      return Token::make(*this, TokenType::TOKEN_STAR);

    case '!':
      return Token::make(*this, match('=') ? TokenType::TOKEN_BANG_EQUAL
                                           : TokenType::TOKEN_BANG);
    case '=':
      return Token::make(*this, match('=') ? TokenType::TOKEN_EQUAL_EQUAL
                                           : TokenType::TOKEN_EQUAL);
    case '<':
      return Token::make(*this, match('=') ? TokenType::TOKEN_LESS_EQUAL
                                           : TokenType::TOKEN_LESS);
    case '>':
      return Token::make(*this, match('=') ? TokenType::TOKEN_GREATER_EQUAL
                                           : TokenType::TOKEN_GREATER);

    case '"':
      return scanString();
  }

  return Token::makeError(*this, "Unexpected character");
}

void Scanner::skipWhitespaceAndComments() {
  for (;;) {
    const auto c = peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
      case '\n':
        line++;
        advance();
        break;
      case '/':
        if (peekNext() == '/') {
          while (peek() != '\n' && !isAtEnd()) {
            advance();
          }
        } else {
          return;
        }
        break;
      default:
        return;
    }
  }
}

Token Scanner::scanString() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') line++;
    advance();
  }
  if (isAtEnd()) return Token::makeError(*this, "Unternminated string");

  advance();
  return Token::make(*this, TokenType::TOKEN_STRING);
}

Token Scanner::scanNumber() {
  while (isdigit(peek())) advance();
  // fractional part
  if (peek() == '.' && isdigit(peekNext())) {
    // consume "."
    advance();
    while (isdigit(peek())) advance();
  }
  return Token::make(*this, TokenType::TOKEN_NUMBER);
}

Token Scanner::scanIdentifier() {
  while (isalnum(peek()) || peek() == '_') advance();
  return Token::make(*this, identifierType());
}

TokenType Scanner::identifierType() {
  switch (*start) {
    case 'a':
      return checkKeyword(1, "nd", TokenType::TOKEN_AND);
    case 'b':
      return checkKeyword(1, "reak", TokenType::TOKEN_BREAK);
    case 'c':
      return checkKeyword(1, "lass", TokenType::TOKEN_CLASS);
    case 'e':
      return checkKeyword(1, "lse", TokenType::TOKEN_ELSE);
    case 'f':
      if (current - start > 1) {
        switch (*(start + 1)) {
          case 'a':
            return checkKeyword(2, "lse", TokenType::TOKEN_FALSE);
          case 'o':
            return checkKeyword(2, "r", TokenType::TOKEN_FOR);
          case 'u':
            return checkKeyword(2, "n", TokenType::TOKEN_FUN);
        }
      }
    case 'i':
      return checkKeyword(1, "f", TokenType::TOKEN_IF);
    case 'n':
      return checkKeyword(1, "il", TokenType::TOKEN_NIL);
    case 'o':
      return checkKeyword(1, "r", TokenType::TOKEN_OR);
    case 'p':
      return checkKeyword(1, "rint", TokenType::TOKEN_PRINT);
    case 'r':
      return checkKeyword(1, "eturn", TokenType::TOKEN_RETURN);
    case 's':
      return checkKeyword(1, "uper", TokenType::TOKEN_SUPER);
    case 't':
      if (current - start > 1) {
        switch (*(start + 1)) {
          case 'h':
            return checkKeyword(2, "is", TokenType::TOKEN_THIS);
          case 'r':
            return checkKeyword(2, "ue", TokenType::TOKEN_TRUE);
        }
      }
    case 'v':
      return checkKeyword(1, "ar", TokenType::TOKEN_VAR);
    case 'w':
      return checkKeyword(1, "hile", TokenType::TOKEN_WHILE);
    default:
      break;
  }
  return TokenType::TOKEN_IDENTIFIER;
}

TokenType Scanner::checkKeyword(size_t offset, const std::string &rest,
                                TokenType type) {
  const auto end = start + offset + rest.length();
  if (end < source.end()) {
    std::string lexeme(start + offset, end);
    if (lexeme == rest) {
      return type;
    }
  }
  return TokenType::TOKEN_IDENTIFIER;
}