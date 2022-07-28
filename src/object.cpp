#include "object.h"

bool operator==(const Object &lhs, const Object &rhs) {
  return lhs.Type == rhs.Type && lhs.isEqual(rhs);
}

bool operator!=(const Object &lhs, const Object &rhs) {
  return lhs.Type != rhs.Type || !lhs.isEqual(rhs);
}