#include "environment.h"

bool Environment::existsInLocalScope(const std::string& identifier) {
  return values.count(identifier) > 0;
}

EnvironmentPtr Environment::findEnvironment(const std::string& identifier) {
  if (existsInLocalScope(identifier)) {
    return shared_from_this();
  }
  if (enclosing) {
    return enclosing->findEnvironment(identifier);
  }
  return nullptr;
}

void Environment::set(const std::string& identifier, ObjectPtr value) {
  if (existsInLocalScope(identifier)) {
    values[identifier] = value;
    return;
  } else if (enclosing) {
    auto parentEnv = enclosing->findEnvironment(identifier);
    if (parentEnv != nullptr) {
      parentEnv->set(identifier, value);
      return;
    }
  } 
  values[identifier] = value;
}

ObjectPtr Environment::get(const std::string& identifier) {
  if (values.count(identifier) > 0) {
    return values[identifier];
  }
  if (enclosing) {
    return enclosing->get(identifier);
  }
  return NULL_OBJECT_PTR;
}