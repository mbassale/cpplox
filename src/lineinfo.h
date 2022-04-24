#ifndef __cpplox_lineinfo_h
#define __cpplox_lineinfo_h

#include "common.h"

struct LineEntry {
  size_t line;
  size_t count;
};

const size_t LINE_INFO_CONTINUE = 0;

class LineInfo {
  std::vector<LineEntry> lines;

 public:
  LineInfo() : lines() {}
  size_t get(size_t offset);
  void write(size_t line = LINE_INFO_CONTINUE);
  void dump();
};

#endif  // __cpplix_lineinfo_h