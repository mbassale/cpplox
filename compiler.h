#ifndef __cpplox_compiler_h
#define __cpplox_compiler_h

#include "common.h"
#include "scanner.h"

class Compiler
{
public:
    explicit Compiler() {}

    void compile(const std::string &source);
};

#endif // __cpplox_compiler_h