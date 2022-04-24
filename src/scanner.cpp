#include "scanner.h"

Token Token::make(const Scanner &scanner, TokenType type) {
  Token token(type);
  token.start = scanner.start;
  token.length = scanner.current - scanner.start;
  token.line = scanner.line;
  return token;
}

Token Token::makeError(const Scanner &scanner, const std::string &error) {
  Token token(TOKEN_ERROR);
  token.error = error;
  token.start = token.error.cbegin();
  token.length = token.error.length();
  token.line = scanner.line;
  return token;
}

Token Scanner::next() {
  skipWhitespaceAndComments();
  start = current;
  if (isAtEnd()) return Token::make(*this, TOKEN_EOF);

  const auto c = advance();
  if (isalpha(c) || c == '_') return scanIdentifier();
  if (isdigit(c)) return scanNumber();
  switch (c) {
    case '(':
      return Token::make(*this, TOKEN_LEFT_PAREN);
    case ')':
      return Token::make(*this, TOKEN_RIGHT_PAREN);
    case '{':
      return Token::make(*this, TOKEN_LEFT_BRACE);
    case '}':
      return Token::make(*this, TOKEN_RIGHT_BRACE);
    case ';':
      return Token::make(*this, TOKEN_SEMICOLON);
    case ',':
      return Token::make(*this, TOKEN_COMMA);
    case '.':
      return Token::make(*this, TOKEN_DOT);
    case '-':
      return Token::make(*this, TOKEN_MINUS);
    case '+':
      return Token::make(*this, TOKEN_PLUS);
    case '/':
      return Token::make(*this, TOKEN_SLASH);
    case '*':
      return Token::make(*this, TOKEN_STAR);

    case '!':
      return Token::make(*this, match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return Token::make(*this, match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return Token::make(*this, match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return Token::make(*this,
                         match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

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
  return Token::make(*this, TOKEN_STRING);
}

Token Scanner::scanNumber() {
  while (isdigit(peek())) advance();
  // fractional part
  if (peek() == '.' && isdigit(peekNext())) {
    // consume "."
    advance();
    while (isdigit(peek())) advance();
  }
  return Token::make(*this, TOKEN_NUMBER);
}

Token Scanner::scanIdentifier() {
  while (isalnum(peek()) || peek() == '_') advance();
  return Token::make(*this, identifierType());
}

TokenType Scanner::identifierType() {
  switch (*start) {
    case 'a':
      return checkKeyword(1, "nd", TOKEN_AND);
    case 'c':
      return checkKeyword(1, "lass", TOKEN_CLASS);
    case 'e':
      return checkKeyword(1, "lse", TOKEN_ELSE);
    case 'f':
      if (current - start > 1) {
        switch (*(start + 1)) {
          case 'a':
            return checkKeyword(2, "lse", TOKEN_FALSE);
          case 'o':
            return checkKeyword(2, "r", TOKEN_FOR);
          case 'u':
            return checkKeyword(2, "n", TOKEN_FUN);
        }
      }
    case 'i':
      return checkKeyword(1, "f", TOKEN_IF);
    case 'n':
      return checkKeyword(1, "il", TOKEN_NIL);
    case 'o':
      return checkKeyword(1, "r", TOKEN_OR);
    case 'p':
      return checkKeyword(1, "rint", TOKEN_PRINT);
    case 'r':
      return checkKeyword(1, "eturn", TOKEN_RETURN);
    case 's':
      return checkKeyword(1, "uper", TOKEN_SUPER);
    case 't':
      if (current - start > 1) {
        switch (*(start + 1)) {
          case 'h':
            return checkKeyword(2, "is", TOKEN_THIS);
          case 'r':
            return checkKeyword(2, "ue", TOKEN_TRUE);
        }
      }
    case 'v':
      return checkKeyword(1, "ar", TOKEN_VAR);
    case 'w':
      return checkKeyword(1, "hile", TOKEN_WHILE);
    default:
      break;
  }
  return TOKEN_IDENTIFIER;
}

TokenType Scanner::checkKeyword(size_t offset, const std::string &rest,
                                TokenType type) {
  std::string lexeme(start + offset, start + offset + rest.length());
  if (lexeme == rest) {
    return type;
  }
  return TOKEN_IDENTIFIER;
}