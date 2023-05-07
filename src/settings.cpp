#include "settings.h"

Settings* Settings::instance = nullptr;

Settings* Settings::getInstance() {
  if (instance == nullptr) {
    instance = new Settings();
  }
  return instance;
}