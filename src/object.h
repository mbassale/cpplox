#ifndef __cpplox_object_h
#define __cpplox_object_h

#include "common.h"

class Object
{
public:
    Object() {}
    Object(const Object &) {}

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