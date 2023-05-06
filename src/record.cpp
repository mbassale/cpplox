#include "record.h"

using cpplox::ast::ClassDeclarationPtr;

Record::Record(EnvironmentPtr enclosingCtx, ClassDeclarationPtr classDecl)
    : Object(ObjectType::OBJ_RECORD), classDecl(classDecl) {
  ctx = Environment::make(enclosingCtx);
  for (auto& method : classDecl->methods) {
    const auto& methodName = method->identifier.lexeme();
    const auto arity = method->params.size();
    methods[methodName] =
        Function::make(ctx, TYPE_METHOD, method, methodName, arity);
  }
}

bool Record::isFalsey() const { return fields.empty() && methods.empty(); }

bool Record::isTruthy() const { return !fields.empty() || !methods.empty(); }

std::shared_ptr<Record> Record::make(
    EnvironmentPtr enclosingCtx, cpplox::ast::ClassDeclarationPtr classDecl) {
  return std::make_shared<Record>(enclosingCtx, classDecl);
}