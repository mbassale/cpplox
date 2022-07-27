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

class Object {
 private:
  ObjectType type;

 public:
  Object() : type(ObjectType::OBJ_EMPTY) {}
  Object(const Object &obj) : type(obj.type) {}
  Object(ObjectType type) : type(type) {}

  inline ObjectType getType() const { return type; }
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

#endif  // __cpplox_object_h