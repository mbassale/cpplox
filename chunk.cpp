#include "chunk.h"

size_t simpleInstruction(const std::string &name, size_t offset);
size_t constantInstruction(const std::string &name, Chunk &chunk, size_t offset);

Chunk::Chunk(const std::string &name) : name(name), code(), constants()
{
}

void Chunk::write(uint8_t byte)
{
    code.push_back(byte);
}

uint8_t Chunk::writeConstant(Value value)
{
    constants.push_back(value);
    return (uint8_t)(constants.size() - 1);
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
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", *this, offset);

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

size_t constantInstruction(const std::string &name, Chunk &chunk, size_t offset)
{
    uint8_t constantOffset = chunk.read(offset);
    std::cout << name << " " << std::setfill('0') << std::setw(4) << (size_t)constantOffset << " "
              << chunk.readConstant(constantOffset) << std::endl;
    return offset + 2;
}