#ifndef __cpplox_function_h
#define __cpplox_function_h

#include "common.h"
#include "object.h"
#include "chunk.h"

enum FunctionType
{
    TYPE_SCRIPT,
    TYPE_FUNCTION
};

class Function : public Object
{
private:
    FunctionType type;
    std::string name;
    int arity;
    Chunk chunk;

public:
    Function() : type(TYPE_FUNCTION), arity(0), name("anonymous"), chunk(name) {}
    Function(const Function &function) : type(function.type), arity(function.arity), name(function.name), chunk(function.chunk) {}
    Function(FunctionType type, const std::string &name, int arity = 0) : type(type), name(name), chunk(name), arity(arity) {}

    inline Chunk &getChunk() { return chunk; }
    inline int getArity() const { return arity; }
    inline int incrArity() { return ++arity; }

    std::string toString() const override;
    bool isFalsey() const override;
    virtual bool isTruthy() const override;
    virtual bool isEqual(const Object &obj) const override;
    inline bool isEqual(const Function &other) const;

protected:
    friend bool operator==(const Function &lhs, const Function &rhs);
};

typedef std::shared_ptr<Function> FunctionPtr;
typedef std::weak_ptr<Function> FunctionWeakPtr;

bool operator==(const Function &lhs, const Function &rhs);

#endif // __cpplox_function_h