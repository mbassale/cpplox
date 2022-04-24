#include "upvalue.h"

std::string Upvalue::toString() const {
  std::ostringstream ss;
  ss << "<upvalue>";
  return ss.str();
}

bool Upvalue::isFalsey() const { return true; }

bool Upvalue::isTruthy() const { return false; }

bool Upvalue::isEqual(const Object &obj) const {
  if (typeid(obj) == typeid(*this)) {
    auto &other = reinterpret_cast<const Upvalue &>(obj);
    return isEqual(other);
  }
  return false;
}

bool Upvalue::isEqual(const Upvalue &other) const { return false; }

bool operator==(const Upvalue &lhs, const Upvalue &rhs) {
  return lhs.isEqual(rhs);
}