#include "settings.h"

namespace cpplox {

Settings* Settings::instance = nullptr;

Settings* Settings::getInstance() {
  if (instance == nullptr) {
    instance = new Settings();
  }
  return instance;
}

}  // namespace cpplox