#include "record.h"

using cpplox::ast::ClassDeclarationPtr;

Record::Record(ClassDeclarationPtr classDecl)
    : Object(ObjectType::OBJ_RECORD), classDecl(classDecl) {
  for (auto& method : classDecl->methods) {
    const auto& methodName = method->identifier.lexeme();
    const auto arity = method->params.size();
    methods[methodName] =
        Function::make(TYPE_METHOD, method, methodName, arity);
  }
}

bool Record::isFalsey() const { return fields.empty() && methods.empty(); }

bool Record::isTruthy() const { return !fields.empty() || !methods.empty(); }

std::shared_ptr<Record> Record::make(
    cpplox::ast::ClassDeclarationPtr classDecl) {
  return std::make_shared<Record>(classDecl);
}