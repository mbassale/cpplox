#ifndef __cpplox_closure_h
#define __cpplox_closure_h

#include "common.h"
#include "function.h"

class Closure : public Object
{
private:
    FunctionPtr function;

public:
    Closure() : Object(ObjectType::OBJ_CLOSURE) {}
    Closure(const Closure &closure) : Object(ObjectType::OBJ_CLOSURE), function(closure.function) {}
    Closure(FunctionPtr function) : Object(ObjectType::OBJ_CLOSURE), function(function) {}

    inline FunctionPtr getFunction() const { return function; }

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

#endif // __cpplox_closure_h