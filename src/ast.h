#pragma once

#include "common.h"
#include "location.h"
#include "object.h"
#include "token.h"

enum class NodeType {

  /*
   * COMPILATION UNITS
   */
  PROGRAM,

  /*
   * DECLARATIONS
   */
  VAR_DECLARATION,
  FUNCTION_DECLARATION,
  CLASS_DECLARATION,

  /*
   * EXPRESSIONS
   */
  EXPRESSION,
  VARIABLE_EXPRESSION,
  MEMBER_EXPRESSION,
  ASSIGNMENT_EXPRESSION,
  BINARY_EXPRESSION,
  UNARY_EXPRESSION,
  CALL_EXPRESSION,
  INTEGER_LITERAL,
  BOOLEAN_LITERAL,
  STRING_LITERAL,
  ARRAY_LITERAL,
  ARRAY_SUBSCRIPT_EXPRESSION,
  NIL_LITERAL,

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
  BREAK_STATEMENT,
  CONTINUE_STATEMENT,
};

struct Node {
  const NodeType Type;

  Node() : Type(NodeType::EMPTY_STATEMENT) {}
  Node(const NodeType type) : Type(type) {}

  virtual bool isEqual(const Node& other) { return true; }

  virtual std::string toString() const { return "(Node)"; }
};
using NodePtr = std::shared_ptr<Node>;

struct Statement : public Node {
  Statement() : Node(NodeType::EMPTY_STATEMENT) {}
  Statement(NodeType type) : Node(type) {}

  static std::shared_ptr<Statement> make() {
    return std::make_shared<Statement>();
  }

  virtual std::string toString() const override { return "(Statement)"; }
};
using StatementPtr = std::shared_ptr<Statement>;

struct Expression : public Node {
  Expression(NodeType type) : Node(type) {}

  virtual std::string toString() const override { return "(Expression)"; }
};
using ExpressionPtr = std::shared_ptr<Expression>;

struct IntegerLiteral : public Expression {
  const int64_t Value;

  IntegerLiteral(const int64_t value)
      : Expression(NodeType::INTEGER_LITERAL), Value(value) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherLiteral = dynamic_cast<const IntegerLiteral&>(other);
      return isEqual(otherLiteral);
    }
    return false;
  }

  bool isEqual(const IntegerLiteral& other) { return Value == other.Value; }

  virtual std::string toString() const override {
    return "(IntegerLiteral " + std::to_string(Value) + ")";
  }

  static std::shared_ptr<IntegerLiteral> make(const int64_t value) {
    return std::make_shared<IntegerLiteral>(value);
  }
};
using IntegerLiteralPtr = std::shared_ptr<IntegerLiteral>;

struct BooleanLiteral : public Expression {
  const int64_t Value;

  BooleanLiteral(const int64_t value)
      : Expression(NodeType::BOOLEAN_LITERAL), Value(value) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherLiteral = dynamic_cast<const BooleanLiteral&>(other);
      return isEqual(otherLiteral);
    }
    return false;
  }

  bool isEqual(const BooleanLiteral& other) { return Value == other.Value; }

  std::string toString() const override {
    return "(BooleanLiteral " + std::to_string(Value) + ")";
  }

  static std::shared_ptr<BooleanLiteral> makeTrue() {
    return std::make_shared<BooleanLiteral>(true);
  }

  static std::shared_ptr<BooleanLiteral> makeFalse() {
    return std::make_shared<BooleanLiteral>(false);
  }

  static std::shared_ptr<BooleanLiteral> make(const bool value) {
    return std::make_shared<BooleanLiteral>(value);
  }
};
using BooleanLiteralPtr = std::shared_ptr<BooleanLiteral>;

struct StringLiteral : public Expression {
  const std::string Value;

  StringLiteral(const std::string& value)
      : Expression(NodeType::STRING_LITERAL), Value(value) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherLiteral = dynamic_cast<const StringLiteral&>(other);
      return isEqual(otherLiteral);
    }
    return false;
  }

  bool isEqual(const StringLiteral& other) { return Value == other.Value; }

  std::string toString() const override {
    return "(StringLiteral " + Value + ")";
  }

  static std::shared_ptr<StringLiteral> make(const std::string& value) {
    return std::make_shared<StringLiteral>(value);
  }
};
using StringLiteralPtr = std::shared_ptr<StringLiteral>;

