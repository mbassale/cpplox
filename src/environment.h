#ifndef __cpplox_environment_h
#define __cpplox_environment_h

#include "common.h"
#include "object.h"

class Environment;
using EnvironmentPtr = std::shared_ptr<Environment>;

class Environment : public std::enable_shared_from_this<Environment> {
 private:
  EnvironmentPtr enclosing{nullptr};
  std::unordered_map<std::string, ObjectPtr> values = {};

 public:
  Environment() {}
  Environment(EnvironmentPtr enclosing) : enclosing(enclosing) {}

  void declare(const std::string& identifier, ObjectPtr value);
  void set(const std::string& identifier, ObjectPtr value);
  ObjectPtr get(const std::string& identifier);

  bool existsInLocalScope(const std::string& identifier);

  EnvironmentPtr findEnvironment(const std::string& identifier);

  std::string toString();

  static EnvironmentPtr make() { return std::make_shared<Environment>(); }
  static EnvironmentPtr make(EnvironmentPtr enclosing) {
    return std::make_shared<Environment>(enclosing);
  }
};

#endif  // __cpplox_environment_h