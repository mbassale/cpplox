#ifndef __cpplox_upvalue_h
#define __cpplox_upvalue_h

#include "common.h"
#include "object.h"
#include "value.h"

class Upvalue : public Object {
 private:
  Value *location;

 public:
  Upvalue() : Object(ObjectType::OBJ_UPVALUE) {}
  Upvalue(const Upvalue &upvalue) : Object(ObjectType::OBJ_UPVALUE) {}
  Upvalue(Value *location)
      : Object(ObjectType::OBJ_UPVALUE), location(location) {}

  Value *getLocation() { return location; }
  std::string toString() const override;
  bool isFalsey() const override;
  virtual bool isTruthy() const override;
  virtual bool isEqual(const Object &obj) const override;
  inline bool isEqual(const Upvalue &other) const;

 protected:
  friend bool operator==(const Upvalue &lhs, const Upvalue &rhs);
};

typedef std::shared_ptr<Upvalue> UpvaluePtr;
typedef std::weak_ptr<Upvalue> UpvalueWeakPtr;

bool operator==(const Upvalue &lhs, const Upvalue &rhs);

#endif  // __cpplox_upvalue_h