struct NilLiteral : public Expression {
  NilLiteral() : Expression(NodeType::NIL_LITERAL) {}

  bool isEqual(const Node& other) override { return Type == other.Type; }

  bool isEqual(const NilLiteral& other) { return true; }

  std::string toString() const override { return "(NilLiteral)"; }

  static std::shared_ptr<NilLiteral> make() {
    return std::make_shared<NilLiteral>();
  }
};
using NilLiteralPtr = std::shared_ptr<NilLiteral>;

// NilLiteral expr also serves as a EmptyExpression.
// When we need a placeholder for an empty expression, for example, on for loop
// conditional and increment, we use the EmptyExpression
using EmptyExpression = NilLiteral;

struct VariableExpr : public Expression {
  std::string identifier;

  VariableExpr(const std::string& identifier)
      : Expression(NodeType::VARIABLE_EXPRESSION), identifier(identifier) {}
  VariableExpr(std::string&& identifier)
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

  std::string toString() const override {
    return "(VariableExpr " + identifier + ")";
  }

  static std::shared_ptr<VariableExpr> make(const std::string& variableName) {
    return std::make_shared<VariableExpr>(variableName);
  }
};
using VariableExprPtr = std::shared_ptr<VariableExpr>;

struct Assignment : public Expression {
  std::string identifier;
  ExpressionPtr value;

  Assignment(const std::string& identifier)
      : Expression(NodeType::ASSIGNMENT_EXPRESSION),
        identifier(identifier),
        value(nullptr) {}
  Assignment(const std::string& identifier, const ExpressionPtr& value)
      : Expression(NodeType::ASSIGNMENT_EXPRESSION),
        identifier(identifier),
        value(value) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherAssignment = dynamic_cast<const Assignment&>(other);
      return isEqual(otherAssignment);
    }
    return false;
  }

  bool isEqual(const Assignment& other) {
    if (identifier != other.identifier) {
      return false;
    }

    // compare values
    bool hasLhs = (bool)this->value;
    bool hasRhs = (bool)other.value;
    if (hasLhs != hasRhs) {
      return false;
    }

    return value->isEqual(*other.value);
  }

  std::string toString() const override {
    return "(Assignment " + identifier + " " + value->toString() + ")";
  }

  static std::shared_ptr<Assignment> make(const std::string& identifier) {
    return std::make_shared<Assignment>(identifier);
  }
  static std::shared_ptr<Assignment> make(const std::string& identifier,
                                          const ExpressionPtr& value) {
    return std::make_shared<Assignment>(identifier, value);
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
      const auto& otherExpr = dynamic_cast<const BinaryExpr&>(other);
      return isEqual(otherExpr);
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

  std::string toString() const override {
    return "(BinaryExpr " + left->toString() + " " + operator_.lexeme() + " " +
           right->toString() + ")";
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

  std::string toString() const override {
    return "(UnaryExpr " + operator_.lexeme() + " " + right->toString() + ")";
  }

  static std::shared_ptr<UnaryExpr> make(const Token& operator_,
                                         const ExpressionPtr& right) {
    return std::make_shared<UnaryExpr>(operator_, right);
  }
};
using UnaryExprPtr = std::shared_ptr<UnaryExpr>;

struct CallExpr : public Expression {
  ExpressionPtr left;
  std::vector<ExpressionPtr> arguments;

  CallExpr(const ExpressionPtr& left,
           const std::vector<ExpressionPtr>& arguments)
      : Expression(NodeType::CALL_EXPRESSION),
        left(left),
        arguments(arguments) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherCallExpr = dynamic_cast<const CallExpr&>(other);
      return isEqual(otherCallExpr);
    }
    return false;
  }

  bool isEqual(const CallExpr& other) {
    if (!left->isEqual(*other.left)) {
      return false;
    }
    if (arguments.size() != other.arguments.size()) {
      return false;
    }
    for (size_t i = 0; i < arguments.size(); i++) {
      if (!arguments[i]->isEqual(*other.arguments[i])) {
        return false;
      }
    }
    return true;
  }

  std::string toString() const override {
    std::string result = "(CallExpr " + left->toString();
    for (const auto& arg : arguments) {
      result += " " + arg->toString();
    }
    result += ")";
    return result;
  }

  static std::shared_ptr<CallExpr> make(
      const ExpressionPtr& left, const std::vector<ExpressionPtr>& arguments) {
    return std::make_shared<CallExpr>(left, arguments);
  }
};
using CallExprPtr = std::shared_ptr<CallExpr>;

