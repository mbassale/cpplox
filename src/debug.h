#ifndef __cpplox_debug_h
#define __cpplox_debug_h

#include "chunk.h"
#include "closure.h"
#include "common.h"
#include "function.h"
#include "native.h"

class InvalidBytecodeException : public std::runtime_error {
 private:
  OpCode opCode;

 public:
  InvalidBytecodeException(OpCode opCode, const std::string &message)
      : opCode(opCode), std::runtime_error(message) {}

  inline OpCode getOpCode() const { return opCode; }
};

class Disassembler {
 private:
  Chunk &chunk;

 public:
  Disassembler(Chunk &chunk) : chunk(chunk) {}

  std::vector<VMInstr> disassemble();
  VMInstr disassembleInstruction(size_t offset);

 private:
  VMInstr simpleInstruction(OpCode opCode, const std::string &name,
                            size_t offset);
  VMInstr constantInstruction(OpCode opCode, const std::string &name,
                              size_t offset);
  VMInstr byteInstruction(OpCode opCode, const std::string &name,
                          size_t offset);
  VMInstr closureInstruction(size_t offset);
  VMInstr jumpInstruction(OpCode opCode, const std::string &name,
                          size_t offset);
};

class OpCodePrinter {
 private:
  Chunk &chunk;
  std::vector<VMInstr> instructions;

 public:
  OpCodePrinter(Chunk &chunk) : chunk(chunk), instructions() {}
  OpCodePrinter(Chunk &chunk, const std::vector<VMInstr> &instructions)
      : chunk(chunk), instructions(instructions) {}

  void print();
  void printInstruction(const VMInstr &instr);

 private:
  void printLineColumn(size_t offset);
  void printInstructionAddr(size_t offset);
  void printSimpleInstruction(const VMInstr &instr);
  void printConstantInstruction(const VMInstr &instr);
  void printByteInstruction(const VMInstr &instr);
  void printClosureInstruction(const VMInstr &instr);
  void printJumpInstruction(const VMInstr &instr);

  std::string printByte(uint8_t byte);
};

#endif  // __cpplox_debug_h