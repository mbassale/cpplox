#include "closure.h"

std::string Closure::toString() const {
  std::ostringstream ss;
  ss << "<closure " << function->toString() << ">";
  return ss.str();
}

bool Closure::isFalsey() const { return true; }

bool Closure::isTruthy() const { return false; }

bool Closure::isEqual(const Object &obj) const {
  if (typeid(obj) == typeid(*this)) {
    auto &other = reinterpret_cast<const Closure &>(obj);
    return isEqual(other);
  }
  return false;
}

bool Closure::isEqual(const Closure &other) const {
  // TODO: implement equality for chunks
  return function == other.function;
}

bool operator==(const Closure &lhs, const Closure &rhs) {
  return lhs.isEqual(rhs);
}