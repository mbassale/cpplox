#ifndef __cpplox_object_h
#define __cpplox_object_h

#include "common.h"

enum ObjectType
{
    OBJ_EMPTY = 0,
    OBJ_FUNCTION,
    OBJ_NATIVE,
    OBJ_CLOSURE,
};

class Object
{
private:
    ObjectType type;

public:
    Object() : type(OBJ_EMPTY) {}
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

#endif // __cpplox_object_h