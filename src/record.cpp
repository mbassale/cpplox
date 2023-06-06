#include "record.h"

Record::Record(EnvironmentPtr ctx, ClassDeclarationPtr classDecl)
    : Object(ObjectType::OBJ_RECORD),
      ctx{ctx},
      classDecl{classDecl},
      fields{},
      methods{} {}

bool Record::isFalsey() const { return fields.empty() && methods.empty(); }

bool Record::isTruthy() const { return !fields.empty() || !methods.empty(); }

std::shared_ptr<Record> Record::make(EnvironmentPtr ctx,
                                     ClassDeclarationPtr classDecl) {
  return std::make_shared<Record>(ctx, classDecl);
}