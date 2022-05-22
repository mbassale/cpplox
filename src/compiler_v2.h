#ifndef __cpplox_compiler_v2_h
#define __cpplox_compiler_v2_h

#include "ast.h"
#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "function.h"
#include "scanner.h"

namespace cpplox {

class CompilerV2 {
 private:
  CompilerConfig config;
  ast::ProgramPtr program;
  ast::NodePtr previous;
  ast::NodePtr current;
  FunctionPtr function{nullptr};
  bool hadError{false};
  bool panicMode{false};
  std::list<std::string> errors{};
  std::vector<Local> locals{};
  std::vector<UpvalueItem> upvalues{};
  int scopeDepth{0};

 public:
  explicit CompilerV2();
  explicit CompilerV2(const CompilerConfig &flags);

  FunctionPtr compile(const std::string &name, const ast::ProgramPtr &program);
  inline bool hasErrors() const { return !errors.empty(); }
  inline const std::list<std::string> &getErrors() const { return errors; }

 protected:
  void compileProgram(const ast::ProgramPtr &program);
  void compileStatement(const ast::StatementPtr &stmt);
  void compilePrintStatement(const ast::PrintStatementPtr &stmt);

  inline Chunk &currentChunk() { return function->getChunk(); }
  void emitByte(uint8_t byte);
  void emitBytes(uint8_t byte1, uint8_t byte2);
  size_t makeConstant(Value value);
  void emitEpilogue();
  void emitConstant(Value value);
  void emitReturn();
  size_t emitJump(uint8_t instruction);
  void patchJump(size_t offset);
  void emitLoop(size_t loopStart);

  // Error routines
  void synchronize();
  void error(const std::string &message);
  void errorAtCurrent(const std::string &message);
  void errorAt(const std::string &message);
};

}  // namespace cpplox

#endif  // __cpplox_compiler_v2_h