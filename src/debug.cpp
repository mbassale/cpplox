#include "debug.h"

std::vector<VMInstr> Disassembler::disassemble() {
  std::vector<VMInstr> instructions;
  for (size_t offset = 0; offset < chunk.size();) {
    auto result = disassembleInstruction(offset);
    instructions.push_back(result);
    offset = result.nextOffset;
  }
  return instructions;
}

VMInstr Disassembler::disassembleInstruction(size_t offset) {
  OpCode instruction = (OpCode)chunk.data()[offset];
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstruction(instruction, "OP_CONSTANT", offset);

    case OP_NIL:
      return simpleInstruction(instruction, "OP_NIL", offset);

    case OP_TRUE:
      return simpleInstruction(instruction, "OP_TRUE", offset);

    case OP_FALSE:
      return simpleInstruction(instruction, "OP_FALSE", offset);

    case OP_POP:
      return simpleInstruction(instruction, "OP_POP", offset);

    case OP_GET_LOCAL:
      return byteInstruction(instruction, "OP_GET_LOCAL", offset);

    case OP_SET_LOCAL:
      return byteInstruction(instruction, "OP_SET_LOCAL", offset);

    case OP_DEFINE_GLOBAL:
      return constantInstruction(instruction, "OP_DEFINE_GLOBAL", offset);

    case OP_GET_GLOBAL:
      return simpleInstruction(instruction, "OP_GET_GLOBAL", offset);

    case OP_SET_GLOBAL:
      return simpleInstruction(instruction, "OP_SET_GLOBAL", offset);

    case OP_GET_UPVALUE:
      return byteInstruction(instruction, "OP_GET_UPVALUE", offset);

    case OP_SET_UPVALUE:
      return byteInstruction(instruction, "OP_SET_UPVALUE", offset);

    case OP_EQUAL:
      return simpleInstruction(instruction, "OP_EQUAL", offset);

    case OP_GREATER:
      return simpleInstruction(instruction, "OP_GREATER", offset);

    case OP_LESS:
      return simpleInstruction(instruction, "OP_LESS", offset);

    case OP_ADD:
      return simpleInstruction(instruction, "OP_ADD", offset);

    case OP_SUBTRACT:
      return simpleInstruction(instruction, "OP_SUBTRACT", offset);

    case OP_MULTIPLY:
      return simpleInstruction(instruction, "OP_MULTIPLY", offset);

    case OP_DIVIDE:
      return simpleInstruction(instruction, "OP_DIVIDE", offset);

    case OP_NEGATE:
      return simpleInstruction(instruction, "OP_NEGATE", offset);

    case OP_NOT:
      return simpleInstruction(instruction, "OP_NOT", offset);

    case OP_PRINT:
      return simpleInstruction(instruction, "OP_PRINT", offset);

    case OP_JUMP:
      return jumpInstruction(instruction, "OP_JUMP", offset);

    case OP_JUMP_IF_FALSE:
      return jumpInstruction(instruction, "OP_JUMP_IF_FALSE", offset);

    case OP_LOOP:
      return jumpInstruction(instruction, "OP_LOOP", offset);

    case OP_CALL:
      return byteInstruction(instruction, "OP_CALL", offset);

    case OP_CLOSURE:
      return closureInstruction(offset);

    case OP_RETURN:
      return simpleInstruction(instruction, "OP_RETURN", offset);

    default:
      std::cout << "Unknown opcode: " << instruction << std::endl;
      return VMInstr(instruction, "Unknown OpCode", 0, offset, offset + 1);
  }
}

VMInstr Disassembler::simpleInstruction(OpCode opCode, const std::string &name,
                                        size_t offset) {
  return VMInstr(opCode, name, 0, offset, offset + 1);
}

VMInstr Disassembler::constantInstruction(OpCode opCode,
                                          const std::string &name,
                                          size_t offset) {
  uint8_t constantOffset = chunk.read(offset + 1);
  VMInstr instr(opCode, name, 0, offset, offset + 2);
  instr.args.bytes.b0 = constantOffset;
  return instr;
}

VMInstr Disassembler::byteInstruction(OpCode opCode, const std::string &name,
                                      size_t offset) {
  uint8_t arg0 = chunk.read(offset + 1);
  VMInstrArgs args;
  args.u32 = 0;
  args.bytes.b0 = arg0;
  return VMInstr(opCode, name, args.u32, offset, offset + 2);
}

VMInstr Disassembler::closureInstruction(size_t offset) {
  uint8_t constantOffset = chunk.read(offset + 1);
  const auto constantValue = chunk.readConstant(constantOffset);
  if (constantValue.isObject()) {
    const auto object = (ObjectPtr)constantValue;
    // we have a function?
    if (object->getType() == ObjectType::OBJ_FUNCTION) {
      const auto function = std::dynamic_pointer_cast<Function>(object);
      if (function) {
        VMInstrArgs args;
        args.u32 = 0;
        args.bytes.b0 = constantOffset;
        return VMInstr(OP_CLOSURE, "OP_CLOSURE", args.u32, offset,
                       offset + function->getUpvalueCount() * 2 + 2);
      }
    }
    throw InvalidBytecodeException(
        OP_CLOSURE, "Closure constant value is not a FunctionPtr");
  }
  throw InvalidBytecodeException(OP_CLOSURE,
                                 "Closure constant value is not an Object");
}

