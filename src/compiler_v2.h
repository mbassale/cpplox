#ifndef __cpplox_compiler_v2_h
#define __cpplox_compiler_v2_h

#include "ast.h"
#include "chunk.h"
#include "common.h"
#include "function.h"
#include "scanner.h"

namespace cpplox {

class CompilerError : public std::runtime_error {
 public:
  CompilerError(const std::string &message) : std::runtime_error(message) {}
};

struct CompilerConfig {
  bool dumpTokens;
  bool disassembleInstructions;
};

struct Local {
  std::string name;
  int depth;
};

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
  int scopeDepth{0};

 public:
  explicit CompilerV2();
  explicit CompilerV2(const CompilerConfig &flags);

  FunctionPtr compile(const std::string &name, const ast::ProgramPtr &program);
  inline bool hasErrors() const { return !errors.empty(); }
  inline const std::list<std::string> &getErrors() const { return errors; }

 protected:
  void compileUnit(const ast::ProgramPtr &program);
  void declaration(const ast::StatementPtr &stmt);
  void varDeclaration(const ast::VarDeclarationPtr &stmt);
  void statement(const ast::StatementPtr &stmt);
  void expressionStatement(const ast::ExpressionStatementPtr &stmt);
  void blockStatement(const ast::BlockPtr &stmt);
  void ifStatement(const ast::IfStatementPtr &stmt);
  void whileStatement(const ast::WhileStatementPtr &stmt);
  void forStatement(const ast::ForStatementPtr &stmt);
  void printStatement(const ast::PrintStatementPtr &stmt);
  void returnStatement(const ast::ReturnStatementPtr &stmt);
  void expression(const ast::ExpressionPtr &expr);
  void binary(const ast::BinaryExprPtr &expr);
  void unary(const ast::UnaryExprPtr &expr);
  void literal(const ast::LiteralPtr &expr);
  void namedVariableExpression(const std::string &name,
                               const ast::ExpressionPtr value);

  inline Chunk &currentChunk() { return function->getChunk(); }
  void emitByte(uint8_t byte);
  void emitBytes(uint8_t byte1, uint8_t byte2);
  size_t makeConstant(Value value);
  void emitEpilogue();
  void emitConstant(Value value);
  size_t emitJump(uint8_t instruction);
  void patchJump(size_t offset);
  void emitLoop(size_t loopStart);

  // Locals routines
  void beginScope();
  void endScope();
  int declareVariable(const std::string &name);
  void declareLocal(const std::string &name);
  void addLocal(const std::string &name);
  void defineVariable(size_t global);
  void markInitialized();
  int resolveLocal(const std::string &name);

  // Error routines
  void error(const std::string &message);
  void errorAtCurrent(const std::string &message);
  void errorAt(const std::string &message);
};

}  // namespace cpplox

#endif  // __cpplox_compiler_v2_h