struct MemberExpr : public Expression {
  VariableExprPtr left;
  std::string member;

  MemberExpr(const VariableExprPtr& left, const std::string& member)
      : Expression(NodeType::MEMBER_EXPRESSION), left(left), member(member) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherMemberExpr = dynamic_cast<const MemberExpr&>(other);
      return isEqual(otherMemberExpr);
    }
    return false;
  }

  bool isEqual(const MemberExpr& other) {
    if (!left->isEqual(*other.left)) {
      return false;
    }
    return member == other.member;
  }

  std::string toString() const override {
    return "(MemberExpr " + left->toString() + " " + member + ")";
  }

  static std::shared_ptr<MemberExpr> make(const VariableExprPtr& left,
                                          const std::string& member) {
    return std::make_shared<MemberExpr>(left, member);
  }
};
using MemberExprPtr = std::shared_ptr<MemberExpr>;

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
                        return stmt1 != nullptr && stmt2 != nullptr &&
                               stmt1->isEqual(*stmt2);
                      });
  }

  std::string toString() const override {
    std::string result = "(Program";
    for (const auto& stmt : statements) {
      result += " " + (stmt != nullptr ? stmt->toString() : "nullptr");
    }
    result += ")";
    return result;
  }

  static std::shared_ptr<Program> make(
      const std::vector<StatementPtr>& statements) {
    return std::make_shared<Program>(statements);
  }
};
using ProgramPtr = std::shared_ptr<Program>;

struct VarDeclaration : public Statement {
  std::string identifier;
  ExpressionPtr initializer;

  VarDeclaration(const std::string& identifier)
      : Statement(NodeType::VAR_DECLARATION),
        identifier(identifier),
        initializer(nullptr) {}
  VarDeclaration(const std::string& identifier,
                 const ExpressionPtr& initializer)
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
    if (identifier != other.identifier) {
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

  std::string toString() const override {
    std::string result = "(VarDeclaration " + identifier;
    if (initializer) {
      result += " " + initializer->toString();
    }
    result += ")";
    return result;
  }

  static std::shared_ptr<VarDeclaration> make(
      const std::string& identifier,
      const ExpressionPtr& initializer = nullptr) {
    return std::make_shared<VarDeclaration>(identifier, initializer);
  }
};
using VarDeclarationPtr = std::shared_ptr<VarDeclaration>;

struct FunctionDeclaration : public Statement {
  std::string identifier;
  std::vector<std::string> params;
  StatementPtr body;

  FunctionDeclaration(const std::string& identifier)
      : Statement(NodeType::FUNCTION_DECLARATION),
        identifier(identifier),
        params(),
        body(nullptr) {}
  FunctionDeclaration(const std::string& identifier,
                      const std::vector<std::string>& params,
                      const StatementPtr& body)
      : Statement(NodeType::FUNCTION_DECLARATION),
        identifier(identifier),
        params(params),
        body(body) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherProgram =
          dynamic_cast<const FunctionDeclaration&>(other);
      return isEqual(otherProgram);
    }
    return false;
  }

  bool isEqual(const FunctionDeclaration& other) {
    if (identifier != other.identifier) {
      return false;
    }

    // compare args
    if (params != other.params) {
      return false;
    }

    // compare body
    bool hasLhs = (bool)this->body;
    bool hasRhs = (bool)other.body;
    if (hasLhs != hasRhs) {
      return false;
    }
    if (this->body && other.body) {
      if (!this->body->isEqual(*other.body)) {
        return false;
      }
    }

    return true;
  }

  std::string toString() const override {
    std::string result = "(FunctionDeclaration " + identifier;
    for (const auto& param : params) {
      result += " " + param;
    }
    if (body) {
      result += " " + body->toString();
    }
    result += ")";
    return result;
  }

  static std::shared_ptr<FunctionDeclaration> make(
      const std::string& identifier, const std::vector<std::string>& params,
      const StatementPtr& body) {
    return std::make_shared<FunctionDeclaration>(identifier, params, body);
  }
};
using FunctionDeclarationPtr = std::shared_ptr<FunctionDeclaration>;

