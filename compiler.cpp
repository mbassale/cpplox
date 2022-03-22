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
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
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

void Compiler::expression()
{
}

void Compiler::emitByte(uint8_t byte)
{
    chunk->write(byte, previous.line);
}

void Compiler::emitEpilogue()
{
    emitReturn();
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