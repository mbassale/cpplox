#include "class_object.h"

bool ClassObject::isEqual(const Object &obj) const {
  if (typeid(obj) == typeid(*this)) {
    auto &other = reinterpret_cast<const ClassObject &>(obj);
    return isEqual(other);
  }
  return false;
}

bool ClassObject::isEqual(const ClassObject &other) const {
  if (declaration == nullptr) {
    return other.declaration == nullptr;
  }
  return declaration->isEqual(*other.declaration);
}

std::shared_ptr<ClassObject> ClassObject::make(
    cpplox::ast::ClassDeclarationPtr declaration) {
  return std::make_shared<ClassObject>(declaration);
}