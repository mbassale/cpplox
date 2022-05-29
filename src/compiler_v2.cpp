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
                                const ProgramPtr& program) {
  function = std::make_shared<Function>(FunctionType::TYPE_SCRIPT, name);
  compileUnit(program);
  emitEpilogue();
  return hadError ? nullptr : std::move(function);
}

void CompilerV2::compileUnit(const ProgramPtr& program) {
  for (const auto& stmt : program->statements) {
    declaration(stmt);
  }
}

void CompilerV2::declaration(const StatementPtr& stmt) {
  switch (stmt->Type) {
    case NodeType::VAR_DECLARATION: {
      const auto varDeclarationStmt =
          std::dynamic_pointer_cast<VarDeclaration>(stmt);
      varDeclaration(varDeclarationStmt);
      break;
    }
    default:
      statement(stmt);
  }
}

void CompilerV2::varDeclaration(const VarDeclarationPtr& stmt) {
  const auto name = stmt->identifier.lexeme();
  uint8_t global = declareVariable(name);
  if (stmt->initializer) {
    expression(stmt->initializer);
  } else {
    emitByte(OP_NIL);
  }
  defineVariable(global);
}

void CompilerV2::statement(const StatementPtr& stmt) {
  switch (stmt->Type) {
    case NodeType::EXPRESSION_STATEMENT: {
      const auto expressionStmt =
          std::dynamic_pointer_cast<ExpressionStatement>(stmt);
      expressionStatement(expressionStmt);
      break;
    }
    case NodeType::BLOCK_STATEMENT: {
      const auto blockStmt = std::dynamic_pointer_cast<Block>(stmt);
      blockStatement(blockStmt);
      break;
    }
    case NodeType::IF_STATEMENT: {
      const auto ifStmt = std::dynamic_pointer_cast<IfStatement>(stmt);
      ifStatement(ifStmt);
      break;
    }
    case NodeType::WHILE_STATEMENT: {
      const auto whileStmt = std::dynamic_pointer_cast<WhileStatement>(stmt);
      whileStatement(whileStmt);
      break;
    }
    case NodeType::PRINT_STATEMENT: {
      const auto printStmt = std::dynamic_pointer_cast<PrintStatement>(stmt);
      printStatement(printStmt);
      break;
    }
    default: {
      std::ostringstream ss;
      ss << "Unknown Statement Type: " << (int)stmt->Type;
      throw CompilerError(ss.str());
    }
  }
}

void CompilerV2::expressionStatement(const ast::ExpressionStatementPtr& stmt) {
  expression(stmt->expression);
  emitByte(OP_POP);
}

void CompilerV2::blockStatement(const ast::BlockPtr& stmt) {
  for (const auto& innerStmt : stmt->statements) {
    statement(innerStmt);
  }
}

void CompilerV2::ifStatement(const ast::IfStatementPtr& stmt) {
  expression(stmt->condition);
  const auto thenJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement(stmt->thenBranch);
  const auto elseJump = emitJump(OP_JUMP);
  patchJump(thenJump);
  emitByte(OP_POP);
  if (stmt->elseBranch) {
    statement(stmt->elseBranch);
  }
  patchJump(elseJump);
}

void CompilerV2::whileStatement(const ast::WhileStatementPtr& stmt) {
  const auto loopStart = currentChunk().size();
  expression(stmt->condition);
  const auto exitJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement(stmt->body);
  emitLoop(loopStart);
  patchJump(exitJump);
  emitByte(OP_POP);
}

void CompilerV2::printStatement(const PrintStatementPtr& stmt) {
  expression(stmt->expression);
  emitByte(OP_PRINT);
}

void CompilerV2::returnStatement(const ast::ReturnStatementPtr& stmt) {
  if (stmt->expression) {
    expression(stmt->expression);
    emitByte(OP_RETURN);
  } else {
    emitByte(OP_NIL);
    emitByte(OP_RETURN);
  }
}

void CompilerV2::expression(const ExpressionPtr& expr) {
  switch (expr->Type) {
    case NodeType::BINARY_EXPRESSION: {
      const auto binaryExpr = std::dynamic_pointer_cast<BinaryExpr>(expr);
      binary(binaryExpr);
      break;
    }
    case NodeType::UNARY_EXPRESSION: {
      const auto unaryExpr = std::dynamic_pointer_cast<UnaryExpr>(expr);
      unary(unaryExpr);
      break;
    }
    case NodeType::LITERAL_EXPRESSION: {
      const auto literalExpr = std::dynamic_pointer_cast<Literal>(expr);
      literal(literalExpr);
      break;
    }
    case NodeType::VARIABLE_EXPRESSION: {
      const auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr);
      namedVariable(varExpr->identifier.lexeme());
      break;
    }
    default: {
      std::ostringstream ss;
      ss << "Unknown Expression Type: " << (int)expr->Type;
      throw CompilerError(ss.str());
    }
  }
}