struct ClassDeclaration : public Statement {
  std::string identifier;
  FunctionDeclarationPtr ctor;
  std::vector<VarDeclarationPtr> fields;
  std::vector<FunctionDeclarationPtr> methods;

  ClassDeclaration(const std::string& identifier)
      : Statement(NodeType::CLASS_DECLARATION),
        identifier(identifier),
        methods() {}
  ClassDeclaration(const std::string& identifier,
                   const FunctionDeclarationPtr& ctor,
                   const std::vector<VarDeclarationPtr>& fields,
                   const std::vector<FunctionDeclarationPtr>& methods)
      : Statement(NodeType::CLASS_DECLARATION),
        identifier(identifier),
        ctor(ctor),
        fields(fields),
        methods(methods) {}

  bool isEqual(const ClassDeclaration& other) {
    if (identifier == other.identifier) {
      return true;
    }
    return false;
  }

  std::string toString() const {
    std::string result = "(ClassDeclaration " + identifier + " ";
    for (const auto& field : fields) {
      result += "var " + field->toString() + ", ";
    }
    for (const auto& method : methods) {
      result += "def " + method->toString() + ", ";
    }
    result += ")";
    return result;
  }

  static std::shared_ptr<ClassDeclaration> make(
      const std::string& identifier, const FunctionDeclarationPtr& ctor,
      const std::vector<VarDeclarationPtr>& fields,
      const std::vector<FunctionDeclarationPtr>& methods) {
    return std::make_shared<ClassDeclaration>(identifier, ctor, fields,
                                              methods);
  }
};
using ClassDeclarationPtr = std::shared_ptr<ClassDeclaration>;

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

  std::string toString() const override {
    std::string result = "(Block";
    for (const auto& stmt : statements) {
      result += " " + stmt->toString();
    }
    result += ")";
    return result;
  }

  static std::shared_ptr<Block> make(
      const std::vector<StatementPtr>& statements) {
    return std::make_shared<Block>(statements);
  }
};
using BlockPtr = std::shared_ptr<Block>;

struct ForStatement : public Statement {
  StatementPtr initializer;
  ExpressionPtr condition;
  ExpressionPtr increment;
  StatementPtr body;

  ForStatement()
      : Statement(NodeType::FOR_STATEMENT),
        initializer(nullptr),
        condition(nullptr),
        increment(nullptr),
        body(nullptr) {}
  ForStatement(const StatementPtr& initializer, const ExpressionPtr& condition,
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
      if (!this->increment->isEqual(*other.increment)) {
        return false;
      }
    }

    return body->isEqual(*other.body);
  }

  std::string toString() const override {
    std::string result = "(ForStatement";
    if (initializer) {
      result += " " + initializer->toString();
    }
    if (condition) {
      result += " " + condition->toString();
    }
    if (increment) {
      result += " " + increment->toString();
    }
    if (body) {
      result += " " + body->toString();
    }
    result += ")";
    return result;
  }

  static std::shared_ptr<ForStatement> make(const StatementPtr& initializer,
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

  std::string toString() const override {
    std::string result = "(WhileStatement";
    if (condition) {
      result += " " + condition->toString();
    }
    if (body) {
      result += " " + body->toString();
    }
    result += ")";
    return result;
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

  std::string toString() const override {
    std::string result = "(PrintStatement";
    if (expression) {
      result += " " + expression->toString();
    }
    result += ")";
    return result;
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

  std::string toString() const override {
    std::string result = "(IfStatement";
    if (condition) {
      result += " " + condition->toString();
    }
    if (thenBranch) {
      result += " " + thenBranch->toString();
    }
    if (elseBranch) {
      result += " " + elseBranch->toString();
    }
    result += ")";
    return result;
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

  std::string toString() const override {
    std::string result = "(ReturnStatement";
    if (expression) {
      result += " " + expression->toString();
    }
    result += ")";
    return result;
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

struct BreakStatement : public Statement {
  BreakStatement() : Statement(NodeType::BREAK_STATEMENT) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherReturnStmt = dynamic_cast<const BreakStatement&>(other);
      return isEqual(otherReturnStmt);
    }
    return false;
  }

  bool isEqual(const BreakStatement& other) { return true; }

  std::string toString() const override { return "(BreakStatement)"; }

  static std::shared_ptr<BreakStatement> make() {
    return std::make_shared<BreakStatement>();
  }
};
using BreakStatementPtr = std::shared_ptr<BreakStatement>;

struct ContinueStatement : public Statement {
  ContinueStatement() : Statement(NodeType::CONTINUE_STATEMENT) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherReturnStmt =
          dynamic_cast<const ContinueStatement&>(other);
      return isEqual(otherReturnStmt);
    }
    return false;
  }

  bool isEqual(const ContinueStatement& other) { return true; }

  std::string toString() const override { return "(ContinueStatement)"; }

  static std::shared_ptr<ContinueStatement> make() {
    return std::make_shared<ContinueStatement>();
  }
};
using ContinueStatementPtr = std::shared_ptr<ContinueStatement>;

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

  std::string toString() const override {
    std::string result = "(ExpressionStatement";
    if (expression) {
      result += " " + expression->toString();
    }
    result += ")";
    return result;
  }

  static std::shared_ptr<ExpressionStatement> make(
      const ExpressionPtr& expression) {
    return std::make_shared<ExpressionStatement>(expression);
  }
};
using ExpressionStatementPtr = std::shared_ptr<ExpressionStatement>;

