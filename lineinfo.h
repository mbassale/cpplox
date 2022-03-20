#ifndef __cpplox_lineinfo_h
#define __cpplox_lineinfo_h

#include "common.h"

struct LineEntry
{
    size_t line;
    size_t count;
};

class LineInfo
{
    std::vector<LineEntry> lines;

public:
    LineInfo() : lines() {}
    size_t get(size_t offset);
    void write(size_t line);
    void dump();
};

#endif // __cpplix_lineinfo_h