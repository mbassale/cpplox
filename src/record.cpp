#include "record.h"

Record::Record(EnvironmentPtr ctx, ClassDeclarationPtr classDecl,
               std::unordered_map<std::string, ObjectPtr> fields,
               std::unordered_map<std::string, FunctionPtr> methods)
    : Object(ObjectType::OBJ_RECORD),
      ctx(ctx),
      classDecl(classDecl),
      fields(fields),
      methods(methods) {}

bool Record::isFalsey() const { return fields.empty() && methods.empty(); }

bool Record::isTruthy() const { return !fields.empty() || !methods.empty(); }

std::shared_ptr<Record> Record::make(
    EnvironmentPtr ctx, ClassDeclarationPtr classDecl,
    std::unordered_map<std::string, ObjectPtr> fields,
    std::unordered_map<std::string, FunctionPtr> methods) {
  return std::make_shared<Record>(ctx, classDecl, fields, methods);
}