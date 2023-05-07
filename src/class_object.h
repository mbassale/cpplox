#pragma once

#include "ast.h"
#include "common.h"
#include "object.h"
#include "function.h"

struct ClassObject : public Object {
  ClassDeclarationPtr declaration;
  std::vector<FunctionPtr> methods;

  ClassObject(ClassDeclarationPtr declaration)
      : Object(ObjectType::OBJ_CLASS), declaration(declaration) {}

  std::string toString() const override { return "<class " + declaration->identifier + ">"; }

  bool isFalsey() const override { return false; }
  bool isTruthy() const override { return true; }

  bool isEqual(const Object &obj) const override;
  bool isEqual(const ClassObject &other) const;

  static std::shared_ptr<ClassObject> make(ClassDeclarationPtr declaration);
};
using ClassObjectPtr = std::shared_ptr<ClassObject>;