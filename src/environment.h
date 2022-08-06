#ifndef __cpplox_environment_h
#define __cpplox_environment_h

#include "common.h"
#include "object.h"

class Environment;
using EnvironmentPtr = std::shared_ptr<Environment>;

class Environment {
 private:
  EnvironmentPtr enclosing;
  std::unordered_map<std::string, ObjectPtr> values;

 public:
  Environment() {}
  Environment(EnvironmentPtr enclosing) : enclosing(enclosing) {}

  void set(const std::string& identifier, ObjectPtr value);
  ObjectPtr get(const std::string& identifier);
};

#endif  // __cpplox_environment_h