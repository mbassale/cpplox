#pragma once

#include "ast.h"
#include "common.h"
#include "environment.h"
#include "object.h"

enum FunctionType { TYPE_SCRIPT, TYPE_FUNCTION, TYPE_METHOD, TYPE_INITIALIZER };

class Function : public Object {
 private:
  EnvironmentPtr enclosingCtx;
  EnvironmentPtr ctx;
  FunctionType functionType;
  FunctionDeclarationPtr declaration;
  std::string name;
  int arity;

 public:
  Function(EnvironmentPtr enclosingCtx, FunctionType functionType,
           FunctionDeclarationPtr declaration, const std::string &name,
           int arity = 0);

  inline FunctionType getType() const { return functionType; }
  inline const std::string &getName() const { return name; }
  inline int getArity() const { return arity; }
  inline int incrArity() { return ++arity; }
  inline FunctionDeclarationPtr getDeclaration() { return declaration; }

  std::string toString() const override;
  bool isFalsey() const override;
  virtual bool isTruthy() const override;
  virtual bool isEqual(const Object &obj) const override;
  inline bool isEqual(const Function &other) const;
  inline EnvironmentPtr getCtx() const { return ctx; }

  static std::shared_ptr<Function> make(EnvironmentPtr enclosingCtx,
                                        FunctionType functionType,
                                        FunctionDeclarationPtr declaration,
                                        const std::string &name, int arity);
};

using FunctionPtr = std::shared_ptr<Function>;
