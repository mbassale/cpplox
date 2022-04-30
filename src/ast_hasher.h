#ifndef __cpplox_ast_hasher_h
#define __cpplox_ast_hasher_h

#include "ast.h"

namespace cpplox::ast {

class AstHasher : public NodeVisitor<bool> {
  bool visitBlockStmt(BlockPtr stmt) override { return false; }
  bool visitExpressionStatement(ExpressionPtr stmt) override { return false; }
  bool visitForStatement(ForStatementPtr stmt) override { return false; }
};

}  // namespace cpplox::ast

#endif  // __cpplox_ast_hasher_h