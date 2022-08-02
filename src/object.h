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

  Object() : Type(ObjectType::OBJ_EMPTY) {}
  Object(const Object &obj) : Type(obj.Type) {}
  Object(const ObjectType type) : Type(type) {}

  virtual std::string toString() const { return std::string(); }
  virtual bool isFalsey() const { return true; }
  virtual bool isTruthy() const { return false; }
  virtual bool isEqual(const Object &obj) const { return true; }

 protected:
  friend bool operator==(const Object &, const Object &);
  friend bool operator!=(const Object &, const Object &);
};

typedef std::shared_ptr<Object> ObjectPtr;

bool operator==(const Object &lhs, const Object &rhs);
bool operator!=(const Object &lhs, const Object &rhs);

struct NullObject : public Object {
  NullObject() : Object(ObjectType::OBJ_NULL) {}

  virtual std::string toString() const { return "nil"; }
  virtual bool isFalsey() const { return true; }
  virtual bool isTruthy() const { return false; }
  virtual bool isEqual(const Object &obj) const {
    return obj.Type == ObjectType::OBJ_NULL;
  }
};

typedef std::shared_ptr<NullObject> NullObjectPtr;

struct IntegerObject : public Object {
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
      const auto &rhs = static_cast<const IntegerObject &>(obj);
      return Value == rhs.Value;
    }
    return false;
  }
};

typedef std::shared_ptr<IntegerObject> IntegerObjectPtr;

struct BooleanObject : public Object {
  bool Value;

  BooleanObject(const bool value)
      : Object(ObjectType::OBJ_INTEGER), Value(value) {}

  std::string toString() const override { return Value ? "true" : "false"; }

  bool isFalsey() const override { return !Value; }
  bool isTruthy() const override { return Value; }
  bool isEqual(const Object &obj) const override {
    if (obj.Type == Type) {
      const auto &rhs = static_cast<const BooleanObject &>(obj);
      return Value == rhs.Value;
    }
    return false;
  }
};

typedef std::shared_ptr<BooleanObject> BooleanObjectPtr;

struct StringObject : public Object {
  std::string Value;

  StringObject(const std::string &value)
      : Object(ObjectType::OBJ_STRING), Value(value) {}

  std::string toString() const override { return Value; }

  bool isFalsey() const override { return Value.length() == 0; }
  bool isTruthy() const override { return Value.length() > 0; }
  bool isEqual(const Object &obj) const override {
    if (obj.Type == Type) {
      const auto &rhs = static_cast<const StringObject &>(obj);
      return Value == rhs.Value;
    }
    return false;
  }
};

static auto NULL_OBJECT_PTR = std::make_shared<NullObject>();
static auto TRUE_OBJECT_PTR = std::make_shared<BooleanObject>(true);
static auto FALSE_OBJECT_PTR = std::make_shared<BooleanObject>(false);

#endif  // __cpplox_object_h