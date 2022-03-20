#ifndef __cpplox_vm_h
#define __cpplox_vm_h

#include "chunk.h"

typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

class VM
{
    Chunk *chunk;
    uint8_t *ip;

public:
    explicit VM() {}
    ~VM() {}

    InterpretResult interpret(Chunk &chunk);

private:
    InterpretResult run();
    inline uint8_t readByte() { return *ip++; }
    inline Value readConstant() { return chunk->readConstant(readByte()); }
    inline Value readConstantLong()
    {
        uint32bytes constantOffset = {0};
        constantOffset.bytes.b0 = readByte();
        constantOffset.bytes.b1 = readByte();
        constantOffset.bytes.b2 = readByte();
        return chunk->readConstant(constantOffset.u32);
    }
};

#endif // __cpplox_vm_h