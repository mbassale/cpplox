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
  static std::shared_ptr<Literal> makeTrue() {
    return std::make_shared<Literal>(Token(TOKEN_TRUE, "true"));
  }
  static std::shared_ptr<Literal> makeFalse() {
    return std::make_shared<Literal>(Token(TOKEN_FALSE, "false"));
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

struct VarDeclaration : public Statement {
  Token identifier;
  ExpressionPtr initializer;

  VarDeclaration(const Token& identifier)
      : identifier(identifier), initializer(nullptr) {}
  VarDeclaration(const Token& identifier, const ExpressionPtr& initializer)
      : identifier(identifier), initializer(initializer) {}

  bool isEqual(const Node& other) override {
    if (typeid(*this) == typeid(other)) {
      const auto& otherProgram = dynamic_cast<const VarDeclaration&>(other);
      return isEqual(otherProgram);
    }
    return false;
  }

  bool isEqual(const VarDeclaration& other) {
    if (!identifier.isEqual(other.identifier)) {
      return false;
    }

    // compare initializers
    bool hasLhs = (bool)this->initializer;
    bool hasRhs = (bool)other.initializer;
    if (hasLhs != hasRhs) {
      return false;
    }
    if (this->initializer && other.initializer) {
      if (!this->initializer->isEqual(*other.initializer)) {
        return false;
      }
    }

    return true;
  }

  static std::shared_ptr<VarDeclaration> make(
      const Token& identifier, const ExpressionPtr& initializer = nullptr) {
    return std::make_shared<VarDeclaration>(identifier, initializer);
  }
};
typedef std::shared_ptr<VarDeclaration> VarDeclarationPtr;

struct Block : public Statement {
  std::vector<StatementPtr> statements;

  Block() : statements() {}
  Block(const std::vector<StatementPtr>& statements) : statements(statements) {}

  bool isEqual(const Node& other) override {
    if (typeid(*this) == typeid(other)) {
      const auto& otherBlock = dynamic_cast<const Block&>(other);
      return isEqual(otherBlock);
    }
    return false;
  }

  bool isEqual(const Block& other) {
    return this->statements.size() == other.statements.size() &&
           std::equal(this->statements.cbegin(), this->statements.cend(),
                      other.statements.cbegin(),
                      [](const auto& stmt1, const auto& stmt2) {
                        return stmt1->isEqual(*stmt2);
                      });
  }

  static std::shared_ptr<Block> make(
      const std::vector<StatementPtr>& statements) {
    return std::make_shared<Block>(statements);
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

  bool isEqual(const Node& other) override {
    if (typeid(*this) == typeid(other)) {
      const auto& otherFor = dynamic_cast<const ForStatement&>(other);
      return isEqual(otherFor);
    }
    return false;
  }

  bool isEqual(const ForStatement& other) {
    // compare initializers
    bool hasLhs = (bool)this->initializer;
    bool hasRhs = (bool)other.initializer;
    if (hasLhs != hasRhs) {
      return false;
    }
    if (this->initializer && other.initializer) {
      if (!this->initializer->isEqual(*other.initializer)) {
        return false;
      }
    }

    // compare conditions
    hasLhs = (bool)this->condition;
    hasRhs = (bool)other.condition;
    if (hasLhs != hasRhs) {
      return false;
    }
    if (this->condition && other.condition) {
      if (!this->condition->isEqual(*other.condition)) {
        return false;
      }
    }

    // compare increments
    hasLhs = (bool)this->increment;
    hasRhs = (bool)other.increment;
    if (hasLhs != hasRhs) {
      return false;
    }
    if (this->increment && other.increment) {
      if (!this->condition->isEqual(*other.increment)) {
        return false;
      }
    }

    return body->isEqual(*other.body);
  }

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

struct WhileStatement : public Statement {
  ExpressionPtr condition;
  StatementPtr body;

  WhileStatement() : condition(nullptr), body(nullptr) {}
  WhileStatement(const ExpressionPtr& condition, const StatementPtr& body)
      : condition(condition), body(body) {}

  bool isEqual(const Node& other) override {
    if (typeid(*this) == typeid(other)) {
      const auto& otherWhile = dynamic_cast<const WhileStatement&>(other);
      return isEqual(otherWhile);
    }
    return false;
  }

  bool isEqual(const WhileStatement& other) {
    // compare conditions
    const auto hasLhs = (bool)this->condition;
    const auto hasRhs = (bool)other.condition;
    if (hasLhs != hasRhs) {
      return false;
    }
    if (this->condition && other.condition) {
      if (!this->condition->isEqual(*other.condition)) {
        return false;
      }
    }

    return body->isEqual(*other.body);
  }

  static std::shared_ptr<WhileStatement> make() {
    return std::make_shared<WhileStatement>();
  }

  static std::shared_ptr<WhileStatement> make(const ExpressionPtr& condition,
                                              const StatementPtr& body) {
    return std::make_shared<WhileStatement>(condition, body);
  }
};
typedef std::shared_ptr<WhileStatement> WhileStatementPtr;

struct PrintStatement : public Statement {
  ExpressionPtr expression;

  PrintStatement() : expression(nullptr) {}
  PrintStatement(const ExpressionPtr& expression) : expression(expression) {}

  bool isEqual(const Node& other) override {
    if (typeid(*this) == typeid(other)) {
      const auto& otherPrint = dynamic_cast<const PrintStatement&>(other);
      return isEqual(otherPrint);
    }
    return false;
  }

  bool isEqual(const PrintStatement& other) {
    return expression->isEqual(*other.expression);
  }

  static std::shared_ptr<PrintStatement> make() {
    return std::make_shared<PrintStatement>();
  }

  static std::shared_ptr<PrintStatement> make(const ExpressionPtr& expression) {
    return std::make_shared<PrintStatement>(expression);
  }
};
typedef std::shared_ptr<PrintStatement> PrintStatementPtr;

struct IfStatement : public Statement {
  ExpressionPtr condition;
  StatementPtr thenBranch;
  StatementPtr elseBranch;

  IfStatement()
      : condition(nullptr), thenBranch(nullptr), elseBranch(nullptr) {}
  IfStatement(const ExpressionPtr& condition, const StatementPtr& thenBranch)
      : condition(condition), thenBranch(thenBranch), elseBranch(nullptr) {}
  IfStatement(const ExpressionPtr& condition, const StatementPtr& thenBranch,
              const StatementPtr& elseBranch)
      : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

  static std::shared_ptr<IfStatement> make(const ExpressionPtr& condition,
                                           const StatementPtr& thenBranch) {
    return std::make_shared<IfStatement>(condition, thenBranch);
  }
  static std::shared_ptr<IfStatement> make(const ExpressionPtr& condition,
                                           const StatementPtr& thenBranch,
                                           const StatementPtr& elseBranch) {
    return std::make_shared<IfStatement>(condition, thenBranch, elseBranch);
  }

  bool isEqual(const Node& other) override {
    if (typeid(*this) == typeid(other)) {
      const auto& otherFor = dynamic_cast<const IfStatement&>(other);
      return isEqual(otherFor);
    }
    return false;
  }

  bool isEqual(const IfStatement& other) {
    // compare conditions
    bool hasLhs = (bool)condition;
    bool hasRhs = (bool)other.condition;
    if (hasLhs != hasRhs) {
      return false;
    }
    if (!this->condition->isEqual(*other.condition)) {
      return false;
    }

    // compare branches
    bool thenBranchEqual = thenBranch->isEqual(*other.thenBranch);
    if (thenBranchEqual && elseBranch) {
      return elseBranch->isEqual(*other.elseBranch);
    }
    return thenBranchEqual;
  }
};
typedef std::shared_ptr<IfStatement> IfStatementPtr;

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