#include "chunk.h"

size_t simpleInstruction(const std::string &name, size_t offset);

Chunk::Chunk(const std::string &name) : name(name), code()
{
}

void Chunk::write(uint8_t byte)
{
    code.push_back(byte);
}

void Chunk::disassemble()
{
    std::cout << "== " << name << " ==\n";
    for (size_t offset = 0; offset < size(); offset++)
    {
        offset = disassembleInstruction(offset);
    }
}

size_t Chunk::disassembleInstruction(size_t offset)
{
    std::cout << std::setfill('0') << std::setw(4) << offset << " ";
    uint8_t instruction = code[offset];
    switch (instruction)
    {
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);

    default:
        std::cout << "Unknown opcode: " << instruction << std::endl;
        return offset + 1;
    }
}

size_t simpleInstruction(const std::string &name, size_t offset)
{
    std::cout << name << std::endl;
    return offset + 1;
}