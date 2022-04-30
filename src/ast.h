#ifndef __cpplox_ast_h
#define __cpplox_ast_h

#include "common.h"
#include "scanner.h"

namespace cpplox::ast {

struct Node {
  Node() {}
};
typedef std::shared_ptr<Node> NodePtr;

struct Statement : public Node {};
typedef std::shared_ptr<Statement> StatementPtr;
#define AS_STATEMENT_PTR(EXPR) \
  std::static_pointer_cast<cpplox::ast::Statement>(EXPR)

struct Expression : public Node {};
typedef std::shared_ptr<Expression> ExpressionPtr;

struct Literal : public Expression {
  Token literal;

  Literal(const Token& literal) : literal(literal) {}
  Literal(Token&& literal) : literal(literal) {}

  static std::shared_ptr<Literal> make(const Token& literal) {
    return std::make_shared<Literal>(literal);
  }
};
typedef std::shared_ptr<Literal> LiteralPtr;

struct Identifier : public Expression {
  Token identifier;
  Identifier(const Token& identifier) : identifier(identifier) {}
};
typedef std::shared_ptr<Identifier> IdentifierPtr;

struct Program : public Node {
  std::vector<StatementPtr> statements;

  Program() : statements() {}
  Program(const std::vector<StatementPtr>& statements)
      : statements(statements) {}

  static std::shared_ptr<Program> make(
      const std::vector<StatementPtr>& statements) {
    return std::make_shared<Program>(statements);
  }
};
typedef std::shared_ptr<Program> ProgramPtr;

struct Block : public Statement {
  std::vector<StatementPtr> statements;
};
typedef std::shared_ptr<Block> BlockPtr;

struct ForStatement : public Statement {
  ExpressionPtr initializer;
  ExpressionPtr condition;
  ExpressionPtr increment;
  StatementPtr body;

  ForStatement()
      : initializer(nullptr),
        condition(nullptr),
        increment(nullptr),
        body(nullptr) {}

  static std::shared_ptr<ForStatement> make() {
    return std::make_shared<ForStatement>();
  }
};
typedef std::shared_ptr<ForStatement> ForStatementPtr;

struct ExpressionStatement : public Statement {
  ExpressionPtr expression;

  ExpressionStatement() : expression(nullptr) {}
  ExpressionStatement(const ExpressionPtr& expression)
      : expression(expression) {}

  static std::shared_ptr<ExpressionStatement> make(
      const ExpressionPtr& expression) {
    return std::make_shared<ExpressionStatement>(expression);
  }
};
typedef std::shared_ptr<ExpressionStatement> ExpressionStatementPtr;

template <typename T>
class NodeVisitor {
  virtual T visitBlockStmt(BlockPtr stmt);
  virtual T visitExpressionStatement(ExpressionPtr stmt);
  virtual T visitForStatement(ForStatementPtr stmt);
};

}  // namespace cpplox::ast

#endif