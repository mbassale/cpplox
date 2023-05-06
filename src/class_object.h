#pragma once

#include "ast.h"
#include "common.h"

struct ClassObject : public Object {
  cpplox::ast::ClassDeclarationPtr declaration;

  ClassObject(cpplox::ast::ClassDeclarationPtr declaration)
      : Object(ObjectType::OBJ_CLASS), declaration(declaration) {}

  std::string toString() const override { return "<class " + declaration->identifier + ">"; }

  bool isFalsey() const override { return false; }
  bool isTruthy() const override { return true; }

  bool isEqual(const Object &obj) const override;
  bool isEqual(const ClassObject &other) const;

  static std::shared_ptr<ClassObject> make(cpplox::ast::ClassDeclarationPtr declaration);
};
using ClassObjectPtr = std::shared_ptr<ClassObject>;