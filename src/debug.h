#ifndef __cpplox_debug_h
#define __cpplox_debug_h

#include "common.h"
#include "chunk.h"

class Disassembler
{
private:
    Chunk &chunk;

public:
    Disassembler(Chunk &chunk) : chunk(chunk) {}

    std::vector<VMInstr> disassemble();
    VMInstr disassembleInstruction(size_t offset);

private:
    VMInstr simpleInstruction(OpCode opCode, const std::string &name, size_t offset);
    VMInstr constantInstruction(OpCode opCode, const std::string &name, size_t offset);
    VMInstr byteInstruction(OpCode opCode, const std::string &name, size_t offset);
    VMInstr jumpInstruction(OpCode opCode, const std::string &name, size_t offset);
};

class OpCodePrinter
{
private:
    Chunk &chunk;
    std::vector<VMInstr> instructions;

public:
    OpCodePrinter(Chunk &chunk, const std::vector<VMInstr> &instructions) : chunk(chunk), instructions(instructions) {}

    void print();

private:
    void printLineColumn(size_t offset);
    void printInstructionAddr(size_t offset);
    void printSimpleInstruction(const VMInstr &instr);
    void printConstantInstruction(const VMInstr &instr);
    void printByteInstruction(const VMInstr &instr);
    void printJumpInstruction(const VMInstr &instr);
};

#endif // __cpplox_debug_h