#pragma once

#include "ast.h"
#include "common.h"
#include "object.h"

enum FunctionType { TYPE_SCRIPT, TYPE_FUNCTION, TYPE_METHOD, TYPE_INITIALIZER };

class Function : public Object {
 private:
  FunctionType functionType;
  cpplox::ast::FunctionDeclarationPtr declaration;
  std::string name;
  int arity;

 public:
  Function(FunctionType functionType, cpplox::ast::FunctionDeclarationPtr declaration,
           const std::string &name, int arity = 0)
      : Object(ObjectType::OBJ_FUNCTION),
        functionType(functionType),
        declaration(declaration),
        name(name),
        arity(arity) {}

  inline FunctionType getType() const { return functionType; }
  inline const std::string &getName() const { return name; }
  inline int getArity() const { return arity; }
  inline int incrArity() { return ++arity; }
  inline cpplox::ast::FunctionDeclarationPtr getDeclaration() { return declaration; }

  std::string toString() const override;
  bool isFalsey() const override;
  virtual bool isTruthy() const override;
  virtual bool isEqual(const Object &obj) const override;
  inline bool isEqual(const Function &other) const;

  static std::shared_ptr<Function> make(FunctionType functionType,
                                        cpplox::ast::FunctionDeclarationPtr declaration,
                                        const std::string &name, int arity);
};

using FunctionPtr = std::shared_ptr<Function>;
