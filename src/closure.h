#ifndef __cpplox_closure_h
#define __cpplox_closure_h

#include "common.h"
#include "function.h"
#include "object.h"
#include "upvalue.h"

class Closure : public Object {
 private:
  FunctionPtr function;
  std::vector<UpvaluePtr> upvalues;

 public:
  Closure() : Object(ObjectType::OBJ_CLOSURE), function(nullptr), upvalues() {}
  Closure(const Closure &closure)
      : Object(ObjectType::OBJ_CLOSURE),
        function(closure.function),
        upvalues(closure.upvalues) {}
  Closure(FunctionPtr function)
      : Object(ObjectType::OBJ_CLOSURE), function(function), upvalues() {
    for (int i = 0; i < function->getUpvalueCount(); i++) {
      upvalues.push_back(nullptr);
    }
  }

  inline FunctionPtr getFunction() const { return function; }
  inline FunctionType getType() const { return function->getType(); }
  inline const std::string &getName() const { return function->getName(); }
  inline Chunk &getChunk() { return function->getChunk(); }
  inline int getArity() const { return function->getArity(); }
  inline std::vector<UpvaluePtr> &getUpvalues() { return upvalues; }
  inline size_t getUpvalueCount() { return upvalues.size(); }

  std::string toString() const override;
  bool isFalsey() const override;
  virtual bool isTruthy() const override;
  virtual bool isEqual(const Object &obj) const override;
  inline bool isEqual(const Closure &other) const;

 protected:
  friend bool operator==(const Closure &lhs, const Closure &rhs);
};

typedef std::shared_ptr<Closure> ClosurePtr;
typedef std::weak_ptr<Closure> ClosureWeakPtr;

bool operator==(const Closure &lhs, const Closure &rhs);

#endif  // __cpplox_closure_h