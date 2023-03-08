#ifndef __cpplox_function_h
#define __cpplox_function_h

#include "ast.h"
#include "common.h"
#include "object.h"

namespace cpplox {

enum FunctionType { TYPE_SCRIPT, TYPE_FUNCTION };

class Function : public Object {
 private:
  FunctionType functionType;
  ast::FunctionDeclarationPtr declaration;
  std::string name;
  int arity;

 public:
  Function(FunctionType functionType, ast::FunctionDeclarationPtr declaration,
           const std::string &name, int arity = 0)
      : Object(ObjectType::OBJ_FUNCTION),
        functionType(functionType),
        declaration(declaration),
        name(name),
        arity(arity) {}

  inline FunctionType getType() const { return functionType; }
  inline const std::string &getName() const { return name; }
  inline int getArity() const { return arity; }
  inline int incrArity() { return ++arity; }
  inline ast::FunctionDeclarationPtr getDeclaration() { return declaration; }

  std::string toString() const override;
  bool isFalsey() const override;
  virtual bool isTruthy() const override;
  virtual bool isEqual(const Object &obj) const override;
  inline bool isEqual(const Function &other) const;

  static std::shared_ptr<Function> make(FunctionType functionType,
                                        ast::FunctionDeclarationPtr declaration,
                                        const std::string &name, int arity);
};

using FunctionPtr = std::shared_ptr<Function>;

}  // namespace cpplox

#endif  // __cpplox_function_h