#include "chunk.h"

size_t simpleInstruction(const std::string &name, size_t offset);
size_t constantInstruction(const std::string &name, Chunk &chunk, size_t offset);
size_t constantLongInstruction(const std::string &name, Chunk &chunk, size_t offset);
size_t byteInstruction(const std::string &name, Chunk &chunk, size_t offset);
size_t jumpInstruction(const std::string &name, int sign, Chunk &chunk, size_t offset);

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

    case OP_NIL:
        return simpleInstruction("OP_NIL", offset);

    case OP_TRUE:
        return simpleInstruction("OP_TRUE", offset);

    case OP_FALSE:
        return simpleInstruction("OP_FALSE", offset);

    case OP_POP:
        return simpleInstruction("OP_POP", offset);

    case OP_GET_LOCAL:
        return byteInstruction("OP_GET_LOCAL", *this, offset);

    case OP_SET_LOCAL:
        return byteInstruction("OP_SET_LOCAL", *this, offset);

    case OP_DEFINE_GLOBAL:
        return constantInstruction("OP_DEFINE_GLOBAL", *this, offset);

    case OP_GET_GLOBAL:
        return simpleInstruction("OP_GET_GLOBAL", offset);

    case OP_SET_GLOBAL:
        return simpleInstruction("OP_SET_GLOBAL", offset);

    case OP_EQUAL:
        return simpleInstruction("OP_EQUAL", offset);

    case OP_GREATER:
        return simpleInstruction("OP_GREATER", offset);

    case OP_LESS:
        return simpleInstruction("OP_LESS", offset);

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

    case OP_NOT:
        return simpleInstruction("OP_NOT", offset);

    case OP_PRINT:
        return simpleInstruction("OP_PRINT", offset);

    case OP_JUMP:
        return jumpInstruction("OP_JUMP", 1, *this, offset);

    case OP_JUMP_IF_FALSE:
        return jumpInstruction("OP_JUMP_IF_FALSE", 1, *this, offset);

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

size_t byteInstruction(const std::string &name, Chunk &chunk, size_t offset)
{
    std::cout << name << " " << std::setfill('0') << std::setw(4) << offset << std::endl;
    return offset + 2;
}

size_t jumpInstruction(const std::string &name, int sign, Chunk &chunk, size_t offset)
{
    uint8_t *code = chunk.data();
    uint16_t jump = (uint16_t)(code[offset + 1] << 8);
    jump |= code[offset + 2];
    std::cout
        << name << " " << std::setfill('0') << std::setw(4) << offset << std::hex << jump << std::dec << std::endl;
    return offset + 3;
}