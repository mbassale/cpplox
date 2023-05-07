#pragma once

struct SourceLocation {
  int line;
  int column;

  SourceLocation() : line(0), column(0) {}
  SourceLocation(int line, int column) : line(line), column(column) {}

  bool operator==(const SourceLocation &other) const {
    return line == other.line && column == other.column;
  }

  static SourceLocation make(int line, int column) {
    return SourceLocation(line, column);
  }
};