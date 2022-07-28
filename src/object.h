#ifndef __cpplox_object_h
#define __cpplox_object_h

#include "common.h"

enum class ObjectType {
  OBJ_EMPTY = 0,
  OBJ_INTEGER,
  OBJ_BOOLEAN,
  OBJ_STRING,
  OBJ_NULL,
  OBJ_RETURN_VALUE,
  OBJ_ERROR,
  OBJ_FUNCTION,
  OBJ_ARRAY,
  OBJ_NATIVE,
};

struct Object {
  const ObjectType Type;

 public:
  Object() : Type(ObjectType::OBJ_EMPTY) {}
  Object(const Object &obj) : Type(obj.Type) {}
  Object(const ObjectType type) : Type(type) {}

  virtual std::string toString() const { return std::string(); }
  virtual bool isFalsey() const { return true; }
  virtual bool isTruthy() const { return false; }
  virtual bool isEqual(const Object &obj) const { return true; }

 protected:
  friend bool operator==(const Object &, const Object &);
};

typedef std::shared_ptr<Object> ObjectPtr;
typedef std::weak_ptr<Object> ObjectWeakPtr;

bool operator==(const Object &lhs, const Object &rhs);

struct IntegerObject : public Object {
 public:
  int64_t Value;

  IntegerObject(const int64_t value)
      : Object(ObjectType::OBJ_INTEGER), Value(value) {}

  std::string toString() const override {
    std::ostringstream ss;
    ss << Value;
    return ss.str();
  }

  bool isFalsey() const override { return Value == 0; }
  bool isTruthy() const override { return Value != 0; }
  bool isEqual(const Object &obj) const override {
    if (obj.Type == Type) {
      const auto &rhs = dynamic_cast<const IntegerObject &>(obj);
      return Value == rhs.Value;
    }
    return false;
  }
};

#endif  // __cpplox_object_h