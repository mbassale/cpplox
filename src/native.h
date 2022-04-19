#ifndef __cpplox_native_h
#define __cpplox_native_h

#include "common.h"
#include "object.h"
#include "chunk.h"

typedef Value (*NativeFnPtr)(int argCount, Value *args);

class NativeFunction : public Object
{
private:
    std::string name;
    NativeFnPtr functionPtr;

public:
    NativeFunction() : name(), functionPtr(nullptr) {}
    NativeFunction(const std::string &name, NativeFnPtr functionPtr) : name(name), functionPtr(functionPtr) {}
    NativeFunction(const NativeFunction &function) : name(function.name), functionPtr(function.functionPtr) {}

    inline const std::string &getName() const { return name; }
    inline NativeFnPtr getFunctionPtr() const { return functionPtr; }

    std::string toString() const override;
    bool isFalsey() const override;
    virtual bool isTruthy() const override;
    virtual bool isEqual(const Object &obj) const override;
    inline bool isEqual(const NativeFunction &other) const;

protected:
    friend bool operator==(const NativeFunction &lhs, const NativeFunction &rhs);
};

typedef std::shared_ptr<NativeFunction> NativeFunctionPtr;
typedef std::weak_ptr<NativeFunction> NativeFunctionWeakPtr;

bool operator==(const NativeFunction &lhs, const NativeFunction &rhs);

#endif // __cpplox_native_h