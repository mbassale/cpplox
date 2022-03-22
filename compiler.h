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

private:
    void advance();
    void expression();
    void consume(TokenType tokenType, const std::string &errorMessage);
    void emitByte(uint8_t byte);
    void emitEpilogue();
    void emitReturn();

    void error(const std::string &message);
    void errorAtCurrent(const std::string &message);
    void errorAt(Token &token, const std::string &message);
};

#endif // __cpplox_compiler_h