#pragma once

#include "common.h"
#include "environment.h"
#include "function.h"
#include "object.h"

class Record : public Object {
 public:
  EnvironmentPtr ctx;
  ClassDeclarationPtr classDecl;
  std::unordered_map<std::string, ObjectPtr> fields;
  std::unordered_map<std::string, FunctionPtr> methods;

  Record(EnvironmentPtr enclosingCtx, ClassDeclarationPtr classDecl);

  std::string toString() const override {
    return "<record " + classDecl->identifier + ">";
  }
  bool isFalsey() const override;
  bool isTruthy() const override;

  bool isEqual(const Object &obj) const override {
    if (obj.Type != ObjectType::OBJ_RECORD) {
      return false;
    }
    auto other = dynamic_cast<const Record &>(obj);
    assert(other.classDecl != nullptr);
    return classDecl->isEqual(*other.classDecl);
  }

  static std::shared_ptr<Record> make(EnvironmentPtr enclosingCtx,
                                      ClassDeclarationPtr classDecl);
};

using RecordPtr = std::shared_ptr<Record>;