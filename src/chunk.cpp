#include "chunk.h"

Chunk::Chunk(const std::string &name) : name(name), code(), constants(), lines()
{
}

void Chunk::write(uint8_t byte, size_t line)
{
    code.push_back(byte);
    lines.write(line);
}

size_t Chunk::writeConstant(Value value)
{
    constants.push_back(value);
    return constants.size() - 1;
}
