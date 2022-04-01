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
    Function(FunctionType type, const std::string &name, int arity = 0) : type(type), chunk(name), arity(arity) {}

    inline Chunk &getChunk() { return chunk; }
};

typedef std::unique_ptr<Function> FunctionPtr;

#endif // __cpplox_function_h