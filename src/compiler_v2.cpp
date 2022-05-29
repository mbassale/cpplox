#include "compiler_v2.h"

namespace cpplox {
using namespace ast;

CompilerV2::CompilerV2() : CompilerV2({false, false}) {}

CompilerV2::CompilerV2(const CompilerConfig& config) : config(config) {
  scopeDepth = 0;
  hadError = false;
  panicMode = false;
  locals.reserve(UINT8_MAX + 1);
}

FunctionPtr CompilerV2::compile(const std::string& name,
                                const ast::ProgramPtr& program) {
  function = std::make_shared<Function>(FunctionType::TYPE_SCRIPT, name);
  compileProgram(program);
  emitEpilogue();
  return hadError ? nullptr : std::move(function);
}

void CompilerV2::compileProgram(const ast::ProgramPtr& program) {
  for (const auto& stmt : program->statements) {
    statement(stmt);
  }
}
void CompilerV2::statement(const ast::StatementPtr& stmt) {
  if (typeid(*stmt) == typeid(ast::PrintStatement)) {
    const auto printStmt = std::dynamic_pointer_cast<ast::PrintStatement>(stmt);
    printStatement(printStmt);
  }
}

void CompilerV2::printStatement(const ast::PrintStatementPtr& stmt) {
  expression(stmt->expression);
  emitByte(OP_PRINT);
}

void CompilerV2::expression(const ast::ExpressionPtr& expr) {}

void CompilerV2::emitByte(uint8_t byte) { currentChunk().write(byte); }

void CompilerV2::emitBytes(uint8_t byte1, uint8_t byte2) {
  currentChunk().write(byte1);
  currentChunk().write(byte2);
}

size_t CompilerV2::makeConstant(Value value) {
  const auto constantOffset = currentChunk().writeConstant(value);
  // TODO: support OP_CONSTANT_LONG opcode.
  if (constantOffset > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0;
  }
  return constantOffset;
}

void CompilerV2::emitEpilogue() { emitReturn(); }

void CompilerV2::emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

void CompilerV2::emitReturn() {
  emitByte(OP_NIL);
  emitByte(OP_RETURN);
}

size_t CompilerV2::emitJump(uint8_t instruction) {
  emitByte(instruction);
  emitByte(0xff);
  emitByte(0xff);
  return currentChunk().size() - 2;
}

void CompilerV2::synchronize() { panicMode = false; }

void CompilerV2::error(const std::string& message) { errorAt(message); }

void CompilerV2::errorAtCurrent(const std::string& message) {
  errorAt(message);
}

void CompilerV2::errorAt(const std::string& message) {
  if (panicMode) return;
  panicMode = true;
  std::ostringstream ss;
  ss << "Error: " << message;
  errors.push_back(ss.str());
}

}  // namespace cpplox