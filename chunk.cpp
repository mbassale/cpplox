#include "chunk.h"

size_t simpleInstruction(const std::string &name, size_t offset);
size_t constantInstruction(const std::string &name, Chunk &chunk, size_t offset);
size_t constantLongInstruction(const std::string &name, Chunk &chunk, size_t offset);

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

void Chunk::disassemble()
{
    std::cout << "== " << name << " ==\n";
    for (size_t offset = 0; offset < size();)
    {
        offset = disassembleInstruction(offset);
    }
}

size_t Chunk::disassembleInstruction(size_t offset)
{
    std::cout << std::setfill('0') << std::setw(4) << offset << " ";

    printLineColumn(offset);

    uint8_t instruction = code[offset];
    switch (instruction)
    {
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", *this, offset);

    case OP_CONSTANT_LONG:
        return constantLongInstruction("OP_CONSTANT_LONG", *this, offset);

    case OP_ADD:
        return simpleInstruction("OP_ADD", offset);

    case OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);

    case OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);

    case OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);

    case OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);

    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);

    default:
        std::cout << "Unknown opcode: " << instruction << std::endl;
        return offset + 1;
    }
}

void Chunk::printLineColumn(size_t offset)
{
    std::cout << std::setfill('0') << std::setw(4) << lines.get(offset) << " ";
}

size_t simpleInstruction(const std::string &name, size_t offset)
{
    std::cout << name << std::endl;
    return offset + 1;
}

size_t constantInstruction(const std::string &name, Chunk &chunk, size_t offset)
{
    uint8_t constantOffset = chunk.read(offset + 1);
    std::cout << name << " " << std::setfill('0') << std::setw(4) << (size_t)constantOffset << " "
              << (std::string)chunk.readConstant(constantOffset) << std::endl;
    return offset + 2;
}

size_t constantLongInstruction(const std::string &name, Chunk &chunk, size_t offset)
{
    uint32bytes constantOffset = {0};
    constantOffset.bytes.b0 = chunk.read(offset + 1);
    constantOffset.bytes.b1 = chunk.read(offset + 2);
    constantOffset.bytes.b2 = chunk.read(offset + 3);
    std::cout << name << " " << std::setfill('0') << std::setw(4) << constantOffset.u32 << " "
              << (std::string)chunk.readConstant(constantOffset.u32) << std::endl;
    return offset + 4;
}