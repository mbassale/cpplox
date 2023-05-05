#include "record.h"

using cpplox::ast::ClassDeclarationPtr;

Record::Record(ClassDeclarationPtr classDecl)
    : Object(ObjectType::OBJ_RECORD),
      classDecl(classDecl),
      name(classDecl->identifier) {
  for (auto& method : classDecl->methods) {
    const auto& methodName = method->identifier.lexeme();
    const auto arity = method->params.size();
    methods[methodName] =
        Function::make(TYPE_METHOD, method, methodName, arity);
  }
}

bool Record::isFalsey() const { return fields.empty() && methods.empty(); }

bool Record::isTruthy() const { return !fields.empty() || !methods.empty(); }