VMInstr Disassembler::jumpInstruction(OpCode opCode, const std::string &name,
                                      size_t offset) {
  uint8_t *code = chunk.data();
  uint16_t jump = (uint16_t)(code[offset + 1] << 8);
  jump |= code[offset + 2];
  VMInstr instr(opCode, name, 0, offset, offset + 3);
  instr.args.words.w0 = jump;
  return instr;
}

void OpCodePrinter::print() {
  std::cout << "chunk " << chunk.getName() << "() {\n";
  for (const auto &instr : instructions) {
    printInstruction(instr);
  }
  std::cout << "} // " << chunk.getName() << std::endl;
}

void OpCodePrinter::printInstruction(const VMInstr &instr) {
  printLineColumn(instr.offset);
  printInstructionAddr(instr.offset);
  std::cout << instr.name << " ";

  switch (instr.opCode) {
    case OP_CONSTANT:
    case OP_DEFINE_GLOBAL:
      printConstantInstruction(instr);
      break;

    case OP_NIL:
    case OP_TRUE:
    case OP_FALSE:
    case OP_POP:
    case OP_GET_GLOBAL:
    case OP_SET_GLOBAL:
    case OP_EQUAL:
    case OP_GREATER:
    case OP_LESS:
    case OP_ADD:
    case OP_SUBTRACT:
    case OP_MULTIPLY:
    case OP_DIVIDE:
    case OP_NEGATE:
    case OP_NOT:
    case OP_PRINT:
    case OP_RETURN:
      printSimpleInstruction(instr);
      break;

    case OP_GET_LOCAL:
    case OP_SET_LOCAL:
    case OP_GET_UPVALUE:
    case OP_SET_UPVALUE:
    case OP_CALL:
      printByteInstruction(instr);
      break;

    case OP_CLOSURE:
      printClosureInstruction(instr);
      break;

    case OP_JUMP:
    case OP_JUMP_IF_FALSE:
    case OP_LOOP:
      printJumpInstruction(instr);
      break;

    default:
      std::cout << "Unknown opcode: " << instr.opCode << std::endl;
      break;
  }
}

void OpCodePrinter::printLineColumn(size_t offset) {
  std::cout << std::dec << std::setfill('0') << std::setw(4)
            << chunk.getLines().get(offset) << " ";
}

void OpCodePrinter::printInstructionAddr(size_t offset) {
  std::cout << std::hex << std::setfill('0') << std::setw(4) << offset << " ";
}

void OpCodePrinter::printSimpleInstruction(const VMInstr &instr) {
  std::cout << std::endl;
}

void OpCodePrinter::printConstantInstruction(const VMInstr &instr) {
  const auto constantOffset = instr.args.bytes.b0;
  const auto constantValue = chunk.readConstant(constantOffset);
  std::cout << std::setfill('0') << std::setw(4) << (size_t)constantOffset
            << " " << (std::string)constantValue << std::endl;
}

void OpCodePrinter::printByteInstruction(const VMInstr &instr) {
  const auto byteOffset = instr.args.bytes.b0;
  std::cout << printByte(byteOffset) << std::endl;
}

void OpCodePrinter::printClosureInstruction(const VMInstr &instr) {
  const auto constantOffset = instr.args.bytes.b0;
  const auto constantValue = chunk.readConstant(constantOffset);
  std::cout << std::setfill('0') << std::setw(4) << (size_t)constantOffset
            << " " << (std::string)constantValue << " ";

  if (constantValue.isObject()) {
    const auto object = (ObjectPtr)constantValue;
    // we have a function?
    if (object->getType() == ObjectType::OBJ_FUNCTION) {
      const auto function = std::dynamic_pointer_cast<Function>(object);
      if (function) {
        for (int i = 0; i < function->getUpvalueCount(); i++) {
          std::cout << "u: " << printByte(chunk.read(instr.offset + i + 1))
                    << " l: " << printByte(chunk.read(instr.offset + i + 2))
                    << ", ";
        }
        std::cout << std::endl;

        Disassembler disassembler(function->getChunk());
        const auto instructions = disassembler.disassemble();
        OpCodePrinter opCodePrinter(function->getChunk(), instructions);
        opCodePrinter.print();
      } else {
        std::cout << std::endl;
      }
    } else {
      std::cout << std::endl;
    }
  } else {
    std::cout << std::endl;
  }
}

void OpCodePrinter::printJumpInstruction(const VMInstr &instr) {
  const auto sign = instr.opCode == OP_LOOP ? -1 : 1;
  const auto jump = instr.args.words.w0;
  std::cout << std::hex << std::setfill('0') << std::setw(4)
            << (sign * jump + instr.offset + 3) << std::endl;
}

std::string OpCodePrinter::printByte(uint8_t byte) {
  std::ostringstream ss;
  ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  return ss.str();
}