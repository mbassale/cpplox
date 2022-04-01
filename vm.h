#ifndef __cpplox_vm_h
#define __cpplox_vm_h

#include "common.h"
#include "chunk.h"
#include "object.h"

typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

const size_t STACK_MAX = 256;

class VMRuntimeError : public std::runtime_error
{
public:
    VMRuntimeError(const std::string &message) : std::runtime_error(message) {}
};

class VM
{
    Chunk *chunk;
    uint8_t *ip;
    std::array<Value, STACK_MAX> stack;
    Value *stackTop;
    std::forward_list<ObjectPtr> objects;
    std::unordered_map<Symbol, Value> globals;

public:
    explicit VM();
    ~VM();

    InterpretResult interpret(Chunk &chunk);

private:
    InterpretResult run();

    inline uint8_t readByte() { return *ip++; }
    inline uint16_t readShort()
    {
        ip += 2;
        return (uint16_t)(ip[-2] << 8 | ip[-1]);
    }
    inline Value &readConstant() { return chunk->readConstant(readByte()); }
    inline Value &readConstantLong()
    {
        uint32bytes constantOffset = {0};
        constantOffset.bytes.b0 = readByte();
        constantOffset.bytes.b1 = readByte();
        constantOffset.bytes.b2 = readByte();
        return chunk->readConstant(constantOffset.u32);
    }
    inline void binaryOperator(uint8_t op);
    inline void resetStack() { stackTop = stack.data(); }
    inline void pushStack(const Value &value)
    {
        *stackTop = value;
        stackTop++;
    }
    inline Value &popStack()
    {
        stackTop--;
        return *stackTop;
    }
    inline bool isStackEmpty() { return stackTop == stack.data(); }
    inline Value &peekStack(size_t distance) const
    {
        return stackTop[-1 - distance];
    }
    void traceStack();
    void traceGlobals();
};

#endif // __cpplox_vm_h