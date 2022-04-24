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

struct Upvalue
{
    uint8_t index;
    bool isLocal;
};

struct CompilerConfig
{
    bool dumpTokens;
    bool disassembleInstructions;
};

class Compiler
{
private:
    Compiler *enclosing{nullptr};
    CompilerConfig config;
    ScannerPtr scanner{nullptr};
    FunctionPtr function{nullptr};
    Token current{};
    Token previous{};
    bool hadError{false};
    bool panicMode{false};
    std::list<std::string> errors{};
    std::vector<Local> locals{};
    std::vector<Upvalue> upvalues{};
    int scopeDepth{0};

public:
    explicit Compiler();
    explicit Compiler(const CompilerConfig &flags);
    explicit Compiler(Compiler &compiler);

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
    int parseVariable(const std::string &errorMessage);
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
    int resolveUpvalue(const Token &name);
    int addUpvalue(uint8_t index, bool isLocal);
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
    // [0] TOKEN_EMPTY
    {},
    // [1] TOKEN_LEFT_PAREN
    {&Compiler::grouping, &Compiler::call, PREC_CALL},
    // [2] TOKEN_RIGHT_PAREN
    {NULL, NULL, PREC_NONE},
    // [3] TOKEN_LEFT_BRACE
    {NULL, NULL, PREC_NONE},
    // [4] TOKEN_RIGHT_BRACE
    {NULL, NULL, PREC_NONE},
    // [5] TOKEN_COMMA
    {NULL, NULL, PREC_NONE},
    // [6] TOKEN_DOT
    {NULL, NULL, PREC_NONE},
    // [7] TOKEN_MINUS
    {&Compiler::unary, &Compiler::binary, PREC_TERM},
    // [8] TOKEN_PLUS
    {NULL, &Compiler::binary, PREC_TERM},
    // [9] TOKEN_SEMICOLON
    {NULL, NULL, PREC_NONE},
    // [10] TOKEN_SLASH
    {NULL, &Compiler::binary, PREC_FACTOR},
    // [11] TOKEN_STAR
    {NULL, &Compiler::binary, PREC_FACTOR},
    // [12] TOKEN_BANG
    {&Compiler::unary, NULL, PREC_FACTOR},
    // [13] TOKEN_BANG_EQUAL
    {NULL, &Compiler::binary, PREC_EQUALITY},
    // [14] TOKEN_EQUAL
    {NULL, NULL, PREC_NONE},
    // [15] TOKEN_EQUAL_EQUAL
    {NULL, &Compiler::binary, PREC_EQUALITY},
    // [16] TOKEN_GREATER
    {NULL, &Compiler::binary, PREC_COMPARISON},
    // [17] TOKEN_GREATER_EQUAL
    {NULL, &Compiler::binary, PREC_COMPARISON},
    // [18] TOKEN_LESS
    {NULL, &Compiler::binary, PREC_COMPARISON},
    // [19] TOKEN_LESS_EQUAL
    {NULL, &Compiler::binary, PREC_COMPARISON},
    // [20] TOKEN_IDENTIFIER
    {&Compiler::variable, NULL, PREC_NONE},
    // [21] TOKEN_STRING
    {&Compiler::string, NULL, PREC_NONE},
    // [22] TOKEN_NUMBER
    {&Compiler::number, NULL, PREC_NONE},
    // [23] TOKEN_AND
    {NULL, &Compiler::logicalAnd, PREC_AND},
    // [24] TOKEN_CLASS
    {NULL, NULL, PREC_NONE},
    // [25] TOKEN_ELSE
    {NULL, NULL, PREC_NONE},
    // [26] TOKEN_FALSE
    {&Compiler::literal, NULL, PREC_NONE},
    // [27] TOKEN_FOR
    {NULL, NULL, PREC_NONE},
    // [28] TOKEN_FUN
    {NULL, NULL, PREC_NONE},
    // [29] TOKEN_IF
    {NULL, NULL, PREC_NONE},
    // [30] TOKEN_NIL
    {&Compiler::literal, NULL, PREC_NONE},
    // [31] TOKEN_OR
    {NULL, &Compiler::logicalOr, PREC_OR},
    // [32] TOKEN_PRINT
    {NULL, NULL, PREC_NONE},
    // [33] TOKEN_RETURN
    {NULL, NULL, PREC_NONE},
    // [34] TOKEN_SUPER
    {NULL, NULL, PREC_NONE},
    // [35] TOKEN_THIS
    {NULL, NULL, PREC_NONE},
    // [36] TOKEN_TRUE
    {&Compiler::literal, NULL, PREC_NONE},
    // [37] TOKEN_VAR
    {NULL, NULL, PREC_NONE},
    // [38] TOKEN_WHILE
    {NULL, NULL, PREC_NONE},
    // [39] TOKEN_ERROR
    {NULL, NULL, PREC_NONE},
    // [40] TOKEN_EOF
    {NULL, NULL, PREC_NONE},
};

#endif // __cpplox_compiler_h