#include "compiler.h"

Compiler::Compiler()
{
    hadError = false;
    panicMode = false;
}

ChunkPtr Compiler::compile(const std::string &source)
{
    chunk = std::make_shared<Chunk>("<name>");
    scanner = std::make_unique<Scanner>(source);
    advance();

    while (!match(TOKEN_EOF))
    {
        declaration();
    }

    emitEpilogue();
    return hadError ? nullptr : chunk;
}

void Compiler::advance()
{
    previous = current;
    for (;;)
    {
        current = scanner->next();
        std::cout << current.str() << std::endl;
        if (current.type != TOKEN_ERROR)
            break;
        errorAtCurrent(current.lexeme());
    }
}

bool Compiler::match(TokenType type)
{
    if (!check(type))
        return false;
    advance();
    return true;
}
bool Compiler::check(TokenType type)
{
    return current.type == type;
}

void Compiler::declaration()
{
    statement();
}

void Compiler::statement()
{
    if (match(TOKEN_PRINT))
    {
        printStatement();
    }
    else
    {
        expressionStatement();
    }
}

void Compiler::printStatement()
{
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

void Compiler::expressionStatement()
{
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

void Compiler::expression()
{
    parsePrecedence(PREC_ASSIGNMENT);
}

void Compiler::number()
{
    double value = std::strtod(previous.lexeme().c_str(), nullptr);
    emitConstant(Value(value));
}

void Compiler::grouping()
{
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::unary()
{
    const auto operatorType = previous.type;

    // Compile the operand.
    parsePrecedence(PREC_UNARY);

    // Emit the operator instruction.
    switch (operatorType)
    {
    case TOKEN_BANG:
        emitByte(OP_NOT);
        break;
    case TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;

    default: // Unreachable.
        return;
    }
}

void Compiler::binary()
{
    const auto operatorType = previous.type;
    const auto rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule.precedence + 1));

    switch (operatorType)
    {
    case TOKEN_BANG_EQUAL:
        emitBytes(OP_EQUAL, OP_NOT);
        break;
    case TOKEN_EQUAL_EQUAL:
        emitByte(OP_EQUAL);
        break;
    case TOKEN_GREATER:
        emitByte(OP_GREATER);
        break;
    case TOKEN_GREATER_EQUAL:
        emitBytes(OP_LESS, OP_NOT);
        break;
    case TOKEN_LESS:
        emitByte(OP_LESS);
        break;
    case TOKEN_LESS_EQUAL:
        emitBytes(OP_GREATER, OP_NOT);
        break;
    case TOKEN_PLUS:
        emitByte(OP_ADD);
        break;
    case TOKEN_MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TOKEN_STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TOKEN_SLASH:
        emitByte(OP_DIVIDE);
        break;
    default: // Unreachable.
        break;
    }
}

void Compiler::literal()
{
    switch (previous.type)
    {
    case TOKEN_FALSE:
        emitByte(OP_FALSE);
        break;
    case TOKEN_NIL:
        emitByte(OP_NIL);
        break;
    case TOKEN_TRUE:
        emitByte(OP_TRUE);
        break;
    default: // Unreachable.
        return;
    }
}

void Compiler::string()
{
    emitConstant(Value(std::string(previous.start + 1, previous.start + previous.length - 1)));
}

void Compiler::parsePrecedence(Precedence precedence)
{
    advance();
    const auto prefixRule = getRule(previous.type).prefix;
    if (prefixRule == nullptr)
    {
        error("Expect expression.");
        return;
    }
    std::invoke(prefixRule, *this);

    while (precedence <= getRule(current.type).precedence)
    {
        advance();
        const auto infixRule = getRule(previous.type).infix;
        std::invoke(infixRule, *this);
    }
}

ParseRule &Compiler::getRule(TokenType tokenType)
{
    return PARSE_RULES[tokenType];
}

void Compiler::emitByte(uint8_t byte)
{
    chunk->write(byte, previous.line);
}

void Compiler::emitBytes(uint8_t byte1, uint8_t byte2)
{
    chunk->write(byte1);
    chunk->write(byte2);
}

size_t Compiler::makeConstant(Value value)
{
    const auto constantOffset = chunk->writeConstant(value);
    // TODO: support OP_CONSTANT_LONG opcode.
    if (constantOffset > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }
    return constantOffset;
}

void Compiler::emitEpilogue()
{
    emitReturn();
}

void Compiler::emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

void Compiler::emitReturn()
{
    emitByte(OP_RETURN);
}

void Compiler::consume(TokenType tokenType, const std::string &errorMessage)
{
    if (current.type == tokenType)
    {
        advance();
        return;
    }
    errorAtCurrent(errorMessage);
}

void Compiler::error(const std::string &message)
{
    errorAt(previous, message);
}

void Compiler::errorAtCurrent(const std::string &message)
{
    errorAt(current, message);
}

void Compiler::errorAt(Token &token, const std::string &message)
{
    if (panicMode)
        return;
    panicMode = true;
    std::ostringstream ss;
    ss << "[line " << token.line << "] Error";
    if (token.type == TOKEN_EOF)
    {
        ss << " at end";
    }
    else if (token.type == TOKEN_ERROR)
    {
        // omit
    }
    else
    {
        ss << " at " << token.length;
    }

    ss << ": " << message;
    errors.push_back(ss.str());
}