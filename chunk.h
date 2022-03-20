#ifndef __cpplox_chunk_h
#define __cpplox_chunk_h

#include "common.h"
#include "value.h"
#include "lineinfo.h"

typedef enum
{
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_RETURN,
} OpCode;

class Chunk
{
private:
    std::string name;
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    LineInfo lines;

public:
    Chunk(const std::string &name);
    void write(uint8_t byte, size_t line);
    uint8_t read(size_t offset) const { return code[offset]; }
    size_t writeConstant(Value value);
    Value readConstant(size_t offset) const { return constants[offset]; }
    size_t size() const { return code.size(); }

    void disassemble();

private:
    size_t disassembleInstruction(size_t offset);
    void printLineColumn(size_t offset);
};

#endif // __cpplox_chunk_h