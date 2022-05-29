#ifndef __cpplox_ast_h
#define __cpplox_ast_h

#include "common.h"
#include "scanner.h"

namespace cpplox::ast {

enum class NodeType {

  /*
   * COMPILATION UNITS
   */
  PROGRAM,

  /*
   * DECLARATIONS
   */
  VAR_DECLARATION,

  /*
   * EXPRESSIONS
   */
  EXPRESSION,
  VARIABLE_EXPRESSION,
  ASSIGNMENT_EXPRESSION,
  BINARY_EXPRESSION,
  UNARY_EXPRESSION,
  LITERAL_EXPRESSION,

  /*
   * STATEMENTS
   */
  STATEMENT,
  EXPRESSION_STATEMENT,
  EMPTY_STATEMENT,
  BLOCK_STATEMENT,
  FOR_STATEMENT,
  IF_STATEMENT,
  WHILE_STATEMENT,
  PRINT_STATEMENT,
  RETURN_STATEMENT,
};

struct Node {
  const NodeType Type;

  Node(const NodeType type) : Type(type) {}

  virtual bool isEqual(const Node& other) { return true; }
};
using NodePtr = std::shared_ptr<Node>;

struct Statement : public Node {
  Statement() : Node(NodeType::EMPTY_STATEMENT) {}
  Statement(NodeType type) : Node(type) {}

  static std::shared_ptr<Statement> make() {
    return std::make_shared<Statement>();
  }
};
using StatementPtr = std::shared_ptr<Statement>;

struct Expression : public Node {
  Expression(NodeType type) : Node(type) {}
};
using ExpressionPtr = std::shared_ptr<Expression>;

struct Literal : public Expression {
  Token literal;

  Literal(const Token& literal)
      : Expression(NodeType::LITERAL_EXPRESSION), literal(literal) {}
  Literal(Token&& literal)
      : Expression(NodeType::LITERAL_EXPRESSION), literal(literal) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherLiteral = dynamic_cast<const Literal&>(other);
      return isEqual(otherLiteral);
    }
    return false;
  }

  bool isEqual(const Literal& other) { return this->literal == other.literal; }

  static std::shared_ptr<Literal> make(const Token& literal) {
    return std::make_shared<Literal>(literal);
  }
  static std::shared_ptr<Literal> makeNumber(const std::string& value) {
    return std::make_shared<Literal>(Token(TokenType::TOKEN_NUMBER, value));
  }
  static std::shared_ptr<Literal> makeString(const std::string& value) {
    return std::make_shared<Literal>(Token(TokenType::TOKEN_STRING, value));
  }
  static std::shared_ptr<Literal> makeTrue() {
    return std::make_shared<Literal>(Token(TokenType::TOKEN_TRUE, "true"));
  }
  static std::shared_ptr<Literal> makeFalse() {
    return std::make_shared<Literal>(Token(TokenType::TOKEN_FALSE, "false"));
  }
};
using LiteralPtr = std::shared_ptr<Literal>;

struct VariableExpr : public Expression {
  Token identifier;
  VariableExpr(const Token& identifier)
      : Expression(NodeType::VARIABLE_EXPRESSION), identifier(identifier) {}
  VariableExpr(Token&& identifier)
      : Expression(NodeType::VARIABLE_EXPRESSION), identifier(identifier) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherIdentifier = dynamic_cast<const VariableExpr&>(other);
      return isEqual(otherIdentifier);
    }
    return false;
  }

  bool isEqual(const VariableExpr& other) {
    return this->identifier == other.identifier;
  }

  static std::shared_ptr<VariableExpr> make(const Token& identifier) {
    return std::make_shared<VariableExpr>(identifier);
  }
  static std::shared_ptr<VariableExpr> make(const std::string& variableName) {
    return std::make_shared<VariableExpr>(
        Token(TokenType::TOKEN_IDENTIFIER, variableName));
  }
};
using VariableExprPtr = std::shared_ptr<VariableExpr>;

struct Assignment : public Expression {
  ExpressionPtr variable;
  ExpressionPtr value;

