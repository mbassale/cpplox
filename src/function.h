#ifndef __cpplox_function_h
#define __cpplox_function_h

#include "chunk.h"
#include "common.h"
#include "object.h"

enum FunctionType { TYPE_SCRIPT, TYPE_FUNCTION };

class Function : public Object {
 private:
  FunctionType functionType;
  std::string name;
  int arity;
  int upvalueCount;
  Chunk chunk;

 public:
  Function()
      : Object(ObjectType::OBJ_FUNCTION),
        functionType(TYPE_FUNCTION),
        arity(0),
        upvalueCount(0),
        name("anonymous"),
        chunk(name) {}
  Function(const Function &function)
      : Object(ObjectType::OBJ_FUNCTION),
        functionType(function.functionType),
        arity(function.arity),
        upvalueCount(function.upvalueCount),
        name(function.name),
        chunk(function.chunk) {}
  Function(FunctionType functionType, const std::string &name, int arity = 0,
           int upvalueCount = 0)
      : Object(ObjectType::OBJ_FUNCTION),
        functionType(functionType),
        name(name),
        chunk(name),
        arity(arity),
        upvalueCount(upvalueCount) {}

  inline FunctionType getType() const { return functionType; }
  inline const std::string &getName() const { return name; }
  inline Chunk &getChunk() { return chunk; }
  inline int getArity() const { return arity; }
  inline int incrArity() { return ++arity; }
  inline int getUpvalueCount() const { return upvalueCount; }
  inline int incrUpvalueCount() { return ++upvalueCount; }

  std::string toString() const override;
  bool isFalsey() const override;
  virtual bool isTruthy() const override;
  virtual bool isEqual(const Object &obj) const override;
  inline bool isEqual(const Function &other) const;

 protected:
  friend bool operator==(const Function &lhs, const Function &rhs);
};

typedef std::shared_ptr<Function> FunctionPtr;
typedef std::weak_ptr<Function> FunctionWeakPtr;

bool operator==(const Function &lhs, const Function &rhs);

#endif  // __cpplox_function_h