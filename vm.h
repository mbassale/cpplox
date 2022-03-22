#ifndef __cpplox_vm_h
#define __cpplox_vm_h

#include "common.h"
#include "chunk.h"

typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

const size_t STACK_MAX = 256;

class VM
{
    Chunk *chunk;
    uint8_t *ip;
    std::array<Value, STACK_MAX> stack;
    Value *stackTop;

public:
    explicit VM() : chunk{}, ip{}, stack() { stackTop = stack.data(); }
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
    inline void binaryOperator(uint8_t op)
    {
        const auto b = popStack();
        const auto a = popStack();
        switch (op)
        {
        case OP_ADD:
            pushStack(a + b);
            break;
        case OP_SUBTRACT:
            pushStack(a - b);
            break;
        case OP_MULTIPLY:
            pushStack(a * b);
            break;
        case OP_DIVIDE:
            pushStack(a / b);
            break;
        default:
            // TODO: throw runtime exception.
            break;
        };
    }

    inline void resetStack() { stackTop = stack.data(); }
    inline void pushStack(Value value)
    {
        *stackTop = value;
        stackTop++;
    }
    inline Value popStack()
    {
        stackTop--;
        return *stackTop;
    }
    inline bool isStackEmpty() { return stackTop == stack.data(); }
    void traceStack();
};

#endif // __cpplox_vm_h