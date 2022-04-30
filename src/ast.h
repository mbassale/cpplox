#ifndef __cpplox_ast_h
#define __cpplox_ast_h

#include "common.h"
#include "scanner.h"

namespace cpplox::ast {

struct Node {
  Node() {}

  virtual bool isEqual(const Node& other) { return true; }
};
typedef std::shared_ptr<Node> NodePtr;

struct Statement : public Node {
  static std::shared_ptr<Statement> make() {
    return std::make_shared<Statement>();
  }
};
typedef std::shared_ptr<Statement> StatementPtr;
#define AS_STATEMENT(EXPR) \
  std::dynamic_pointer_cast<cpplox::ast::Statement>(EXPR)

struct Expression : public Node {};
typedef std::shared_ptr<Expression> ExpressionPtr;
#define AS_EXPRESSION(EXPR) \
  std::dynamic_pointer_cast<cpplox::ast::Expression>(EXPR)

struct Literal : public Expression {
  Token literal;

  Literal(const Token& literal) : literal(literal) {}
  Literal(Token&& literal) : literal(literal) {}

  bool isEqual(const Node& other) override {
    if (typeid(other) == typeid(*this)) {
      const auto& otherLiteral = dynamic_cast<const Literal&>(other);
      return isEqual(otherLiteral);
    }
    return false;
  }

  bool isEqual(const Literal& other) { return this->literal == other.literal; }

  static std::shared_ptr<Literal> make(const Token& literal) {
    return std::make_shared<Literal>(literal);
  }
};
typedef std::shared_ptr<Literal> LiteralPtr;

struct Identifier : public Expression {
  Token identifier;
  Identifier(const Token& identifier) : identifier(identifier) {}

  bool isEqual(const Node& other) override {
    if (typeid(other) == typeid(*this)) {
      const auto& otherIdentifier = dynamic_cast<const Identifier&>(other);
      return isEqual(otherIdentifier);
    }
    return false;
  }

  bool isEqual(const Identifier& other) {
    return this->identifier == other.identifier;
  }
};
typedef std::shared_ptr<Identifier> IdentifierPtr;

struct Program : public Node {
  std::vector<StatementPtr> statements;

  Program() : statements() {}
  Program(const std::vector<StatementPtr>& statements)
      : statements(statements) {}

  bool isEqual(const Node& other) override {
    if (typeid(*this) == typeid(other)) {
      const auto& otherProgram = dynamic_cast<const Program&>(other);
      return isEqual(otherProgram);
    }
    return false;
  }

  bool isEqual(const Program& other) {
    return this->statements.size() == other.statements.size() &&
           std::equal(this->statements.cbegin(), this->statements.cend(),
                      other.statements.cbegin(),
                      [](const auto& stmt1, const auto& stmt2) {
                        return stmt1->isEqual(*stmt2);
                      });
  }

  static std::shared_ptr<Program> make(
      const std::vector<StatementPtr>& statements) {
    return std::make_shared<Program>(statements);
  }
};
typedef std::shared_ptr<Program> ProgramPtr;

struct Block : public Statement {
  std::vector<StatementPtr> statements;

  bool isEqual(const Node& other) override {
    if (typeid(*this) == typeid(other)) {
      const auto& otherBlock = dynamic_cast<const Block&>(other);
      return isEqual(otherBlock);
    }
    return false;
  }

  bool isEqual(const Block& other) {
    return this->statements == other.statements;
  }
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
  ForStatement(const ExpressionPtr& initializer, const ExpressionPtr& condition,
               const ExpressionPtr& increment, const StatementPtr& body)
      : initializer(initializer),
        condition(condition),
        increment(increment),
        body(body) {}

  static std::shared_ptr<ForStatement> make() {
    return std::make_shared<ForStatement>();
  }

  static std::shared_ptr<ForStatement> make(const ExpressionPtr& initializer,
                                            const ExpressionPtr& condition,
                                            const ExpressionPtr& increment,
                                            const StatementPtr& body) {
    return std::make_shared<ForStatement>(initializer, condition, increment,
                                          body);
  }
};
typedef std::shared_ptr<ForStatement> ForStatementPtr;

struct ExpressionStatement : public Statement {
  ExpressionPtr expression;

  ExpressionStatement() : expression(nullptr) {}
  ExpressionStatement(const ExpressionPtr& expression)
      : expression(expression) {}

  bool isEqual(const Node& other) override {
    if (typeid(*this) == typeid(other)) {
      const auto& otherExprStmt =
          dynamic_cast<const ExpressionStatement&>(other);
      return isEqual(otherExprStmt);
    }
    return false;
  }

  bool isEqual(const ExpressionStatement& other) {
    return this->expression->isEqual(*other.expression);
  }

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