#ifndef __cpplox_object_h
#define __cpplox_object_h

#include "common.h"

class Object
{
public:
    Object() {}
    Object(const Object &) {}

    std::string toString() const { return std::string(); }
    bool isFalsey() const { return true; }
    bool isTruthy() const { return false; }

protected:
    friend bool operator==(const Object &, const Object &);
    virtual bool isEqual(const Object &obj) const { return false; }
};

typedef std::unique_ptr<Object> ObjectPtr;

bool operator==(const Object &lhs, const Object &rhs);

#endif // __cpplox_object_h