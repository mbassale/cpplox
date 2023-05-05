#pragma once

#include "common.h"
#include "object.h"
#include "function.h"

class Record : public Object {
 public:
  cpplox::ast::ClassDeclarationPtr classDecl;
  const std::string name;
  std::unordered_map<std::string, ObjectPtr> fields;
  std::unordered_map<std::string, FunctionPtr> methods;

  Record(cpplox::ast::ClassDeclarationPtr classDecl);

  std::string toString() const override { return name; }
  bool isFalsey() const override;
  bool isTruthy() const override;

  bool isEqual(const Object &obj) const override {
    if (obj.Type != ObjectType::OBJ_RECORD) {
      return false;
    }
    auto other = dynamic_cast<const Record &>(obj);
    return name == other.name;
  }
};

using RecordPtr = std::shared_ptr<Record>;