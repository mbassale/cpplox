#ifndef __cpplox_compiler_h
#define __cpplox_compiler_h

#include "common.h"
#include "scanner.h"
#include "chunk.h"
#include "function.h"

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

struct ParseFnArgs
{
    bool canAssign = true;
};

typedef void (Compiler::*ParseFn)(const ParseFnArgs &args);

struct ParseRule
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};

struct Local
{
    Token name;
    int depth;
};

struct CompilerConfig
{
    bool dumpTokens;
    bool disassembleInstructions;
};

class Compiler
{
private:
    CompilerConfig config;
    ScannerPtr scanner{nullptr};
    FunctionPtr function{nullptr};
    Token current{};
    Token previous{};
    bool hadError{false};
    bool panicMode{false};
    std::list<std::string> errors{};
    std::vector<Local> locals{};
    int scopeDepth{0};

public:
    explicit Compiler();
    explicit Compiler(const CompilerConfig &flags);
    explicit Compiler(const Compiler &compiler);

    FunctionPtr compile(const std::string &name, const std::string &source);
    FunctionPtr compileFunction(Compiler &compiler, FunctionType &functionType, const std::string &name);
    inline bool hasErrors() const { return !errors.empty(); }
    inline const std::list<std::string> &getErrors() const { return errors; }

    void declaration(const ParseFnArgs &args);
    void funDeclaration(const ParseFnArgs &args);
    void varDeclarationStatement(const ParseFnArgs &args);
    void statement(const ParseFnArgs &args);
    void printStatement(const ParseFnArgs &args);
    void returnStatement(const ParseFnArgs &args);
    void forStatement(const ParseFnArgs &args);
    void ifStatement(const ParseFnArgs &args);
    void whileStatement(const ParseFnArgs &args);
    void expressionStatement(const ParseFnArgs &args);
    void expression(const ParseFnArgs &args);
    void number(const ParseFnArgs &args);
    void grouping(const ParseFnArgs &args);
    void unary(const ParseFnArgs &args);
    void binary(const ParseFnArgs &args);
    void call(const ParseFnArgs &args);
    void literal(const ParseFnArgs &args);
    void string(const ParseFnArgs &args);
    void variable(const ParseFnArgs &args);
    void logicalAnd(const ParseFnArgs &args);
    void logicalOr(const ParseFnArgs &args);

private:
    inline Chunk &currentChunk() { return function->getChunk(); }
    void advance();
    bool match(TokenType type);
    bool check(TokenType type);
    void parsePrecedence(Precedence precedence);
    size_t parseVariable(const std::string &errorMessage);
    size_t identifierConstant(const Token &name);
    void namedVariable(const Token &name, bool canAssign);
    ParseRule &getRule(TokenType tokenType);
    void consume(TokenType tokenType, const std::string &errorMessage);
    void defineVariable(size_t global);
    uint8_t argumentList(const ParseFnArgs &args);
    void declareVariable();
    void markInitialized();
    void addLocal(const Token &name);
    int resolveLocal(const Token &name);
    void beginScope();
    void block();
    void compileDefinition(FunctionType type, const std::string &name);
    void endScope();
    void emitByte(uint8_t byte);
    void emitBytes(uint8_t byte1, uint8_t byte2);
    size_t makeConstant(Value value);
    void emitEpilogue();
    void emitConstant(Value value);
    void emitReturn();
    size_t emitJump(uint8_t instruction);
    void patchJump(size_t offset);
    void emitLoop(size_t loopStart);

    void synchronize();
    void error(const std::string &message);
    void errorAtCurrent(const std::string &message);
    void errorAt(Token &token, const std::string &message);
};

static ParseRule PARSE_RULES[] = {
    {},
    {&Compiler::grouping, &Compiler::call, PREC_CALL},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {&Compiler::unary, &Compiler::binary, PREC_TERM},
    {NULL, &Compiler::binary, PREC_TERM},
    {NULL, NULL, PREC_NONE},
    {NULL, &Compiler::binary, PREC_FACTOR},
    {&Compiler::unary, NULL, PREC_NONE},
    {NULL, &Compiler::binary, PREC_FACTOR},
    {NULL, &Compiler::binary, PREC_EQUALITY},
    {NULL, NULL, PREC_NONE},
    {NULL, &Compiler::binary, PREC_EQUALITY},
    {NULL, &Compiler::binary, PREC_COMPARISON},
    {NULL, &Compiler::binary, PREC_COMPARISON},
    {NULL, &Compiler::binary, PREC_COMPARISON},
    {NULL, &Compiler::binary, PREC_COMPARISON},
    {&Compiler::variable, NULL, PREC_NONE},
    {&Compiler::string, NULL, PREC_NONE},
    {&Compiler::number, NULL, PREC_NONE},
    {NULL, &Compiler::logicalAnd, PREC_AND},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {&Compiler::literal, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {&Compiler::literal, NULL, PREC_NONE},
    {NULL, &Compiler::logicalOr, PREC_OR},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {&Compiler::literal, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
    {NULL, NULL, PREC_NONE},
};

#endif // __cpplox_compiler_h