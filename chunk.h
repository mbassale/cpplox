#ifndef __cpplox_chunk_h
#define __cpplox_chunk_h

#include "common.h"

typedef enum
{
    OP_RETURN,
} OpCode;

class Chunk
{
private:
    std::string name;
    std::vector<uint8_t> code;

public:
    Chunk(const std::string &name);
    void write(uint8_t byte);
    size_t size() { return code.size(); }

    void disassemble();

private:
    size_t disassembleInstruction(size_t offset);
};

#endif // __cpplox_chunk_h