struct ArrayLiteral : public Expression {
  std::vector<ExpressionPtr> elements;

  ArrayLiteral() : Expression(NodeType::ARRAY_LITERAL), elements() {}
  ArrayLiteral(const std::vector<ExpressionPtr>& elements)
      : Expression(NodeType::ARRAY_LITERAL), elements(elements) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherExprStmt = dynamic_cast<const ArrayLiteral&>(other);
      return isEqual(otherExprStmt);
    }
    return false;
  }

  bool isEqual(const ArrayLiteral& other) {
    if (elements.size() != other.elements.size()) {
      return false;
    }
    for (size_t i = 0; i < elements.size(); ++i) {
      if (!elements[i]->isEqual(*other.elements[i])) {
        return false;
      }
    }
    return true;
  }

  std::string toString() const override {
    std::string result = "(ArrayLiteral";
    for (const auto& element : elements) {
      result += " " + element->toString();
    }
    result += ")";
    return result;
  }

  static std::shared_ptr<ArrayLiteral> make(
      const std::vector<ExpressionPtr>& elements = {}) {
    return std::make_shared<ArrayLiteral>(elements);
  }
};
using ArrayLiteralPtr = std::shared_ptr<ArrayLiteral>;

struct ArraySubscriptExpr : public Expression {
  ExpressionPtr array;
  ExpressionPtr index;

  ArraySubscriptExpr()
      : Expression(NodeType::ARRAY_SUBSCRIPT_EXPRESSION),
        array(nullptr),
        index(nullptr) {}
  ArraySubscriptExpr(const ExpressionPtr& array, const ExpressionPtr& index)
      : Expression(NodeType::ARRAY_SUBSCRIPT_EXPRESSION),
        array(array),
        index(index) {}

  bool isEqual(const Node& other) override {
    if (Type == other.Type) {
      const auto& otherExprStmt =
          dynamic_cast<const ArraySubscriptExpr&>(other);
      return isEqual(otherExprStmt);
    }
    return false;
  }

  std::string toString() const override {
    std::string result = "(ArraySubscriptExpr";
    if (array) {
      result += " " + array->toString();
    }
    if (index) {
      result += " " + index->toString();
    }
    result += ")";
    return result;
  }

  bool isEqual(const ArraySubscriptExpr& other) {
    return array->isEqual(*other.array) && index->isEqual(*other.index);
  }

  static std::shared_ptr<ArraySubscriptExpr> make(const ExpressionPtr& array,
                                                  const ExpressionPtr& index) {
    return std::make_shared<ArraySubscriptExpr>(array, index);
  }
};

using ArraySubscriptExprPtr = std::shared_ptr<ArraySubscriptExpr>;

template <typename T>
class NodeVisitor {
  virtual T visitBlockStmt(BlockPtr stmt);
  virtual T visitExpressionStatement(ExpressionPtr stmt);
  virtual T visitForStatement(ForStatementPtr stmt);
};