  Assignment(const ExpressionPtr& variable)
      : Expression(NodeType::ASSIGNMENT_EXPRESSION),
        variable(variable),
        value(nullptr) {}
  Assignment(const ExpressionPtr& variable, const ExpressionPtr& value)
      : Expression(NodeType::ASSIGNMENT_EXPRESSION),
        variable(variable),
        value(value) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherAssignment = dynamic_cast<const Assignment&>(other);
      return isEqual(otherAssignment);
    }
    return false;
  }

  bool isEqual(const Assignment& other) {
    if (!variable->isEqual(*other.variable)) {
      return false;
    }

    // compare initializers
    bool hasLhs = (bool)this->value;
    bool hasRhs = (bool)other.value;
    if (hasLhs != hasRhs) {
      return false;
    }

    return value->isEqual(*other.value);
  }

  static std::shared_ptr<Assignment> make(const ExpressionPtr& variable) {
    return std::make_shared<Assignment>(variable);
  }
  static std::shared_ptr<Assignment> make(const ExpressionPtr& variable,
                                          const ExpressionPtr& value) {
    return std::make_shared<Assignment>(variable, value);
  }
};
using AssignmentPtr = std::shared_ptr<Assignment>;

struct BinaryExpr : public Expression {
  ExpressionPtr left;
  Token operator_;
  ExpressionPtr right;

  BinaryExpr(const ExpressionPtr& left, const Token& operator_,
             const ExpressionPtr& right)
      : Expression(NodeType::BINARY_EXPRESSION),
        left(left),
        operator_(operator_),
        right(right) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherAssignment = dynamic_cast<const BinaryExpr&>(other);
      return isEqual(otherAssignment);
    }
    return false;
  }

  bool isEqual(const BinaryExpr& other) {
    if (!left->isEqual(*other.left)) {
      return false;
    }
    if (!operator_.isEqual(other.operator_)) {
      return false;
    }
    return right->isEqual(*other.right);
  }

  static std::shared_ptr<BinaryExpr> make(const ExpressionPtr& left,
                                          const Token& operator_,
                                          const ExpressionPtr& right) {
    return std::make_shared<BinaryExpr>(left, operator_, right);
  }
};
using BinaryExprPtr = std::shared_ptr<BinaryExpr>;

struct UnaryExpr : public Expression {
  Token operator_;
  ExpressionPtr right;

  UnaryExpr(const Token& operator_, const ExpressionPtr& right)
      : Expression(NodeType::UNARY_EXPRESSION),
        operator_(operator_),
        right(right) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherAssignment = dynamic_cast<const UnaryExpr&>(other);
      return isEqual(otherAssignment);
    }
    return false;
  }

  bool isEqual(const UnaryExpr& other) {
    if (!operator_.isEqual(other.operator_)) {
      return false;
    }
    return right->isEqual(*other.right);
  }

  static std::shared_ptr<UnaryExpr> make(const Token& operator_,
                                         const ExpressionPtr& right) {
    return std::make_shared<UnaryExpr>(operator_, right);
  }
};
using UnaryExprPtr = std::shared_ptr<UnaryExpr>;

struct Program : public Node {
  std::vector<StatementPtr> statements;

  Program() : Node(NodeType::PROGRAM), statements() {}
  Program(const std::vector<StatementPtr>& statements)
      : Node(NodeType::PROGRAM), statements(statements) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
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
using ProgramPtr = std::shared_ptr<Program>;

struct VarDeclaration : public Statement {
  Token identifier;
  ExpressionPtr initializer;

  VarDeclaration(const Token& identifier)
      : Statement(NodeType::VAR_DECLARATION),
        identifier(identifier),
        initializer(nullptr) {}
  VarDeclaration(const Token& identifier, const ExpressionPtr& initializer)
      : Statement(NodeType::VAR_DECLARATION),
        identifier(identifier),
        initializer(initializer) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
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
using VarDeclarationPtr = std::shared_ptr<VarDeclaration>;

struct Block : public Statement {
  std::vector<StatementPtr> statements;

  Block() : Statement(NodeType::BLOCK_STATEMENT), statements() {}
  Block(const std::vector<StatementPtr>& statements)
      : Statement(NodeType::BLOCK_STATEMENT), statements(statements) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
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
using BlockPtr = std::shared_ptr<Block>;

struct ForStatement : public Statement {
  ExpressionPtr initializer;
  ExpressionPtr condition;
  ExpressionPtr increment;
  StatementPtr body;

