#ifndef __cpplox_vm_h
#define __cpplox_vm_h

#include "common.h"
#include "debug.h"
#include "chunk.h"
#include "object.h"
#include "function.h"
#include "native.h"
#include "closure.h"

typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

const size_t FRAMES_MAX = 256;
const size_t STACK_MAX = FRAMES_MAX * (UINT8_MAX + 1);

struct CallFrame
{
public:
    ClosurePtr closure; // compiled function obj
    uint8_t *ip;        // return address
    Value *fp;          // frame pointer

    CallFrame() : closure(nullptr), ip(nullptr), fp(nullptr) {}
    inline void reset()
    {
        closure.reset();
        ip = nullptr;
        fp = nullptr;
    }
    inline Function &getFunction() const { return *(closure->getFunction()); }
    inline Chunk &getChunk() const { return getFunction().getChunk(); }
};

class VMRuntimeError : public std::runtime_error
{
public:
    VMRuntimeError(const std::string &message) : std::runtime_error(message) {}
};

class VM
{
    std::array<Value, STACK_MAX> stack;
    std::array<CallFrame, FRAMES_MAX> frames;
    size_t frameCount;
    Value *stackTop;
    std::forward_list<ObjectPtr> objects;
    std::unordered_map<Symbol, Value> globals;

public:
    explicit VM();
    ~VM();

    InterpretResult interpret(FunctionPtr function);
    void defineNative(const std::string &name, NativeFnPtr function);

private:
    InterpretResult run();

    inline CallFrame &pushFrame(ClosurePtr closure, int argCount)
    {
        frameCount++;
        auto &frame = frames[frameCount - 1];
        frame.ip = closure->getChunk().data();
        frame.closure = closure;
        frame.fp = stackTop - argCount - 1;
        return frame;
    }
    inline bool callValue(Value callee, int argCount);
    inline bool call(ClosurePtr closure, int argCount);
    inline void popFrame()
    {
        stackTop = getFrame().fp;
        frameCount--;
        if (frameCount >= 0)
        {
            auto &frame = frames[frameCount];
            frame.reset();
        }
    }
    inline CallFrame &getFrame() { return frames[frameCount - 1]; }
    inline Chunk &getChunk() { return getFrame().getChunk(); }

    inline uint8_t readByte() { return *getFrame().ip++; }
    inline uint16_t readShort()
    {
        auto &frame = getFrame();
        frame.ip += 2;
        return (uint16_t)(frame.ip[-2] << 8 | frame.ip[-1]);
    }
    inline Value &readConstant() { return getChunk().readConstant(readByte()); }
    inline void incrIp(size_t offset) { getFrame().ip += offset; }
    inline void decrIp(size_t offset) { getFrame().ip -= offset; }
    inline void binaryOperator(uint8_t op);
    inline void resetStack() { stackTop = stack.data(); }
    inline void pushStack(const Value &value)
    {
        *stackTop = value;
        stackTop++;
    }
    inline void pushStack(FunctionPtr function)
    {
        auto object = std::dynamic_pointer_cast<Object>(function);
        pushStack(Value(object));
    }
    inline void pushStack(ClosurePtr closure)
    {
        auto object = std::dynamic_pointer_cast<Object>(closure);
        pushStack(Value(object));
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

    void traceInstruction();
    void traceStack();
    void traceGlobals();
    void runtimeError(const std::string &message);
};

#endif // __cpplox_vm_h