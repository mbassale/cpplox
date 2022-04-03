#ifndef __cpplox_chunk_h
#define __cpplox_chunk_h

#include "common.h"
#include "value.h"
#include "lineinfo.h"

typedef enum
{
    OP_CONSTANT,
    OP_CONSTANT_LONG,
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
    explicit Chunk(const std::string &name);
    void write(uint8_t byte, size_t line = LINE_INFO_CONTINUE);
    inline uint8_t read(size_t offset) const { return code[offset]; }
    size_t writeConstant(Value value);
    inline Value &readConstant(size_t offset) { return constants[offset]; }
    inline uint8_t *data() { return code.data(); }
    size_t size() const { return code.size(); }

    void disassemble();
    size_t disassembleInstruction(size_t offset);

private:
    void printLineColumn(size_t offset);
    void printInstructionAddr(size_t offset);
    size_t simpleInstruction(const std::string &name, size_t offset);
    size_t constantInstruction(const std::string &name, size_t offset);
    size_t constantLongInstruction(const std::string &name, size_t offset);
    size_t byteInstruction(const std::string &name, size_t offset);
    size_t jumpInstruction(const std::string &name, int sign, size_t offset);
};

typedef std::unique_ptr<Chunk> ChunkPtr;

#endif // __cpplox_chunk_h