  ForStatement()
      : Statement(NodeType::FOR_STATEMENT),
        initializer(nullptr),
        condition(nullptr),
        increment(nullptr),
        body(nullptr) {}
  ForStatement(const ExpressionPtr& initializer, const ExpressionPtr& condition,
               const ExpressionPtr& increment, const StatementPtr& body)
      : Statement(NodeType::FOR_STATEMENT),
        initializer(initializer),
        condition(condition),
        increment(increment),
        body(body) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
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
using ForStatementPtr = std::shared_ptr<ForStatement>;

struct WhileStatement : public Statement {
  ExpressionPtr condition;
  StatementPtr body;

  WhileStatement()
      : Statement(NodeType::WHILE_STATEMENT),
        condition(nullptr),
        body(nullptr) {}
  WhileStatement(const ExpressionPtr& condition, const StatementPtr& body)
      : Statement(NodeType::WHILE_STATEMENT),
        condition(condition),
        body(body) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
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
using WhileStatementPtr = std::shared_ptr<WhileStatement>;

struct PrintStatement : public Statement {
  ExpressionPtr expression;

  PrintStatement()
      : Statement(NodeType::PRINT_STATEMENT), expression(nullptr) {}
  PrintStatement(const ExpressionPtr& expression)
      : Statement(NodeType::PRINT_STATEMENT), expression(expression) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
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
using PrintStatementPtr = std::shared_ptr<PrintStatement>;

struct IfStatement : public Statement {
  ExpressionPtr condition;
  StatementPtr thenBranch;
  StatementPtr elseBranch;

  IfStatement()
      : Statement(NodeType::IF_STATEMENT),
        condition(nullptr),
        thenBranch(nullptr),
        elseBranch(nullptr) {}
  IfStatement(const ExpressionPtr& condition, const StatementPtr& thenBranch)
      : Statement(NodeType::IF_STATEMENT),
        condition(condition),
        thenBranch(thenBranch),
        elseBranch(nullptr) {}
  IfStatement(const ExpressionPtr& condition, const StatementPtr& thenBranch,
              const StatementPtr& elseBranch)
      : Statement(NodeType::IF_STATEMENT),
        condition(condition),
        thenBranch(thenBranch),
        elseBranch(elseBranch) {}

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
    if (Type == other.Type) {
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
using IfStatementPtr = std::shared_ptr<IfStatement>;

struct ReturnStatement : public Statement {
  ExpressionPtr expression;

  ReturnStatement()
      : Statement(NodeType::RETURN_STATEMENT), expression(nullptr) {}
  ReturnStatement(const ExpressionPtr& expression)
      : Statement(NodeType::RETURN_STATEMENT), expression(expression) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherReturnStmt = dynamic_cast<const ReturnStatement&>(other);
      return isEqual(otherReturnStmt);
    }
    return false;
  }

  bool isEqual(const ReturnStatement& other) {
    bool hasLhs = (bool)expression;
    bool hasRhs = (bool)other.expression;
    if (hasLhs != hasRhs) {
      return false;
    }
    if (hasLhs) {
      return expression->isEqual(*other.expression);
    }
    return true;
  }

  static std::shared_ptr<ReturnStatement> make() {
    return std::make_shared<ReturnStatement>();
  }
  static std::shared_ptr<ReturnStatement> make(
      const ExpressionPtr& expression) {
    return std::make_shared<ReturnStatement>(expression);
  }
};
using ReturnStatementPtr = std::shared_ptr<ReturnStatement>;

struct ExpressionStatement : public Statement {
  ExpressionPtr expression;

  ExpressionStatement()
      : Statement(NodeType::EXPRESSION_STATEMENT), expression(nullptr) {}
  ExpressionStatement(const ExpressionPtr& expression)
      : Statement(NodeType::EXPRESSION_STATEMENT), expression(expression) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
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
using ExpressionStatementPtr = std::shared_ptr<ExpressionStatement>;

template <typename T>
class NodeVisitor {
  virtual T visitBlockStmt(BlockPtr stmt);
  virtual T visitExpressionStatement(ExpressionPtr stmt);
  virtual T visitForStatement(ForStatementPtr stmt);
};

}  // namespace cpplox::ast

#endif