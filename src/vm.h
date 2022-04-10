#ifndef __cpplox_vm_h
#define __cpplox_vm_h

#include "common.h"
#include "chunk.h"
#include "object.h"
#include "function.h"

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
    FunctionPtr function; // compiled function obj
    uint8_t *ip;          // return address
    Value *fp;            // frame pointer

    CallFrame() : function(nullptr), ip(nullptr), fp(nullptr) {}
    inline void reset()
    {
        function.reset();
        ip = nullptr;
        fp = nullptr;
    }
    inline Function &getFunction() const { return *function; }
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

private:
    InterpretResult run();

    inline CallFrame &initFrame(FunctionPtr function, size_t frameOffset)
    {
        auto &frame = frames[frameOffset];
        frame.ip = function->getChunk().data();
        frame.function = function;
        frame.fp = stackTop;
        return frame;
    }
    inline CallFrame &pushFrame(FunctionPtr function)
    {
        return initFrame(function, frameCount++);
    }
    inline bool callValue(Value callee, int argCount);
    inline bool call(FunctionPtr function, int argCount);
    inline void popFrame()
    {
        auto &frame = frames[--frameCount];
        frame.reset();
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
        auto object = std::static_pointer_cast<Object>(function);
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
    void traceStack();
    void traceGlobals();
    void runtimeError(const std::string &message);
};

#endif // __cpplox_vm_h