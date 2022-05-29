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
    statement(stmt);
  }
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