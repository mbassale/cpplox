#ifndef __cpplox_compiler_h
#define __cpplox_compiler_h

#include "common.h"
#include "scanner.h"
#include "chunk.h"

class CompilerError : public std::runtime_error
{
public:
    CompilerError(const std::string &message) : std::runtime_error(message) {}
};

enum Precedence
{
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
};

class Compiler;
typedef void (Compiler::*ParseFn)();

struct ParseRule
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};

class Compiler
{
private:
    ScannerUniquePtr scanner{};
    ChunkPtr chunk{};
    Token current{};
    Token previous{};
    bool hadError;
    bool panicMode;
    std::list<std::string> errors;

public:
    explicit Compiler();

    ChunkPtr compile(const std::string &source);
    inline bool hasErrors() const { return !errors.empty(); }
    inline const std::list<std::string> &getErrors() const { return errors; }

    void declaration();
    void varDeclarationStatement();
    void statement();
    void printStatement();
    void expressionStatement();
    void expression();
    void number();
    void grouping();
    void unary();
    void binary();
    void literal();
    void string();

private:
    void advance();
    bool match(TokenType type);
    bool check(TokenType type);
    void parsePrecedence(Precedence precedence);
    size_t parseVariable(const std::string &errorMessage);
    size_t identifierConstant(const Token &name);
    ParseRule &getRule(TokenType tokenType);
    void consume(TokenType tokenType, const std::string &errorMessage);
    void defineVariable(size_t global);
    void emitByte(uint8_t byte);
    void emitBytes(uint8_t byte1, uint8_t byte2);
    size_t makeConstant(Value value);
    void emitEpilogue();
    void emitConstant(Value value);
    void emitReturn();

    void synchronize();
    void error(const std::string &message);
    void errorAtCurrent(const std::string &message);
    void errorAt(Token &token, const std::string &message);
};

static ParseRule PARSE_RULES[] = {
    [TOKEN_LEFT_PAREN] = {&Compiler::grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {&Compiler::unary, &Compiler::binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, &Compiler::binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, &Compiler::binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, &Compiler::binary, PREC_FACTOR},
    [TOKEN_BANG] = {&Compiler::unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, &Compiler::binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, &Compiler::binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, &Compiler::binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, &Compiler::binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, &Compiler::binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, &Compiler::binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOKEN_STRING] = {&Compiler::string, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {&Compiler::number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {&Compiler::literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {&Compiler::literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {&Compiler::literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

#endif // __cpplox_compiler_h