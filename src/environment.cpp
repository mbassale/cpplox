#include "environment.h"

void Environment::set(std::string identifier, ObjectPtr value) {
  values[identifier] = value;
}

ObjectPtr Environment::get(std::string identifier) {
  if (values.count(identifier) > 0) {
    return values[identifier];
  }
  if (enclosing) {
    return enclosing->get(identifier);
  }
  return NULL_OBJECT_PTR;
}