void CompilerV2::binary(const ast::BinaryExprPtr& expr) {
  expression(expr->left);
  expression(expr->right);
  const auto operatorType = expr->operator_.type;
  switch (operatorType) {
    case TokenType::TOKEN_BANG_EQUAL:
      emitBytes(OP_EQUAL, OP_NOT);
      break;
    case TokenType::TOKEN_EQUAL_EQUAL:
      emitByte(OP_EQUAL);
      break;
    case TokenType::TOKEN_GREATER:
      emitByte(OP_GREATER);
      break;
    case TokenType::TOKEN_GREATER_EQUAL:
      emitBytes(OP_LESS, OP_NOT);
      break;
    case TokenType::TOKEN_LESS:
      emitByte(OP_LESS);
      break;
    case TokenType::TOKEN_LESS_EQUAL:
      emitBytes(OP_GREATER, OP_NOT);
      break;
    case TokenType::TOKEN_PLUS:
      emitByte(OP_ADD);
      break;
    case TokenType::TOKEN_MINUS:
      emitByte(OP_SUBTRACT);
      break;
    case TokenType::TOKEN_STAR:
      emitByte(OP_MULTIPLY);
      break;
    case TokenType::TOKEN_SLASH:
      emitByte(OP_DIVIDE);
      break;
    default:
      assert(false);  // unreachable
      break;
  }
}

void CompilerV2::unary(const ast::UnaryExprPtr& expr) {
  expression(expr->right);
  const auto operatorType = expr->operator_.type;
  switch (operatorType) {
    case TokenType::TOKEN_BANG:
      emitByte(OP_NOT);
      break;
    case TokenType::TOKEN_MINUS:
      emitByte(OP_NEGATE);
      break;
    default:
      assert(false);  // unreachable
      break;
  }
}

void CompilerV2::literal(const ast::LiteralPtr& expr) {
  switch (expr->TokenType_) {
    case TokenType::TOKEN_NUMBER:
    case TokenType::TOKEN_STRING:
      emitConstant(expr->Value_);
      break;
    case TokenType::TOKEN_NIL:
      emitByte(OP_NIL);
      break;
    case TokenType::TOKEN_TRUE:
      emitByte(OP_TRUE);
      break;
    case TokenType::TOKEN_FALSE:
      emitByte(OP_FALSE);
      break;
    default:
      assert(false);  // unreachable
      break;
  }
}

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

void CompilerV2::emitEpilogue() {
  emitByte(OP_NIL);
  emitByte(OP_RETURN);
}

void CompilerV2::emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

size_t CompilerV2::emitJump(uint8_t instruction) {
  emitByte(instruction);
  emitByte(0xff);
  emitByte(0xff);
  return currentChunk().size() - 2;
}

void CompilerV2::patchJump(size_t offset) {
  size_t jump = currentChunk().size() - offset - 2;
  if (jump > UINT16_MAX) {
    error("Too much code to jump over.");
  }
  uint8_t* code = currentChunk().data();
  code[offset] = (jump >> 8) & 0xFF;
  code[offset + 1] = jump & 0xFF;
}

void CompilerV2::emitLoop(size_t loopStart) {
  emitByte(OP_LOOP);
  const auto offset = currentChunk().size() - loopStart + 2;
  if (offset > UINT16_MAX) error("Loop body too large.");
  emitByte((offset >> 8) & 0xff);
  emitByte(offset & 0xff);
}

int CompilerV2::declareVariable(const std::string& name) {
  declareLocal(name);
  if (scopeDepth > 0) return 0;

  return makeConstant(Value(name));
}

void CompilerV2::declareLocal(const std::string& name) {
  if (scopeDepth == 0) return;

  auto it = locals.rbegin();
  while (it != locals.rend()) {
    if (it->depth != -1 && it->depth < scopeDepth) break;

    if (name == it->name) {
      std::ostringstream ss;
      ss << "Already a variable named '" << name << "' in this scope.";
      error(ss.str());
    }

    it++;
  }

  addLocal(name);
}

void CompilerV2::addLocal(const std::string& name) {
  Local local;
  local.name = name;
  local.depth = -1;
  locals.push_back(local);
}

void CompilerV2::defineVariable(size_t global) {
  if (scopeDepth > 0) {
    markInitialized();
    return;
  }
  emitBytes(OP_DEFINE_GLOBAL, global);
}

void CompilerV2::markInitialized() {
  if (scopeDepth == 0) return;
  locals[locals.size() - 1].depth = scopeDepth;
}

void CompilerV2::namedVariable(const std::string& name) {
  uint8_t getOp, setOp;
  int offset = resolveLocal(name);
  if (offset != -1) {
    getOp = OP_GET_LOCAL;
    setOp = OP_SET_LOCAL;
  } else {
    offset = makeConstant(Value(name));
    getOp = OP_GET_GLOBAL;
    setOp = OP_SET_GLOBAL;
  }

  emitBytes(getOp, offset);
}

int CompilerV2::resolveLocal(const std::string& name) {
  for (int i = locals.size() - 1; i >= 0; i--) {
    if (name == locals[i].name) {
      if (locals[i].depth == -1) {
        std::ostringstream ss;
        ss << "Can't read local variable '" << name
           << "' in its own initializer.";
        error(ss.str());
      }
      return i;
    }
  }
  return -1;
}

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