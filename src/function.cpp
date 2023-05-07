#include "function.h"

Function::Function(EnvironmentPtr enclosingCtx, FunctionType functionType,
                   FunctionDeclarationPtr declaration, const std::string &name,
                   int arity)
    : Object(ObjectType::OBJ_FUNCTION),
      enclosingCtx(enclosingCtx),
      functionType(functionType),
      declaration(declaration),
      name(name),
      arity(arity) {
  ctx = Environment::make(enclosingCtx);
}

std::shared_ptr<Function> Function::make(EnvironmentPtr enclosingCtx,
                                         FunctionType functionType,
                                         FunctionDeclarationPtr declaration,
                                         const std::string &name, int arity) {
  return std::make_shared<Function>(enclosingCtx, functionType, declaration,
                                    name, arity);
}

std::string Function::toString() const {
  std::ostringstream ss;
  ss << "<func " << name << "(#" << arity << ")>";
  return ss.str();
}

bool Function::isFalsey() const { return true; }

bool Function::isTruthy() const { return false; }

bool Function::isEqual(const Object &obj) const {
  if (typeid(obj) == typeid(*this)) {
    auto &other = reinterpret_cast<const Function &>(obj);
    return isEqual(other);
  }
  return false;
}

bool Function::isEqual(const Function &other) const {
  // TODO: implement equality for chunks
  return arity == other.arity && name == other.name;
}