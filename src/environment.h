#ifndef __cpplox_environment_h
#define __cpplox_environment_h

#include "common.h"
#include "object.h"

class Environment {
 private:
  std::shared_ptr<Environment> enclosing;
  std::unordered_map<std::string, ObjectPtr> values;

 public:
  Environment() {}
  Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

  void set(const std::string& identifier, ObjectPtr value);
  ObjectPtr get(const std::string& identifier);
};

#endif  // __cpplox_environment_h