#ifndef __cpplox_chunk_h
#define __cpplox_chunk_h

#include "common.h"
#include "value.h"
#include "lineinfo.h"

typedef enum
{
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_CALL,
    OP_CLOSURE,
    OP_RETURN,
} OpCode;

union VMInstrArgs
{
    uint32_t u32;
    struct
    {
        uint16_t w0;
        uint16_t w1;
    } words;
    struct
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
    } bytes;
};

struct VMInstr
{
    OpCode opCode;
    std::string name;
    VMInstrArgs args;
    size_t offset;
    size_t nextOffset;

    VMInstr(OpCode opCode) : VMInstr(opCode, std::string(), 0, 0, 0) {}
    VMInstr(OpCode opCode, const std::string &name, uint32_t args, size_t offset, size_t nextOffset) : opCode(opCode), name(name), offset(offset), nextOffset(nextOffset)
    {
        this->args.u32 = args;
    }

    static VMInstr makeConstant(uint8_t index, size_t offset = 0, size_t nextOffset = 0)
    {
        VMInstrArgs args;
        args.u32 = 0;
        args.bytes.b0 = index;
        return VMInstr(OP_CONSTANT, "OP_CONSTANT", args.u32, offset, nextOffset);
    }

    static VMInstr makeDefineGlobal(uint8_t index, size_t offset = 0, size_t nextOffset = 0)
    {
        VMInstrArgs args;
        args.u32 = 0;
        args.bytes.b0 = index;
        return VMInstr(OP_DEFINE_GLOBAL, "OP_DEFINE_GLOBAL", args.u32, offset, nextOffset);
    }
};

class Chunk
{
private:
    std::string name;
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    LineInfo lines;

public:
    explicit Chunk(const std::string &name);

    inline const std::string &getName() const { return name; }
    inline uint8_t *data() { return code.data(); }
    inline size_t size() const { return code.size(); }
    inline LineInfo &getLines() { return lines; }

    void write(uint8_t byte, size_t line = LINE_INFO_CONTINUE);
    inline uint8_t read(size_t offset) const { return code[offset]; }
    size_t writeConstant(Value value);
    inline Value &readConstant(size_t offset) { return constants[offset]; }
};

typedef std::unique_ptr<Chunk> ChunkPtr;

#endif // __cpplox_chunk_h